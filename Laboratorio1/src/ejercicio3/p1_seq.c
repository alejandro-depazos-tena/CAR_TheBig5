/* TermoHPC - T1 de referencia para el speedup ABSOLUTO (apartado V.B).
 * Es la mejor version secuencial (el paso y el residuo en un solo recorrido,
 * igual que step_seq en los demas programas) compilada SIN -fopenmp: no
 * incluye omp.h ni usa ninguna funcion de OpenMP.
 * Compilar: gcc -Wall -Wextra -O2 -std=c11 p1_seq.c -o p1_seq -lm
 * Uso: ./p1_seq [N=2048] [pasos=300] [repeticiones=10]
 * Reloj de pared: clock_gettime(CLOCK_MONOTONIC), porque omp_get_wtime() no
 * existe sin OpenMP (clock() no vale: mide tiempo de CPU, no de pared).
 * Imprime el tiempo de cada repeticion y el minimo (criterio de las tablas),
 * la mediana y el maximo: la diferencia entre ellos es la dispersion de la
 * maquina, que hay que conocer antes de interpretar cualquier speedup.
 */
#define _POSIX_C_SOURCE 199309L
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static size_t bytes(size_t n) { return n * n * sizeof(double); }

static double reloj(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + 1e-9 * (double)ts.tv_nsec;
}

static void init(double *a, double *b, size_t n)
{
    memset(a, 0, bytes(n));
    memset(b, 0, bytes(n));
    for (size_t j = 0; j < n; ++j)
        a[j] = b[j] = 100.0;
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

static double checksum(const double *a, size_t n)
{
    double s = 0.0;
    for (size_t i = 1; i < n - 1; ++i)
        for (size_t j = 1; j < n - 1; ++j)
            s += a[i * n + j];
    return s;
}

static int compara(const void *x, const void *y)
{
    double a = *(const double *)x, b = *(const double *)y;
    return (a > b) - (a < b);
}

int main(int argc, char **argv)
{
    size_t n = argc > 1 ? strtoull(argv[1], NULL, 10) : 2048;
    size_t steps = argc > 2 ? strtoull(argv[2], NULL, 10) : 300;
    int reps = argc > 3 ? atoi(argv[3]) : 10;
    if (n < 3 || steps == 0 || reps < 1) {
        fprintf(stderr, "Uso: %s [N>=3] [pasos>0] [repeticiones>0]\n", argv[0]);
        return EXIT_FAILURE;
    }
    double *a = malloc(bytes(n)), *b = malloc(bytes(n));
    double *t = malloc((size_t)reps * sizeof(double));
    if (!a || !b || !t) {
        fprintf(stderr, "No se pudo reservar la malla.\n");
        return EXIT_FAILURE;
    }

    double r = 0.0, cs = 0.0;
    printf("# Secuencial compilada sin -fopenmp. N=%zu pasos=%zu repeticiones=%d\n",
           n, steps, reps);
    printf("repeticion,tiempo_s\n");
    for (int rep = 0; rep < reps; ++rep) {
        init(a, b, n);                       /* fuera del cronometro */
        double *u = a, *v = b;
        double t0 = reloj();
        for (size_t s = 0; s < steps; ++s) {
            r = step_seq(u, v, n);
            double *x = u; u = v; v = x;     /* intercambio de punteros */
        }
        t[rep] = reloj() - t0;
        cs = checksum(u, n);                 /* consume el resultado */
        printf("%d,%.9f\n", rep + 1, t[rep]);
    }
    qsort(t, (size_t)reps, sizeof(double), compara);
    double mediana = reps % 2 ? t[reps / 2] : 0.5 * (t[reps / 2 - 1] + t[reps / 2]);
    printf("# T1_minimo=%.9f T1_mediana=%.9f T1_maximo=%.9f dispersion_max_min=%.1f%% "
           "ns_por_celda_y_paso=%.4f checksum=%.17g residuo=%.17g\n",
           t[0], mediana, t[reps - 1], 100.0 * (t[reps - 1] / t[0] - 1.0),
           1e9 * t[0] / ((double)n * (double)n * (double)steps), cs, r);
    free(a); free(b); free(t);
    return EXIT_SUCCESS;
}
