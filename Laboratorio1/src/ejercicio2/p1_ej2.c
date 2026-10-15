/* TermoHPC - Ejercicio 2: paralelizacion con OpenMP y verificacion.
 * Compilar: gcc -Wall -Wextra -O2 -std=c11 -fopenmp p1_ej2.c -o p1_ej2 -lm
 * Uso: ./p1_ej2 [N=2048] [pasos=300]
 * Imprime la tabla de verificacion con 1, 2, 4, 8, 12, 16, 20 y 24 hilos
 * y, despues, el fallo provocado (actualizacion in situ).
 */
#include <math.h>
#include <omp.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Tolerancia declarada ANTES de medir: con double y campos de orden 100,
 * 1e-9 detecta cualquier error real y deja margen a la reordenacion. */
#define TOLERANCIA 1e-9

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

/* Referencia secuencial: el mismo paso que el Ejercicio 1. */
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

/* Paso paralelo. Se reparte el bucle de FILAS (memoria contigua por hilo).
 * a, b, n: compartidas (solo se lee a y cada hilo escribe filas distintas
 *          de b, asi que no hay carrera).
 * i, j, k, v, d: privadas por estar declaradas dentro del bucle.
 * r: reduction(max) -> cada hilo tiene su copia y se combinan al final.
 * La barrera implicita al final del for separa un paso del siguiente. */
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

/* Ejecuta 'steps' pasos con intercambio de punteros; devuelve el residuo. */
static double run(double **a, double **b, size_t n, size_t steps, int paralelo)
{
    double r = 0.0;
    for (size_t s = 0; s < steps; ++s) {
        r = paralelo ? step_omp(*a, *b, n) : step_seq(*a, *b, n);
        double *tmp = *a; *a = *b; *b = tmp;
    }
    return r;
}

/* FALLO PROVOCADO: un solo buffer. Las vecinas de la frontera entre franjas
 * pueden estar ya actualizadas o no segun el reparto -> depende de los hilos. */
static void in_situ(double *u, size_t n, size_t steps)
{
    for (size_t s = 0; s < steps; ++s) {
#pragma omp parallel for default(none) shared(u, n) schedule(static)
        for (size_t i = 1; i < n - 1; ++i)
            for (size_t j = 1; j < n - 1; ++j) {
                size_t k = i * n + j;
                u[k] = 0.25 * (u[k - n] + u[k + n] + u[k - 1] + u[k + 1]);
            }
    }
}

/* Norma del maximo de la diferencia celda a celda. */
static double difference(const double *a, const double *b, size_t n)
{
    double dmax = 0.0;
    for (size_t k = 0; k < n * n; ++k) {
        double d = fabs(a[k] - b[k]);
        if (d > dmax) dmax = d;
    }
    return dmax;
}

static double checksum(const double *a, size_t n)
{
    double s = 0.0;
    for (size_t i = 1; i < n - 1; ++i)
        for (size_t j = 1; j < n - 1; ++j)
            s += a[i * n + j];
    return s;
}

/* Bordes intactos y temperaturas en [0, 100]. */
static int checks(const double *a, size_t n)
{
    for (size_t j = 0; j < n; ++j)
        if (a[j] != 100.0 || a[(n - 1) * n + j] != 0.0) return 0;
    for (size_t i = 1; i < n - 1; ++i)
        if (a[i * n] != 0.0 || a[i * n + n - 1] != 0.0) return 0;
    for (size_t k = 0; k < n * n; ++k)
        if (!isfinite(a[k]) || a[k] < 0.0 || a[k] > 100.0) return 0;
    return 1;
}

/* Hilos que hay de verdad dentro de una region paralela (apartado IV.D). */
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
    if (n < 3 || steps == 0) {
        fprintf(stderr, "Uso: %s [N>=3] [pasos>0]\n", argv[0]);
        return EXIT_FAILURE;
    }

    double *seq_a = malloc(bytes(n)), *seq_b = malloc(bytes(n));
    double *par_a = malloc(bytes(n)), *par_b = malloc(bytes(n));
    if (!seq_a || !seq_b || !par_a || !par_b) {
        fprintf(stderr, "No se pudo reservar la malla.\n");
        return EXIT_FAILURE;
    }
    omp_set_dynamic(0);

    /* Referencia secuencial, calculada una sola vez. */
    init(seq_a, seq_b, n);
    double r_seq = run(&seq_a, &seq_b, n, steps, 0);
    double cs_seq = checksum(seq_a, n);

    printf("# N=%zu pasos=%zu tolerancia=%.0e\n", n, steps, TOLERANCIA);
    printf("tipo,hilos_pedidos,hilos_reales,max_diff,checksum,dif_checksum_rel,"
           "residuo,bit_a_bit,correcto\n");
    printf("secuencial,1,1,0,%.17g,0,%.17g,1,%d\n", cs_seq, r_seq, checks(seq_a, n));

    int todo_ok = 1;
    for (size_t q = 0; q < NUM_HILOS; ++q) {
        omp_set_num_threads(HILOS[q]);
        int reales = hilos_reales();
        init(par_a, par_b, n);
        double r_par = run(&par_a, &par_b, n, steps, 1);
        double diff = difference(seq_a, par_a, n);
        double cs = checksum(par_a, n);
        double dcs = fabs(cs - cs_seq) / fabs(cs_seq);
        int ok = reales == HILOS[q] && diff <= TOLERANCIA && dcs <= TOLERANCIA
                 && checks(par_a, n);
        todo_ok &= ok;
        printf("jacobi_omp,%d,%d,%.3e,%.17g,%.3e,%.17g,%d,%d\n", HILOS[q], reales,
               diff, cs, dcs, r_par, diff == 0.0, ok);
    }

    /* Fallo provocado: la verificacion debe rechazarlo y su resultado debe
     * cambiar con el numero de hilos (comparese el checksum entre filas). */
    for (size_t q = 0; q < NUM_HILOS; ++q) {
        omp_set_num_threads(HILOS[q]);
        int reales = hilos_reales();
        init(par_a, par_b, n);
        in_situ(par_a, n, steps);
        double diff = difference(seq_a, par_a, n);
        double cs = checksum(par_a, n);
        printf("in_situ,%d,%d,%.3e,%.17g,%.3e,NA,%d,%d\n", HILOS[q], reales, diff,
               cs, fabs(cs - cs_seq) / fabs(cs_seq), diff == 0.0,
               diff <= TOLERANCIA);
    }

    printf("# verificacion Jacobi paralelo: %s\n", todo_ok ? "PASA" : "FALLA");
    free(seq_a); free(seq_b); free(par_a); free(par_b);
    return todo_ok ? EXIT_SUCCESS : EXIT_FAILURE;
}
