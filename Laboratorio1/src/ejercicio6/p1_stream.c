/* TermoHPC - Techo de ancho de banda de la maquina (apartado V.G).
 * Triad de STREAM: a[i] = b[i] + f*c[i] sobre vectores que no caben en cache.
 * Compilar: gcc -Wall -Wextra -O2 -std=c11 -fopenmp p1_stream.c -o p1_stream -lm
 * Uso: ./p1_stream [elementos=40000000] [repeticiones=10]
 * Modelos de trafico: 24 B/elemento (2 lecturas + 1 escritura) y 32 B
 * (ademas, la lectura previa de la linea que se escribe).
 * Criterio: minimo de las repeticiones.
 */
#include <omp.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

static const int HILOS[] = {1, 2, 4, 8, 12, 16, 20, 24};
#define NUM_HILOS (sizeof(HILOS) / sizeof(HILOS[0]))

int main(int argc, char **argv)
{
    size_t n = argc > 1 ? strtoull(argv[1], NULL, 10) : 40000000;
    int reps = argc > 2 ? atoi(argv[2]) : 10;
    if (n == 0 || reps < 1) {
        fprintf(stderr, "Uso: %s [elementos>0] [repeticiones>0]\n", argv[0]);
        return EXIT_FAILURE;
    }
    double *a = malloc(n * sizeof(double));
    double *b = malloc(n * sizeof(double));
    double *c = malloc(n * sizeof(double));
    if (!a || !b || !c) {
        fprintf(stderr, "No hay memoria para los tres vectores.\n");
        return EXIT_FAILURE;
    }
    omp_set_dynamic(0);
    const double f = 3.0;

    printf("# elementos=%zu MB_totales=%.0f repeticiones=%d criterio=minimo\n",
           n, 3.0 * n * sizeof(double) / 1e6, reps);
    printf("hilos,hilos_reales,tiempo_s,GB_s_24B,GB_s_32B,comprobacion\n");
    for (size_t q = 0; q < NUM_HILOS; ++q) {
        int p = HILOS[q];
        omp_set_num_threads(p);
        int reales = 0;
#pragma omp parallel default(none) shared(reales)
        {
#pragma omp single
            reales = omp_get_num_threads();
        }
        /* Inicializacion con el mismo reparto que la medida. */
#pragma omp parallel for default(none) shared(a, b, c, n) schedule(static)
        for (size_t i = 0; i < n; ++i) {
            a[i] = 0.0;
            b[i] = 1.0;
            c[i] = 2.0;
        }
        double mejor = 1e30;
        for (int r = 0; r < reps; ++r) {
            double t0 = omp_get_wtime();
#pragma omp parallel for default(none) shared(a, b, c, n, f) schedule(static)
            for (size_t i = 0; i < n; ++i)
                a[i] = b[i] + f * c[i];
            double dt = omp_get_wtime() - t0;
            if (dt < mejor) mejor = dt;
        }
        /* Consumir el resultado: todos deben valer 1 + 3*2 = 7. */
        double s = a[0] + a[n / 2] + a[n - 1];
        printf("%d,%d,%.6f,%.2f,%.2f,%g\n", p, reales, mejor,
               24.0 * n / mejor / 1e9, 32.0 * n / mejor / 1e9, s / 3.0);
    }
    free(a); free(b); free(c);
    return EXIT_SUCCESS;
}
