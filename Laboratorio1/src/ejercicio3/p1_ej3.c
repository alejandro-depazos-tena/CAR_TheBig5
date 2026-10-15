/* TermoHPC - Ejercicio 3: escalabilidad fuerte y techo de la maquina.
 * Compilar: gcc -Wall -Wextra -O2 -std=c11 -fopenmp p1_ej3.c -o p1_ej3 -lm
 * Uso: ./p1_ej3 [N=2048] [pasos=300] [repeticiones=4]
 *   Malla pequena con trabajo comparable: ./p1_ej3 256 19200
 *   (256^2 * 19200 = 2048^2 * 300 celdas-paso)
 * Salida CSV: una fila por numero de hilos. Criterio: MINIMO de las
 * repeticiones; la inicializacion va dentro del bucle de repeticiones y
 * fuera del cronometro.
 */
#include <math.h>
#include <omp.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const int HILOS[] = {1, 2, 4, 8, 12, 16, 20, 24};
#define NUM_HILOS (sizeof(HILOS) / sizeof(HILOS[0]))

/* Trabajo aritmetico de cada hilo en el experimento del techo. */
#define TRABAJO_TECHO 200000000L

static size_t bytes(size_t n) { return n * n * sizeof(double); }

static void init(double *a, double *b, size_t n)
{
    memset(a, 0, bytes(n));
    memset(b, 0, bytes(n));
    for (size_t j = 0; j < n; ++j)
        a[j] = b[j] = 100.0;
}

static double checksum(const double *a, size_t n)
{
    double s = 0.0;
    for (size_t i = 1; i < n - 1; ++i)
        for (size_t j = 1; j < n - 1; ++j)
            s += a[i * n + j];
    return s;
}

static double step_seq(const double *a, double *b, size_t n)
{
    double r = 0.0;
    for (size_t i = 1; i < n - 1; ++i)
        for (size_t j = 1; j < n - 1; ++j) {
            size_t k = i * n + j;
            b[k] = 0.25 * (a[k - n] + a[k + n] + a[k - 1] + a[k + 1]);
            double d = fabs(b[k] - a[k]);
            if (d > r) r = d;
        }
    return r;
}

/* Version del Ejercicio 2, sin cambios. */
static double step_omp(const double *a, double *b, size_t n)
{
    double r = 0.0;
#pragma omp parallel for default(none) shared(a, b, n) reduction(max : r) schedule(static)
    for (size_t i = 1; i < n - 1; ++i)
        for (size_t j = 1; j < n - 1; ++j) {
            size_t k = i * n + j;
            double v = 0.25 * (a[k - n] + a[k + n] + a[k - 1] + a[k + 1]);
            b[k] = v;
            double d = fabs(v - a[k]);
            if (d > r) r = d;
        }
    return r;
}

/* Mejor de 'reps' ejecuciones; deja en *cs el checksum y en *res el ultimo
 * residuo. Hay que CONSUMIR el residuo: si nadie lo usa, -O2 elimina su
 * calculo en la version secuencial (no en la paralela, por la reduccion) y
 * T1 saldria artificialmente barato. */
static double mide(double *a, double *b, size_t n, size_t steps, int reps,
                   int paralelo, double *cs, double *res)
{
    double mejor = 1e30;
    for (int rep = 0; rep < reps; ++rep) {
        init(a, b, n);
        double *u = a, *v = b;
        double r = 0.0;
        double t0 = omp_get_wtime();
        for (size_t s = 0; s < steps; ++s) {
            r = paralelo ? step_omp(u, v, n) : step_seq(u, v, n);
            double *tmp = u; u = v; v = tmp;
        }
        double dt = omp_get_wtime() - t0;
        if (dt < mejor) mejor = dt;
        *cs = checksum(u, n);
        *res = r;
    }
    return mejor;
}

/* Techo de calculo (apartado IV.C): CADA hilo hace el mismo trabajo
 * aritmetico, sin memoria. En una maquina homogenea el tiempo seria plano.
 * Devuelve el mejor tiempo y deja la suma en *sink para que no se elimine. */
static double mide_techo(int reps, double *sink)
{
    double mejor = 1e30;
    for (int rep = 0; rep < reps; ++rep) {
        double suma = 0.0;
        double t0 = omp_get_wtime();
#pragma omp parallel default(none) reduction(+ : suma)
        {
            double x = 1.0 + 1e-9 * omp_get_thread_num();
            for (long w = 0; w < TRABAJO_TECHO; ++w)
                x = x * 1.0000001 + 1e-7; /* cadena dependiente */
            suma += x;
        }
        double dt = omp_get_wtime() - t0;
        if (dt < mejor) mejor = dt;
        *sink += suma;
    }
    return mejor;
}

/* Coste fijo de un paso vacio: abrir el equipo, repartir un bucle trivial
 * de N-2 filas y la barrera final. Sirve para separar, en la malla pequena,
 * el coste de sincronizar del coste de calcular. Devuelve segundos. */
static double mide_region_vacia(size_t n, int reps)
{
    const int veces = 2000;
    double mejor = 1e30;
    for (int rep = 0; rep < reps; ++rep) {
        double t0 = omp_get_wtime();
        for (int v = 0; v < veces; ++v) {
#pragma omp parallel for default(none) shared(n) schedule(static)
            for (size_t i = 1; i < n - 1; ++i)
                __asm__ volatile("" ::: "memory"); /* cuerpo vacio no eliminable */
        }
        double dt = (omp_get_wtime() - t0) / veces;
        if (dt < mejor) mejor = dt;
    }
    return mejor;
}

static int hilos_reales(void)
{
    int reales = 0;
#pragma omp parallel default(none) shared(reales)
    {
#pragma omp single
        reales = omp_get_num_threads();
    }
    return reales;
}

int main(int argc, char **argv)
{
    size_t n = argc > 1 ? strtoull(argv[1], NULL, 10) : 2048;
    size_t steps = argc > 2 ? strtoull(argv[2], NULL, 10) : 300;
    int reps = argc > 3 ? atoi(argv[3]) : 4;
    if (n < 3 || steps == 0 || reps < 1) {
        fprintf(stderr, "Uso: %s [N>=3] [pasos>0] [repeticiones>0]\n", argv[0]);
        return EXIT_FAILURE;
    }

    double *a = malloc(bytes(n)), *b = malloc(bytes(n));
    if (!a || !b) {
        fprintf(stderr, "No se pudo reservar la malla.\n");
        return EXIT_FAILURE;
    }
    omp_set_dynamic(0);

    /* T1 para el speedup ABSOLUTO: version secuencial, sin pragmas. */
    double cs_seq, res_seq;
    double t_seq = mide(a, b, n, steps, reps, 0, &cs_seq, &res_seq);

    double sink = 0.0, t_techo1 = 0.0;
    printf("# N=%zu pasos=%zu repeticiones=%d criterio=minimo T1_secuencial=%.9f "
           "residuo=%.17g\n", n, steps, reps, t_seq, res_seq);
    printf("hilos,hilos_reales,N,pasos,tiempo_s,speedup_abs,eficiencia,"
           "s_por_paso,checksum_ok,tiempo_techo_s,techo_speedup,"
           "region_vacia_s\n");
    for (size_t q = 0; q < NUM_HILOS; ++q) {
        int p = HILOS[q];
        omp_set_num_threads(p);
        int reales = hilos_reales();

        double cs, res;
        double t = mide(a, b, n, steps, reps, 1, &cs, &res);
        /* El maximo no depende del orden: el residuo debe coincidir exacto. */
        int cs_ok = fabs(cs - cs_seq) <= 1e-9 * fabs(cs_seq) && res == res_seq;

        double t_techo = mide_techo(reps, &sink);
        if (p == 1) t_techo1 = t_techo;
        /* p hilos hacen p veces el trabajo de uno: techo = p*T1/Tp. */
        double techo = p * t_techo1 / t_techo;

        double region = mide_region_vacia(n, reps);

        printf("%d,%d,%zu,%zu,%.9f,%.4f,%.4f,%.6e,%d,%.6f,%.4f,%.6e\n", p, reales,
               n, steps, t, t_seq / t, t_seq / (t * p), t / steps, cs_ok, t_techo,
               techo, region);
    }
    fprintf(stderr, "(sumidero del techo: %g)\n", sink);
    free(a); free(b);
    return EXIT_SUCCESS;
}
