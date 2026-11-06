/* TermoHPC - Ejercicio 5: comparacion de planificaciones.
 * Compilar: gcc -Wall -Wextra -O2 -std=c11 -fopenmp p1_ej5.c -o p1_ej5 -lm
 * Uso: ./p1_ej5 [N=2048] [pasos=300] [hilos=12] [repeticiones=4]
 * El bucle usa schedule(runtime) y la planificacion se fija con
 * omp_set_schedule(), asi el codigo medido es identico en todos los casos.
 * Cada caso se verifica contra la version SECUENCIAL (en todas las rondas).
 * Las repeticiones van en RONDAS INTERCALADAS: cada ronda mide una vez la
 * secuencial (T1) y las diez planificaciones, para que una racha lenta de la
 * maquina no caiga sobre todas las repeticiones de un mismo caso. Criterio:
 * minimo de las rondas; tiempo_peor_s es el maximo (dispersion del caso).
 */
#include <math.h>
#include <omp.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOLERANCIA 1e-9

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

/* Igual que el Ejercicio 2 salvo schedule(runtime). */
static double step_omp(const double *a, double *b, size_t n)
{
    double r = 0.0;
#pragma omp parallel for default(none) shared(a, b, n) reduction(max : r) schedule(runtime)
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

static omp_sched_t sched(const char *s)
{
    if (strcmp(s, "dynamic") == 0) return omp_sched_dynamic;
    if (strcmp(s, "guided") == 0) return omp_sched_guided;
    return omp_sched_static;
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

/* Referencia secuencial: mejor de 'reps'; deja el campo final en ref. */
static double mide_seq(double *ref, double *tmp, size_t n, size_t steps,
                       int reps, double *res_seq)
{
    double t_seq = 1e30;
    for (int rep = 0; rep < reps; ++rep) {
        init(ref, tmp, n);
        double *u = ref, *v = tmp;
        double t0 = omp_get_wtime();
        for (size_t s = 0; s < steps; ++s) {
            *res_seq = step_seq(u, v, n); /* se imprime: -O2 no puede quitarlo */
            double *x = u; u = v; v = x;
        }
        double dt = omp_get_wtime() - t0;
        if (dt < t_seq) t_seq = dt;
        if (u != ref) memcpy(ref, u, bytes(n)); /* fuera del cronometro */
    }
    return t_seq;
}

/* chunk = 0 significa "sin trozo" (reparto por defecto de la planificacion).
 * Devuelve el mejor tiempo y deja en *d la diferencia maxima con la
 * referencia secuencial. */
static double run_case(const char *kind, int chunk, size_t n, size_t steps,
                       int reps, const double *reference, double *a, double *b,
                       double *d)
{
    omp_set_schedule(sched(kind), chunk);
    double mejor = 1e30;
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
        *d = diff(u, reference, n);
    }
    return mejor;
}

int main(int argc, char **argv)
{
    size_t n = argc > 1 ? strtoull(argv[1], NULL, 10) : 2048;
    size_t steps = argc > 2 ? strtoull(argv[2], NULL, 10) : 300;
    int hilos = argc > 3 ? atoi(argv[3]) : 12;
    int reps = argc > 4 ? atoi(argv[4]) : 4;
    if (n < 3 || steps == 0 || hilos < 1 || reps < 1) {
        fprintf(stderr, "Uso: %s [N] [pasos] [hilos] [repeticiones]\n", argv[0]);
        return EXIT_FAILURE;
    }
    omp_set_dynamic(0);
    omp_set_num_threads(hilos);

    double *ref = malloc(bytes(n)), *tmp = malloc(bytes(n));
    double *a = malloc(bytes(n)), *b = malloc(bytes(n));
    if (!ref || !tmp || !a || !b) return EXIT_FAILURE;

    int reales = hilos_reales();

    static const struct { const char *kind; int chunk; } CASOS[] = {
        {"static", 0}, {"static", 1}, {"static", 4}, {"static", 16},
        {"static", 64}, {"dynamic", 1}, {"dynamic", 4}, {"dynamic", 16},
        {"dynamic", 64}, {"guided", 0},
    };
    enum { NUM_CASOS = sizeof(CASOS) / sizeof(CASOS[0]) };
    double t[NUM_CASOS], t_peor[NUM_CASOS], d[NUM_CASOS];
    for (size_t c = 0; c < NUM_CASOS; ++c) {
        t[c] = 1e30;
        t_peor[c] = d[c] = 0.0;
    }

    double t_seq = 1e30, t_seq_peor = 0.0, res_seq = 0.0;
    int seq_igual = 1;
    for (int ronda = 0; ronda < reps; ++ronda) {
        /* Referencia secuencial (y su tiempo, para el speedup absoluto). El
         * campo correcto se guarda en ref en la primera ronda; despues solo se
         * cronometra (en tmp/a) y se comprueba que da el mismo residuo. */
        double res = 0.0;
        double ts = ronda == 0 ? mide_seq(ref, tmp, n, steps, 1, &res)
                               : mide_seq(tmp, a, n, steps, 1, &res);
        if (ronda == 0) res_seq = res;
        seq_igual &= res == res_seq;
        if (ts < t_seq) t_seq = ts;
        if (ts > t_seq_peor) t_seq_peor = ts;
        fprintf(stderr, "[ronda %d/%d] T1=%.6f", ronda + 1, reps, ts);

        for (size_t c = 0; c < NUM_CASOS; ++c) {
            double dc = 0.0;
            double tc = run_case(CASOS[c].kind, CASOS[c].chunk, n, steps, 1, ref,
                                 a, b, &dc);
            if (tc < t[c]) t[c] = tc;
            if (tc > t_peor[c]) t_peor[c] = tc;
            if (dc > d[c]) d[c] = dc;
            fprintf(stderr, " %s,%d=%.6f", CASOS[c].kind, CASOS[c].chunk, tc);
        }
        fprintf(stderr, "\n");
    }

    printf("# N=%zu pasos=%zu hilos=%d hilos_reales=%d repeticiones=%d "
           "rondas=intercaladas criterio=minimo T1_secuencial=%.9f T1_peor=%.9f "
           "residuo=%.17g\n", n, steps, hilos, reales, reps, t_seq, t_seq_peor,
           res_seq);
    printf("schedule,chunk,tiempo_s,speedup_abs,max_diff,correcto,tiempo_peor_s\n");
    int todo_ok = reales == hilos && seq_igual;
    for (size_t c = 0; c < NUM_CASOS; ++c) {
        printf("%s,%d,%.9f,%.4f,%.3e,%d,%.9f\n", CASOS[c].kind, CASOS[c].chunk, t[c],
               t_seq / t[c], d[c], d[c] <= TOLERANCIA, t_peor[c]);
        todo_ok &= d[c] <= TOLERANCIA;
    }
    printf("# verificacion de todas las planificaciones: %s\n",
           todo_ok ? "PASA" : "FALLA");

    free(ref); free(tmp); free(a); free(b);
    return todo_ok ? EXIT_SUCCESS : EXIT_FAILURE;
}
