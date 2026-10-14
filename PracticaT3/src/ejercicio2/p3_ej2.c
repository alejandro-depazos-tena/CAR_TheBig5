/* p3_ej2.c - un bucle desequilibrado y las planificaciones (apartado 3.2.4). */
#include <stdio.h>
#include <omp.h>
#ifndef _WIN32
#include <unistd.h>
#endif
#include "crono.h"

#define N 20000
#define REP 5
#define MAXH 64

static double r[N];

/* La iteracion i cuesta i sumas en coma flotante. */
static double trabajo(int i)
{
    double s = 0.0;
    for (int k = 0; k < i; k++)
        s += (double)k * 1e-9;
    return s;
}

static double secuencial(void)
{
    double t0 = reloj();
    for (int i = 0; i < N; i++)
        r[i] = trabajo(i);
    return reloj() - t0;
}

static double paralelo(omp_sched_t tipo, int bloque)
{
    omp_set_schedule(tipo, bloque);
    double mejor = 1e30;

    for (int rep = 0; rep < REP; rep++) {
        double t0 = reloj();
#pragma omp parallel for schedule(runtime)
        for (int i = 0; i < N; i++)
            r[i] = trabajo(i);
        double t = reloj() - t0;
        if (t < mejor)
            mejor = t;
    }
    return mejor;
}

/* Ejecuta el bucle real (con trabajo(i)) y suma el coste i de cada
   iteracion en la casilla del hilo que la ejecuta. Cada hilo escribe solo
   en su casilla, asi que no hay carrera. Deja en millones[] el trabajo de
   cada hilo y en *sin_trabajo los hilos que no han recibido nada. */
static double reparto(omp_sched_t tipo, int bloque, double millones[MAXH],
                      int *sin_trabajo)
{
    long long carga[MAXH] = {0};
    int hilos = 0;

    *sin_trabajo = 0;
    if (omp_get_max_threads() > MAXH) {
        fprintf(stderr, "Demasiados hilos configurados: %d (maximo %d)\n",
                omp_get_max_threads(), MAXH);
        return 0.0;
    }

    omp_set_schedule(tipo, bloque);
#pragma omp parallel
    {
#pragma omp single
        hilos = omp_get_num_threads();

#pragma omp for schedule(runtime)
        for (int i = 0; i < N; i++) {
            r[i] = trabajo(i);
            carga[omp_get_thread_num()] += i;
        }
    }

    long long total = 0;
    long long maxima = 0;
    for (int h = 0; h < hilos; h++) {
        total += carga[h];
        if (carga[h] > maxima)
            maxima = carga[h];
        if (carga[h] == 0)
            (*sin_trabajo)++;
        millones[h] = (double)carga[h] / 1e6;
    }

    double media = (double)total / (double)hilos;
    return (double)maxima / media;
}

/* Colores ANSI solo si la salida es una terminal (no al redirigir). */
static const char *NEGRITA = "", *CIAN = "", *VERDE = "", *ROJO = "",
                  *TENUE = "", *FIN = "";

static void activa_colores(void)
{
#ifndef _WIN32
    if (isatty(STDOUT_FILENO)) {
        NEGRITA = "\033[1m";
        CIAN = "\033[1;36m";
        VERDE = "\033[1;32m";
        ROJO = "\033[1;31m";
        TENUE = "\033[2m";
        FIN = "\033[0m";
    }
#endif
}

static void linea(int ancho)
{
    printf("  %s", TENUE);
    for (int i = 0; i < ancho; i++)
        putchar('-');
    printf("%s\n", FIN);
}

static void titulo(const char *texto)
{
    printf("\n  %s%s%s\n", CIAN, texto, FIN);
}

int main(void)
{
    struct {
        const char *nombre;
        const char *corto;
        omp_sched_t tipo;
        int bloque;
    } planificaciones[] = {
        {"static (por defecto)", "static", omp_sched_static, 0},
        {"static, 1", "static,1", omp_sched_static, 1},
        {"dynamic (bloque 1)", "dynamic", omp_sched_dynamic, 1},
        {"dynamic, 64", "dyn,64", omp_sched_dynamic, 64},
        {"guided", "guided", omp_sched_guided, 0}
    };
    enum { NPLAN = sizeof(planificaciones) / sizeof(planificaciones[0]) };

    activa_colores();
    int hilos = omp_get_max_threads();

    printf("\n");
    linea(62);
    printf("  %s Practica 3 - Ejercicio 2: bucle desequilibrado%s\n",
           NEGRITA, FIN);
    printf("  %d hilos | %d iteraciones | mejor de %d repeticiones\n",
           hilos, N, REP);
    linea(62);

    double ts = 1e30;
    for (int rep = 0; rep < REP; rep++) {
        double t = secuencial();
        if (t < ts)
            ts = t;
    }

    double control = 0.0;
    for (int i = 0; i < N; i++)
        control += r[i];

    double tiempos[NPLAN];
    int mejor = 0;
    int peor = 0;
    for (int k = 0; k < NPLAN; k++) {
        tiempos[k] = paralelo(planificaciones[k].tipo,
                              planificaciones[k].bloque);
        if (tiempos[k] < tiempos[mejor])
            mejor = k;
        if (tiempos[k] > tiempos[peor])
            peor = k;
    }

    titulo("TIEMPOS");
    linea(46);
    printf("  %s%-22s %12s %10s%s\n", NEGRITA, "planificacion", "tiempo (s)",
           "speedup", FIN);
    linea(46);
    printf("  %-22s %12.4f %9.2fx\n", "secuencial", ts, 1.0);
    for (int k = 0; k < NPLAN; k++) {
        const char *color = k == mejor ? VERDE : (k == peor ? ROJO : "");
        printf("  %s%-22s %12.4f %9.2fx%s%s\n", color,
               planificaciones[k].nombre, tiempos[k], ts / tiempos[k], FIN,
               k == mejor ? "  <- la mas rapida"
                          : (k == peor ? "  <- la mas lenta" : ""));
    }
    linea(46);

    double comprueba = 0.0;
    for (int i = 0; i < N; i++)
        comprueba += r[i];
    if (comprueba == control)
        printf("  resultado: %sidentico al secuencial%s\n", VERDE, FIN);
    else
        printf("  resultado: %sDISTINTO%s\n", ROJO, FIN);

    double millones[NPLAN][MAXH];
    double factores[NPLAN];
    int sin_trabajo[NPLAN];
    for (int k = 0; k < NPLAN; k++)
        factores[k] = reparto(planificaciones[k].tipo,
                              planificaciones[k].bloque, millones[k],
                              &sin_trabajo[k]);

    titulo("REPARTO DEL TRABAJO");
    linea(56);
    printf("  %s%-22s %20s %12s%s\n", NEGRITA, "planificacion",
           "mas cargado/media", "sin trabajo", FIN);
    linea(56);
    for (int k = 0; k < NPLAN; k++)
        printf("  %-22s %19.2fx %12d\n", planificaciones[k].nombre,
               factores[k], sin_trabajo[k]);
    linea(56);

    titulo("TRABAJO DE CADA HILO (millones de sumas)");
    linea(8 + 10 * NPLAN);
    printf("  %s%6s  ", NEGRITA, "hilo");
    for (int k = 0; k < NPLAN; k++)
        printf("%10s", planificaciones[k].corto);
    printf("%s\n", FIN);
    linea(8 + 10 * NPLAN);
    for (int h = 0; h < hilos && h < MAXH; h++) {
        printf("  %6d  ", h);
        for (int k = 0; k < NPLAN; k++)
            printf("%10.2f", millones[k][h]);
        printf("\n");
    }
    linea(8 + 10 * NPLAN);
    printf("\n");
    return 0;
}
