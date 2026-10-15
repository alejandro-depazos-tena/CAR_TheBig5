/* TermoHPC - Ejercicio 4: escalabilidad debil.
 * Compilar: gcc -Wall -Wextra -O2 -std=c11 -fopenmp p1_ej4.c -o p1_ej4 -lm
 * Uso: ./p1_ej4 [N1=1024] [pasos=300] [repeticiones=4]
 * N(p) = round(N1 * sqrt(p)): el trabajo de un paso va con N^2, asi que
 * para conservar las celdas por hilo N se multiplica por sqrt(p), no por p.
 * Criterio: minimo de las repeticiones.
 */
#include <math.h>
#include <omp.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const int HILOS[] = {1, 2, 4, 8, 12, 16, 20, 24};
#define NUM_HILOS (sizeof(HILOS) / sizeof(HILOS[0]))

static size_t bytes(size_t n) { return n * n * sizeof(double); }

static void init(double *a, double *b, size_t n)
{
    memset(a, 0, bytes(n));
    memset(b, 0, bytes(n));
    for (size_t j = 0; j < n; ++j)
        a[j] = b[j] = 100.0;
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

static double checksum(const double *a, size_t n)
{
    double s = 0.0;
    for (size_t i = 1; i < n - 1; ++i)
        for (size_t j = 1; j < n - 1; ++j)
            s += a[i * n + j];
    return s;
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
    size_t n1 = argc > 1 ? strtoull(argv[1], NULL, 10) : 1024;
    size_t steps = argc > 2 ? strtoull(argv[2], NULL, 10) : 300;
    int reps = argc > 3 ? atoi(argv[3]) : 4;
    if (n1 < 3 || steps == 0 || reps < 1) {
        fprintf(stderr, "Uso: %s [N1>=3] [pasos>0] [repeticiones>0]\n", argv[0]);
        return EXIT_FAILURE;
    }
    omp_set_dynamic(0);

    double t1 = 0.0;
    printf("# N1=%zu pasos=%zu repeticiones=%d criterio=minimo\n", n1, steps, reps);
    printf("hilos,hilos_reales,N,celdas_por_hilo,pasos,tiempo_s,eficiencia_debil,"
           "MB_dos_buffers,checksum\n");
    for (size_t q = 0; q < NUM_HILOS; ++q) {
        int p = HILOS[q];
        size_t n = (size_t)llround((double)n1 * sqrt((double)p));
        double *a = malloc(bytes(n)), *b = malloc(bytes(n));
        if (!a || !b) {
            fprintf(stderr, "No hay memoria para N=%zu.\n", n);
            return EXIT_FAILURE;
        }
        omp_set_num_threads(p);
        int reales = hilos_reales();

        double mejor = 1e30, cs = 0.0;
        for (int rep = 0; rep < reps; ++rep) {
            init(a, b, n);
            double *u = a, *v = b;
            double t0 = omp_get_wtime();
            for (size_t s = 0; s < steps; ++s) {
                step_omp(u, v, n);
                double *tmp = u; u = v; v = tmp;
            }
            double dt = omp_get_wtime() - t0;
            if (dt < mejor) mejor = dt;
            cs = checksum(u, n);
        }
        if (p == 1) t1 = mejor;

        printf("%d,%d,%zu,%.0f,%zu,%.9f,%.4f,%.1f,%.17g\n", p, reales, n,
               (double)n * n / p, steps, mejor, t1 / mejor,
               2.0 * bytes(n) / 1e6, cs);
        free(a); free(b);
    }
    return EXIT_SUCCESS;
}
