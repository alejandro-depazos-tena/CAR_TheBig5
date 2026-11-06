/* TermoHPC - Ejercicio 6: optimizacion guiada por medicion.
 * Compilar: gcc -Wall -Wextra -O2 -std=c11 -fopenmp p1_ej6.c -o p1_ej6 -lm
 * Uso: ./p1_ej6 [N=2048] [pasos=300] [hilos=12] [repeticiones=4]
 *
 * La salida tiene tres partes:
 *  1. Referencia secuencial: define el campo correcto y T1.
 *  2. EVIDENCIAS medidas con la version base, que motivan las hipotesis:
 *       E1  coste de un paso vacio (abrir el equipo, repartir y barrera)
 *           frente al tiempo de un paso real;
 *       E2  desequilibrio: tiempo de trabajo de cada hilo dentro de un paso
 *           (con static y con dynamic,4) frente al reloj del paso.
 *     E3 (informe de vectorizacion de gcc) y E4 (techo de ancho de banda,
 *     STREAM) se obtienen fuera de este programa: ver
 *     scripts/ejecutar_ej2_en_adelante.sh.
 *  3. Variantes. Cada una cambia UNA sola cosa respecto de la variante de la
 *     que parte (columna parte_de) y se verifica celda a celda contra la
 *     secuencial antes de dar su tiempo:
 *       base          Ejercicio 2: parallel for por paso, schedule(static)
 *       it1_region    [iteracion 1] una sola region paralela para los pasos
 *       it2_dynamic   [iteracion 2] base con schedule(dynamic, 4)
 *       it3_simd      [iteracion 3] it2 con el bucle interior vectorizado
 *                     (#pragma omp simd)
 *       it4_control   [control]     it3 escrito a mano con SSE2 y escritura
 *                     normal: debe costar lo mismo que it3
 *       it4_nt        [iteracion 4] it4_control con escritura NO temporal
 *       alt_collapse2 [otra hipotesis] base con collapse(2)
 *       alt_residuo10 [otra hipotesis] base calculando el residuo cada 10 pasos
 *     La afinidad (OMP_PROC_BIND / OMP_PLACES) no necesita codigo: se mide
 *     ejecutando este mismo binario con esas variables de entorno.
 *  Las repeticiones van en RONDAS INTERCALADAS: cada ronda mide una vez la
 *  secuencial y las ocho variantes, para que una racha lenta de la maquina no
 *  caiga sobre todas las repeticiones de una misma variante. Criterio: minimo
 *  de las rondas; tiempo_peor_s es el maximo (dispersion de la variante). La
 *  verificacion se hace en todas las rondas.
 *
 * Por que SSE2 a mano en it4: la clausula portable de OpenMP 5.0
 * '#pragma omp simd nontemporal(b)' la acepta gcc 13 pero no genera ninguna
 * instruccion movnt; la escritura no temporal solo se consigue con el
 * intrinseco _mm_stream_pd. it4_control usa exactamente el mismo codigo con
 * _mm_store_pd, para que it4_nt cambie una sola cosa: la instruccion de
 * escritura (mas la barrera sfence que exige).
 *
 * Modelos de trafico declarados (apartado V.G), por celda interior y paso:
 *   16 B: leer u una vez + escribir u_nueva (cota inferior del enunciado);
 *   24 B: ademas, la lectura previa de la linea que se escribe (RFO), que
 *         hace el procesador con una escritura normal; la escritura no
 *         temporal la evita, asi que para it4_nt el trafico real es 16 B.
 */
#include <math.h>
#include <omp.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(__SSE2__)
#include <emmintrin.h>
#define HAY_SSE2 1
#else
#define HAY_SSE2 0
#endif

#define TOLERANCIA 1e-9
#define RESIDUO_CADA 10
#define TROZO_DYNAMIC 4

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

/* ---------------------------------------------------------------- base */
/* La version del Ejercicio 2, sin cambios. */
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

/* ------------------------------------------------- iteracion 1 (region) */
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

/* ------------------------------------------------ iteracion 2 (dynamic) */
/* Cambio unico respecto de base: schedule(dynamic, 4). Los hilos que van mas
 * deprisa piden mas filas; las filas de cada trozo siguen siendo contiguas. */
static double step_dynamic(const double *a, double *b, size_t n)
{
    double r = 0.0;
#pragma omp parallel for default(none) shared(a, b, n) reduction(max : r) schedule(dynamic, TROZO_DYNAMIC)
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

/* --------------------------------------------------- iteracion 3 (simd) */
/* Cambio unico respecto de it2: #pragma omp simd en el bucle interior. Cada
 * celda hace las mismas operaciones en el mismo orden (el campo sale igual
 * bit a bit) y el maximo no depende del orden de combinacion. El maximo se
 * escribe con ?: para que el vectorizador lo reconozca como reduccion. */
static double step_simd(const double *a, double *b, size_t n)
{
    double r = 0.0;
#pragma omp parallel for default(none) shared(a, b, n) reduction(max : r) schedule(dynamic, TROZO_DYNAMIC)
    for (size_t i = 1; i < n - 1; ++i) {
#pragma omp simd reduction(max : r)
        for (size_t j = 1; j < n - 1; ++j) {
            size_t k = i * n + j;
            double v = 0.25 * (a[k - n] + a[k + n] + a[k - 1] + a[k + 1]);
            b[k] = v;
            double d = fabs(v - a[k]);
            r = d > r ? d : r;
        }
    }
    return r;
}

/* ------------------------------- iteracion 4 (escritura no temporal) */
/* Una fila interior con SSE2: dos celdas por instruccion. 'nt' elige la
 * instruccion de escritura: _mm_store_pd (normal: la linea se lee antes de
 * escribirla, RFO) o _mm_stream_pd (no temporal: se escribe directamente en
 * memoria, sin leerla ni ocupar la cache). Las dos exigen direccion alineada
 * a 16 B, de ahi el pelado escalar inicial. La suma se hace en el mismo orden
 * que la secuencial ((arriba + abajo) + izquierda) + derecha. */
static inline double fila_sse2(const double *a, double *b, size_t i, size_t n,
                               int nt)
{
    const double *arr = a + (i - 1) * n, *cen = a + i * n, *aba = a + (i + 1) * n;
    double *out = b + i * n;
    double r = 0.0;
    size_t j = 1;
#if HAY_SSE2
    for (; j < n - 1 && ((uintptr_t)(out + j) & 15u) != 0; ++j) {
        double v = 0.25 * (arr[j] + aba[j] + cen[j - 1] + cen[j + 1]);
        out[j] = v;
        double d = fabs(v - cen[j]);
        if (d > r) r = d;
    }
    const __m128d cuarto = _mm_set1_pd(0.25);
    const __m128d signo = _mm_set1_pd(-0.0);
    __m128d rv = _mm_setzero_pd();
    for (; j + 1 < n - 1; j += 2) {
        __m128d s = _mm_add_pd(_mm_loadu_pd(arr + j), _mm_loadu_pd(aba + j));
        s = _mm_add_pd(s, _mm_loadu_pd(cen + j - 1));
        s = _mm_add_pd(s, _mm_loadu_pd(cen + j + 1));
        __m128d v = _mm_mul_pd(cuarto, s);
        if (nt)
            _mm_stream_pd(out + j, v);
        else
            _mm_store_pd(out + j, v);
        __m128d d = _mm_andnot_pd(signo, _mm_sub_pd(v, _mm_loadu_pd(cen + j)));
        rv = _mm_max_pd(rv, d); /* |v - u| y maximo, por pares */
    }
    double par[2];
    _mm_storeu_pd(par, rv);
    if (par[0] > r) r = par[0];
    if (par[1] > r) r = par[1];
#else
    (void)nt;
#endif
    for (; j < n - 1; ++j) { /* resto escalar (o todo, sin SSE2) */
        double v = 0.25 * (arr[j] + aba[j] + cen[j - 1] + cen[j + 1]);
        out[j] = v;
        double d = fabs(v - cen[j]);
        if (d > r) r = d;
    }
    return r;
}

/* Paso de it4_control (nt = 0) e it4_nt (nt = 1): mismo reparto que it3. Las
 * escrituras no temporales no siguen el orden normal de la memoria: cada hilo
 * hace sfence antes de la barrera del final de la region, para que el paso
 * siguiente lea los valores nuevos. */
static double step_sse2(const double *a, double *b, size_t n, int nt)
{
    double r = 0.0;
#pragma omp parallel default(none) shared(a, b, n, nt) reduction(max : r)
    {
#pragma omp for schedule(dynamic, TROZO_DYNAMIC) nowait
        for (size_t i = 1; i < n - 1; ++i) {
            double rf = fila_sse2(a, b, i, n, nt);
            if (rf > r) r = rf;
        }
#if HAY_SSE2
        if (nt) _mm_sfence();
#endif
    } /* barrera implicita: separa este paso del siguiente */
    return r;
}

/* ------------------------------------------------- otras hipotesis */
/* Cambio unico respecto de base: collapse(2) reparte las (N-2)^2 celdas en
 * vez de las filas. */
static double step_collapse(const double *a, double *b, size_t n)
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

/* Cambio unico respecto de base: el paso sin residuo (sin fabs, sin
 * comparacion y sin reduccion). Se usa en los pasos en que no se necesita el
 * residuo; cada RESIDUO_CADA pasos y en el ultimo se usa el paso base. */
static void step_sin_residuo(const double *a, double *b, size_t n)
{
#pragma omp parallel for default(none) shared(a, b, n) schedule(static)
    for (size_t i = 1; i < n - 1; ++i)
        for (size_t j = 1; j < n - 1; ++j) {
            size_t k = i * n + j;
            b[k] = 0.25 * (a[k - n] + a[k + n] + a[k - 1] + a[k + 1]);
        }
}

/* ------------------------------------------------------- evidencias */
/* E1: coste fijo de un paso vacio con el mismo reparto que la base (abrir el
 * equipo, repartir N-2 filas sin trabajo y la barrera final). */
static double mide_paso_vacio(size_t n, int reps)
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

/* E2: el paso base instrumentado. Cada hilo apunta cuanto tarda en hacer SU
 * parte (desde que entra en la region hasta que acaba sus filas, antes de la
 * barrera). Se usa schedule(runtime) para medir static y dynamic con el
 * mismo codigo. Imprime, por paso: reloj, trabajo minimo/medio/maximo de los
 * hilos y la fraccion del reloj que, en media, un hilo NO esta trabajando
 * (entrada tardia en la region + espera en la barrera). */
static void mide_desequilibrio(double *a, double *b, size_t n, size_t steps,
                               int p, omp_sched_t kind, int chunk,
                               const char *nombre, double *sumidero)
{
    double *trabajo = calloc((size_t)p, sizeof(double));
    if (!trabajo) return;
    omp_set_schedule(kind, chunk);
    init(a, b, n);
    double *u = a, *v = b;
    double pared = 0.0;
    for (size_t s = 0; s < steps; ++s) {
        double r = 0.0;
        double t0 = omp_get_wtime();
#pragma omp parallel default(none) shared(u, v, n, trabajo) reduction(max : r)
        {
            double ti = omp_get_wtime();
#pragma omp for schedule(runtime) nowait
            for (size_t i = 1; i < n - 1; ++i)
                for (size_t j = 1; j < n - 1; ++j) {
                    size_t k = i * n + j;
                    double val = 0.25 * (u[k - n] + u[k + n] + u[k - 1] + u[k + 1]);
                    v[k] = val;
                    double d = fabs(val - u[k]);
                    if (d > r) r = d;
                }
            trabajo[omp_get_thread_num()] += omp_get_wtime() - ti;
        }
        pared += omp_get_wtime() - t0;
        *sumidero += r;
        double *x = u; u = v; v = x;
    }
    double mn = 1e30, mx = 0.0, media = 0.0;
    for (int q = 0; q < p; ++q) {
        if (trabajo[q] < mn) mn = trabajo[q];
        if (trabajo[q] > mx) mx = trabajo[q];
        media += trabajo[q] / p;
    }
    printf("# E2 desequilibrio %s: reloj_por_paso_s=%.4e trabajo_por_paso_s "
           "min=%.4e media=%.4e max=%.4e max/min=%.2f fraccion_sin_trabajar=%.1f%%\n",
           nombre, pared / steps, mn / steps, media / steps, mx / steps, mx / mn,
           100.0 * (1.0 - media / pared));
    printf("# E2 trabajo de cada hilo %s (ms/paso):", nombre);
    for (int q = 0; q < p; ++q) printf(" %.3f", 1e3 * trabajo[q] / steps);
    printf("\n");
    free(trabajo);
}

/* ------------------------------------------------------- medida */
enum { BASE, IT1, IT2, IT3, IT4C, IT4, ALT_COLLAPSE, ALT_RESIDUO, NUM_VARIANTES };

static const struct {
    const char *nombre;
    int parte_de;       /* indice de la variante de la que parte (-1: ninguna) */
    int bytes_celda;    /* trafico real declarado por celda y paso */
    const char *cambio;
} VARIANTES[NUM_VARIANTES] = {
    {"base", -1, 24, "Ejercicio 2: parallel for por paso con schedule(static)"},
    {"it1_region", BASE, 24, "una sola region paralela para todos los pasos"},
    {"it2_dynamic", BASE, 24, "schedule(static) -> schedule(dynamic 4)"},
    {"it3_simd", IT2, 24, "#pragma omp simd en el bucle interior"},
    {"it4_control", IT3, 24, "control: it3 escrito con SSE2 a mano (escritura normal)"},
    {"it4_nt", IT4C, 16, "escritura normal -> escritura no temporal (_mm_stream_pd)"},
    {"alt_collapse2", BASE, 24, "collapse(2)"},
    {"alt_residuo10", BASE, 24, "residuo solo cada 10 pasos y en el ultimo"},
};

/* Mejor de 'reps'; deja en *final la malla resultante para verificarla y en
 * *res el residuo del ultimo paso. */
static double measure(double *a, double *b, size_t n, size_t steps, int mode,
                      int reps, const double **final, double *res)
{
    double mejor = 1e30;
    for (int rep = 0; rep < reps; ++rep) {
        init(a, b, n); /* fuera del cronometro */
        double *u = a, *v = b;
        double r = 0.0;
        double t0 = omp_get_wtime();
        if (mode == IT1) {
            r = run_region_unica(a, b, n, steps);
            if (steps % 2) { u = b; v = a; }
        } else {
            for (size_t s = 0; s < steps; ++s) {
                switch (mode) {
                case BASE: r = step(u, v, n); break;
                case IT2: r = step_dynamic(u, v, n); break;
                case IT3: r = step_simd(u, v, n); break;
                case IT4C: r = step_sse2(u, v, n, 0); break;
                case IT4: r = step_sse2(u, v, n, 1); break;
                case ALT_COLLAPSE: r = step_collapse(u, v, n); break;
                default: /* ALT_RESIDUO */
                    if ((s + 1) % RESIDUO_CADA == 0 || s + 1 == steps)
                        r = step(u, v, n);
                    else
                        step_sin_residuo(u, v, n);
                }
                double *x = u; u = v; v = x;
            }
        }
        double dt = omp_get_wtime() - t0;
        if (dt < mejor) mejor = dt;
        *final = u;
        *res = r;
    }
    return mejor;
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
    int reales = hilos_reales();

    double *ref = malloc(bytes(n)), *tmp = malloc(bytes(n));
    double *a = malloc(bytes(n)), *b = malloc(bytes(n));
    if (!ref || !tmp || !a || !b) {
        fprintf(stderr, "No se pudo reservar la malla.\n");
        return EXIT_FAILURE;
    }

    /* 1. Referencia secuencial: define el resultado correcto (en ref). */
    double res_seq = 0.0;
    double t_seq = mide_seq(ref, tmp, n, steps, 1, &res_seq);
    double t_seq_peor = t_seq;

    printf("# N=%zu pasos=%zu hilos=%d hilos_reales=%d repeticiones=%d "
           "rondas=intercaladas criterio=minimo SSE2=%s\n", n, steps, p, reales,
           reps, HAY_SSE2 ? "si" : "no");
    printf("# OMP_PROC_BIND=%s OMP_PLACES=%s\n",
           getenv("OMP_PROC_BIND") ? getenv("OMP_PROC_BIND") : "(sin fijar)",
           getenv("OMP_PLACES") ? getenv("OMP_PLACES") : "(sin fijar)");

    /* 2. Evidencias, medidas con la version base. */
    double sumidero = 0.0;
    double vacio = mide_paso_vacio(n, reps);
    mide_desequilibrio(a, b, n, steps, p, omp_sched_static, 0, "static", &sumidero);
    mide_desequilibrio(a, b, n, steps, p, omp_sched_dynamic, TROZO_DYNAMIC,
                       "dynamic4", &sumidero);

    /* 3. Variantes, en rondas intercaladas con la secuencial. */
    double t[NUM_VARIANTES], t_peor[NUM_VARIANTES], dif[NUM_VARIANTES];
    int res_ok[NUM_VARIANTES];
    for (int m = 0; m < NUM_VARIANTES; ++m) {
        t[m] = 1e30;
        t_peor[m] = dif[m] = 0.0;
        res_ok[m] = 1;
    }
    int seq_igual = 1;
    for (int ronda = 0; ronda < reps; ++ronda) {
        if (ronda > 0) { /* la ronda 0 usa la medida que dejo ref */
            double res = 0.0;
            double ts = mide_seq(tmp, a, n, steps, 1, &res);
            seq_igual &= res == res_seq;
            if (ts < t_seq) t_seq = ts;
            if (ts > t_seq_peor) t_seq_peor = ts;
        }
        fprintf(stderr, "[ronda %d/%d]", ronda + 1, reps);
        for (int m = 0; m < NUM_VARIANTES; ++m) {
            const double *final = NULL;
            double r = 0.0;
            double tm = measure(a, b, n, steps, m, 1, &final, &r);
            if (tm < t[m]) t[m] = tm;
            if (tm > t_peor[m]) t_peor[m] = tm;
            double d = diff(final, ref, n);
            if (d > dif[m]) dif[m] = d;
            res_ok[m] &= r == res_seq;
            fprintf(stderr, " %s=%.6f", VARIANTES[m].nombre, tm);
        }
        fprintf(stderr, "\n");
    }

    double celdas = (double)(n - 2) * (double)(n - 2);
    printf("# T1_secuencial=%.9f T1_peor=%.9f residuo=%.17g\n", t_seq, t_seq_peor,
           res_seq);
    printf("# E1 paso_vacio_s=%.4e s_por_paso_base=%.4e fraccion_del_paso=%.2f%%\n",
           vacio, t[BASE] / steps, 100.0 * vacio / (t[BASE] / steps));
    printf("variante,parte_de,cambio,tiempo_s,speedup_abs,mejora_vs_anterior,"
           "mejora_vs_base,max_diff,correcto,s_por_paso,GB_s_16B,GB_s_24B,"
           "B_celda_real,GB_s_real,tiempo_peor_s\n");
    int todo_ok = reales == p && seq_igual;
    for (int m = 0; m < NUM_VARIANTES; ++m) {
        int padre = VARIANTES[m].parte_de;
        double paso = t[m] / steps;
        int ok = dif[m] <= TOLERANCIA && res_ok[m];
        todo_ok &= ok;
        printf("%s,%s,%s,%.9f,%.4f,%.4f,%.4f,%.3e,%d,%.6e,%.2f,%.2f,%d,%.2f,%.9f\n",
               VARIANTES[m].nombre, padre < 0 ? "-" : VARIANTES[padre].nombre,
               VARIANTES[m].cambio, t[m], t_seq / t[m],
               padre < 0 ? 1.0 : t[padre] / t[m], t[BASE] / t[m], dif[m], ok,
               paso, 16.0 * celdas / paso / 1e9, 24.0 * celdas / paso / 1e9,
               VARIANTES[m].bytes_celda,
               VARIANTES[m].bytes_celda * celdas / paso / 1e9, t_peor[m]);
    }
    printf("# verificacion de todas las variantes: %s\n", todo_ok ? "PASA" : "FALLA");
    fprintf(stderr, "(sumidero de E2: %g)\n", sumidero);
    free(ref); free(tmp); free(a); free(b);
    return todo_ok ? EXIT_SUCCESS : EXIT_FAILURE;
}
