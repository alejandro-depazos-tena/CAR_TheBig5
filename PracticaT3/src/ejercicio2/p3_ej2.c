/* p3_ej2.c - un bucle desequilibrado y las planificaciones (apartado 3.2.4). */
#include <stdio.h>
#include <omp.h>
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

/* Ejecuta el bucle real y suma el coste i de cada iteracion en su hilo. */
static double reparto(omp_sched_t tipo, int bloque)
{
    double carga[MAXH] = {0.0};
    int hilos = 0;

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
        for (int i = 0; i < N; i++)
            carga[omp_get_thread_num()] += (double)i;
    }

    double total = 0.0;
    double maxima = 0.0;
    int sin_trabajo = 0;
    for (int i = 0; i < hilos; i++) {
        total += carga[i];
        if (carga[i] > maxima)
            maxima = carga[i];
        if (carga[i] == 0.0)
            sin_trabajo++;
    }

    double media = total / (double)hilos;
    double factor = maxima / media;
    printf("mas cargado %5.2f veces la media   hilos sin trabajo %d\n",
           factor, sin_trabajo);
    return factor;
}

int main(void)
{
    struct {
        const char *nombre;
        omp_sched_t tipo;
        int bloque;
    } planificaciones[] = {
        {"static (por defecto)", omp_sched_static, 0},
        {"static, 1", omp_sched_static, 1},
        {"dynamic (bloque 1)", omp_sched_dynamic, 1},
        {"dynamic, 64", omp_sched_dynamic, 64},
        {"guided", omp_sched_guided, 0}
    };

    int n_planificaciones =
        (int)(sizeof(planificaciones) / sizeof(planificaciones[0]));
    printf("%d hilos, %d iteraciones, mejor de %d\n",
           omp_get_max_threads(), N, REP);

    double ts = 1e30;
    for (int rep = 0; rep < REP; rep++) {
        double t = secuencial();
        if (t < ts)
            ts = t;
    }

    double control = 0.0;
    for (int i = 0; i < N; i++)
        control += r[i];
    printf("%-22s %8.4f s  %5.2fx\n", "secuencial", ts, 1.0);

    for (int k = 0; k < n_planificaciones; k++) {
        double t = paralelo(planificaciones[k].tipo, planificaciones[k].bloque);
        printf("%-22s %8.4f s  %5.2fx\n",
               planificaciones[k].nombre, t, ts / t);
    }

    double comprueba = 0.0;
    for (int i = 0; i < N; i++)
        comprueba += r[i];
    printf("resultado %s\n",
           comprueba == control ? "identico al secuencial" : "DISTINTO");

    printf("reparto del trabajo entre los %d hilos\n", omp_get_max_threads());
    for (int k = 0; k < n_planificaciones; k++) {
        printf("%-22s", planificaciones[k].nombre);
        reparto(planificaciones[k].tipo, planificaciones[k].bloque);
    }
    return 0;
}
