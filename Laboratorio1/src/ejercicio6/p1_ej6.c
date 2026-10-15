/* TermoHPC - Ejercicio 6: optimizacion guiada por medicion.
 * Compilar: gcc -Wall -Wextra -O2 -std=c11 -fopenmp p1_ej6.c -o p1_ej6 -lm
 * Uso: ./p1_ej6 [N=2048] [pasos=300] [hilos=12] [repeticiones=4]
 * Cada variante cambia UNA sola cosa respecto de la base (Ejercicio 2) y
 * se verifica contra la version secuencial antes de dar su tiempo.
 *   base           : parallel for dentro del bucle temporal
 *   collapse2      : igual, con collapse(2)
 *   region_unica   : una sola region parallel para todos los pasos
 *   residuo_cada_10: el residuo solo se calcula cada 10 pasos
 * Salvo la ultima, todas calculan el residuo en cada paso.
 */
#include <math.h>
#include <omp.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOLERANCIA 1e-9
#define RESIDUO_CADA 10

static size_t bytes(size_t n) { return n * n * sizeof(double); }

static void init(double *a, double *b, size_t n)
{
    memset(a, 0, bytes(n));
    memset(b, 0, bytes(n));
    for (size_t j = 0; j < n; ++j)
        a[j] = b[j] = 100.0;
}

static double diff(const double *a, const double *b, size_t n)
{
    double m = 0.0;
    for (size_t k = 0; k < n * n; ++k) {
        double d = fabs(a[k] - b[k]);
        if (d > m) m = d;
    }
    return m;
}

/* Referencia secuencial: hace el MISMO trabajo que la paralela (residuo
 * incluido), para que T1 sea comparable. */
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

/* Base: la version del Ejercicio 2. */
static double step(const double *a, double *b, size_t n)
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

/* Cambio unico: collapse(2) reparte las (N-2)^2 celdas en vez de las filas. */
static double collapse_step(const double *a, double *b, size_t n)
{
    double r = 0.0;
#pragma omp parallel for default(none) shared(a, b, n) reduction(max : r) collapse(2) schedule(static)
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

/* Cambio unico: el paso sin residuo (sin fabs, sin comparacion y sin
 * reduccion). Se usa en los pasos en que no se necesita el residuo. */
static void step_sin_residuo(const double *a, double *b, size_t n)
{
#pragma omp parallel for default(none) shared(a, b, n) schedule(static)
    for (size_t i = 1; i < n - 1; ++i)
        for (size_t j = 1; j < n - 1; ++j) {
            size_t k = i * n + j;
            b[k] = 0.25 * (a[k - n] + a[k + n] + a[k - 1] + a[k + 1]);
        }
}

/* Cambio unico: el equipo se crea una vez. Cada hilo intercambia SU copia
 * privada de los punteros (todas avanzan igual), asi no hace falta single
 * para el intercambio. Barreras por paso: la del single que pone r a cero y
 * la implicita del for, que es la que separa dos pasos y no se puede quitar. */
static double run_region_unica(double *a0, double *b0, size_t n, size_t steps)
{
    double r = 0.0;
#pragma omp parallel default(none) shared(a0, b0, n, steps, r)
    {
        double *u = a0, *v = b0;
        for (size_t s = 0; s < steps; ++s) {
#pragma omp single
            r = 0.0;
#pragma omp for schedule(static) reduction(max : r)
            for (size_t i = 1; i < n - 1; ++i)
                for (size_t j = 1; j < n - 1; ++j) {
                    size_t k = i * n + j;
                    double val = 0.25 * (u[k - n] + u[k + n] + u[k - 1] + u[k + 1]);
                    v[k] = val;
                    double d = fabs(val - u[k]);
                    if (d > r) r = d;
                }
            double *tmp = u; u = v; v = tmp;
        }
    }
    return r;
}

/* Mejor de 'reps'; deja en *final la malla resultante para verificarla. */
static double measure(double *a, double *b, size_t n, size_t steps, int mode,
                      int reps, const double **final)
{
    double mejor = 1e30;
    for (int rep = 0; rep < reps; ++rep) {
        init(a, b, n);
        double *u = a, *v = b;
        double t0 = omp_get_wtime();
        if (mode == 2) {
            run_region_unica(a, b, n, steps);
            if (steps % 2) { u = b; v = a; }
        } else {
            for (size_t s = 0; s < steps; ++s) {
                if (mode == 0) step(u, v, n);
                else if (mode == 1) collapse_step(u, v, n);
                /* mode 3: residuo solo cada RESIDUO_CADA pasos y en el ultimo */
                else if ((s + 1) % RESIDUO_CADA == 0 || s + 1 == steps) step(u, v, n);
                else step_sin_residuo(u, v, n);
                double *x = u; u = v; v = x;
            }
        }
        double dt = omp_get_wtime() - t0;
        if (dt < mejor) mejor = dt;
        *final = u;
    }
    return mejor;
}

int main(int argc, char **argv)
{
    size_t n = argc > 1 ? strtoull(argv[1], NULL, 10) : 2048;
    size_t steps = argc > 2 ? strtoull(argv[2], NULL, 10) : 300;
    int p = argc > 3 ? atoi(argv[3]) : 12;
    int reps = argc > 4 ? atoi(argv[4]) : 4;
    if (n < 3 || steps == 0 || p < 1 || reps < 1) {
        fprintf(stderr, "Uso: %s [N] [pasos] [hilos] [repeticiones]\n", argv[0]);
        return EXIT_FAILURE;
    }
    omp_set_dynamic(0);
    omp_set_num_threads(p);

    double *ref = malloc(bytes(n)), *tmp = malloc(bytes(n));
    double *a = malloc(bytes(n)), *b = malloc(bytes(n));
    if (!ref || !tmp || !a || !b) return EXIT_FAILURE;

    /* Referencia secuencial: define el resultado correcto y T1. */
    double t_seq = 1e30, res_seq = 0.0;
    for (int rep = 0; rep < reps; ++rep) {
        init(ref, tmp, n);
        double *u = ref, *v = tmp;
        double t0 = omp_get_wtime();
        for (size_t s = 0; s < steps; ++s) {
            res_seq = step_seq(u, v, n); /* se imprime: -O2 no puede quitarlo */
            double *x = u; u = v; v = x;
        }
        double dt = omp_get_wtime() - t0;
        if (dt < t_seq) t_seq = dt;
        if (u != ref) memcpy(ref, u, bytes(n));
    }

    /* Modelo de trafico declarado (apartado V.G): por celda interior se lee
     * una vez su valor y se escribe el nuevo -> 16 B. Es una COTA INFERIOR. */
    double moved = (double)(n - 2) * (n - 2) * 2.0 * sizeof(double);

    printf("# N=%zu pasos=%zu hilos=%d repeticiones=%d criterio=minimo "
           "T1_secuencial=%.9f modelo=16B/celda residuo=%.17g\n", n, steps, p, reps, t_seq, res_seq);
    printf("variante,tiempo_s,speedup_abs,mejora_vs_base,max_diff,correcto,"
           "bytes_por_paso,GB_s\n");
    static const char *NOMBRES[] = {"base", "collapse2", "region_unica",
                                    "residuo_cada_10"};
    double base = 0.0;
    for (int mode = 0; mode < 4; ++mode) {
        const double *final = NULL;
        double t = measure(a, b, n, steps, mode, reps, &final);
        if (mode == 0) base = t;
        double d = diff(final, ref, n);
        printf("%s,%.9f,%.4f,%.4f,%.3e,%d,%.0f,%.2f\n", NOMBRES[mode], t,
               t_seq / t, base / t, d, d <= TOLERANCIA, moved,
               moved / (t / steps) / 1e9);
    }
    /* La afinidad no necesita codigo: se mide ejecutando este mismo binario
     * con OMP_PROC_BIND=close OMP_PLACES=cores (ver ejecutar_todo.sh). */
    printf("# OMP_PROC_BIND=%s OMP_PLACES=%s\n",
           getenv("OMP_PROC_BIND") ? getenv("OMP_PROC_BIND") : "(sin fijar)",
           getenv("OMP_PLACES") ? getenv("OMP_PLACES") : "(sin fijar)");
    free(ref); free(tmp); free(a); free(b);
    return EXIT_SUCCESS;
}
