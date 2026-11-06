/* TermoHPC - Ejercicio 4: escalabilidad debil.
 * Compilar: gcc -Wall -Wextra -O2 -std=c11 -fopenmp p1_ej4.c -o p1_ej4 -lm
 * Uso: ./p1_ej4 [N1=1024] [pasos=300] [repeticiones=4]
 * N(p) = round(N1 * sqrt(p)): el trabajo de un paso va con N^2, asi que
 * para conservar las celdas por hilo N se multiplica por sqrt(p), no por p.
 * Criterio: minimo de las repeticiones, hechas en RONDAS INTERCALADAS (cada
 * ronda mide una vez los ocho tamanos), para que una racha lenta de la
 * maquina no caiga sobre todas las repeticiones de un mismo punto.
 * tiempo_peor_s es el maximo de las rondas (dispersion del punto).
 * Eficiencia debil = T1/Tp, con T1 = la version paralela con 1 hilo y N1.
 * Ademas, para cada N se mide la version secuencial con la MISMA N:
 *  - sirve de referencia para VERIFICAR la medida paralela (norma del maximo
 *    de la diferencia celda a celda y checksum), como en el Ejercicio 2;
 *  - su coste por celda y paso dice si el tamano de la malla (hasta ~400 MB
 *    con N = 5017) cambia por si solo el coste de cada celda.
 */
#include <math.h>
#include <omp.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

/* Referencia secuencial: el mismo paso (con residuo) sin pragmas. */
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

static double checksum(const double *a, size_t n)
{
    double s = 0.0;
    for (size_t i = 1; i < n - 1; ++i)
        for (size_t j = 1; j < n - 1; ++j)
            s += a[i * n + j];
    return s;
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

/* Mejor de 'reps'; deja en *final la malla resultante y en *res el residuo
 * del ultimo paso (se imprime: -O2 no puede eliminar su calculo). */
static double mide(double *a, double *b, size_t n, size_t steps, int reps,
                   int paralelo, double **final, double *res)
{
    double mejor = 1e30;
    for (int rep = 0; rep < reps; ++rep) {
        init(a, b, n);                       /* fuera del cronometro */
        double *u = a, *v = b;
        double r = 0.0;
        double t0 = omp_get_wtime();
        for (size_t s = 0; s < steps; ++s) {
            r = paralelo ? step_omp(u, v, n) : step_seq(u, v, n);
            double *tmp = u; u = v; v = tmp;
        }
        double dt = omp_get_wtime() - t0;
        if (dt < mejor) mejor = dt;
        *final = u;
        *res = r;
    }
    return mejor;
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

    size_t ns[NUM_HILOS];
    int reales[NUM_HILOS], ok[NUM_HILOS];
    double t[NUM_HILOS], t_peor[NUM_HILOS], t_seq[NUM_HILOS], cs[NUM_HILOS];
    double dmax[NUM_HILOS], dcs[NUM_HILOS];
    for (size_t q = 0; q < NUM_HILOS; ++q) {
        ns[q] = (size_t)llround((double)n1 * sqrt((double)HILOS[q]));
        ok[q] = 1;
        t[q] = t_seq[q] = 1e30;
        t_peor[q] = dmax[q] = dcs[q] = 0.0;
    }

    for (int ronda = 0; ronda < reps; ++ronda) {
        fprintf(stderr, "[ronda %d/%d]", ronda + 1, reps);
        for (size_t q = 0; q < NUM_HILOS; ++q) {
            int p = HILOS[q];
            size_t n = ns[q];
            double *a = malloc(bytes(n)), *b = malloc(bytes(n));
            double *sa = malloc(bytes(n)), *sb = malloc(bytes(n));
            if (!a || !b || !sa || !sb) {
                fprintf(stderr, "No hay memoria para N=%zu.\n", n);
                return EXIT_FAILURE;
            }

            /* Referencia secuencial con la misma N (tiempo y campo correcto). */
            double *ref = NULL, res_seq = 0.0;
            double ts = mide(sa, sb, n, steps, 1, 0, &ref, &res_seq);
            if (ts < t_seq[q]) t_seq[q] = ts;
            double cs_seq = checksum(ref, n);

            omp_set_num_threads(p);
            reales[q] = hilos_reales();
            double *fin = NULL, res_par = 0.0;
            double tp = mide(a, b, n, steps, 1, 1, &fin, &res_par);
            if (tp < t[q]) t[q] = tp;
            if (tp > t_peor[q]) t_peor[q] = tp;
            cs[q] = checksum(fin, n);

            /* Verificacion en cada ronda: campo celda a celda, checksum y
             * residuo frente a la secuencial con la misma N. */
            double d = difference(fin, ref, n);
            double dc = fabs(cs[q] - cs_seq) / fabs(cs_seq);
            if (d > dmax[q]) dmax[q] = d;
            if (dc > dcs[q]) dcs[q] = dc;
            ok[q] &= reales[q] == p && d <= TOLERANCIA && dc <= TOLERANCIA
                     && res_par == res_seq;
            fprintf(stderr, " N%zu:p%d=%.4f/sec=%.4f", n, p, tp, ts);
            free(a); free(b); free(sa); free(sb);
        }
        fprintf(stderr, "\n");
    }

    int todo_ok = 1;
    printf("# N1=%zu pasos=%zu repeticiones=%d rondas=intercaladas criterio=minimo "
           "tolerancia=%.0e\n", n1, steps, reps, TOLERANCIA);
    printf("hilos,hilos_reales,N,celdas_por_hilo,pasos,tiempo_s,eficiencia_debil,"
           "MB_dos_buffers,checksum,tiempo_seq_mismo_N_s,ns_celda_paso_seq,"
           "speedup_abs_mismo_N,max_diff,dif_checksum_rel,correcto,tiempo_peor_s\n");
    for (size_t q = 0; q < NUM_HILOS; ++q) {
        size_t n = ns[q];
        todo_ok &= ok[q];
        printf("%d,%d,%zu,%.0f,%zu,%.9f,%.4f,%.1f,%.17g,%.9f,%.4f,%.4f,%.3e,%.3e,%d,%.9f\n",
               HILOS[q], reales[q], n, (double)n * n / HILOS[q], steps, t[q],
               t[0] / t[q], 2.0 * bytes(n) / 1e6, cs[q], t_seq[q],
               1e9 * t_seq[q] / ((double)n * (double)n * (double)steps),
               t_seq[q] / t[q], dmax[q], dcs[q], ok[q], t_peor[q]);
    }
    printf("# verificacion de todas las mallas: %s\n", todo_ok ? "PASA" : "FALLA");
    return todo_ok ? EXIT_SUCCESS : EXIT_FAILURE;
}
