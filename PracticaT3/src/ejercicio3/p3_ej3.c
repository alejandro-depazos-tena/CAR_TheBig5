/* p3_ej3.c - Comparacion de critical, atomic y reduction
   con acumuladores double y long long. */

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "crono.h"

#define N   20000000
#define REP 3
#define P   8

static double *x;
static long long *v;

/* ---------- Acumulador double ---------- */

static double d_secuencial(double *r)
{
    double s = 0.0;
    double t0 = reloj();

    for (int i = 0; i < N; i++) {
        s += x[i];
    }

    *r = s;
    return reloj() - t0;
}

static double d_critical(double *r)
{
    double s = 0.0;
    double t0 = reloj();

    #pragma omp parallel for num_threads(P)
    for (int i = 0; i < N; i++) {
        #pragma omp critical
        {
            s += x[i];
        }
    }

    *r = s;
    return reloj() - t0;
}

static double d_atomic(double *r)
{
    double s = 0.0;
    double t0 = reloj();

    #pragma omp parallel for num_threads(P)
    for (int i = 0; i < N; i++) {
        #pragma omp atomic
        s += x[i];
    }

    *r = s;
    return reloj() - t0;
}

static double d_reduction(double *r)
{
    double s = 0.0;
    double t0 = reloj();

    #pragma omp parallel for num_threads(P) reduction(+:s)
    for (int i = 0; i < N; i++) {
        s += x[i];
    }

    *r = s;
    return reloj() - t0;
}

/* ---------- Acumulador long long ---------- */

static double l_secuencial(double *r)
{
    long long s = 0;
    double t0 = reloj();

    for (int i = 0; i < N; i++) {
        s += v[i];
    }

    *r = (double)s;
    return reloj() - t0;
}

static double l_critical(double *r)
{
    long long s = 0;
    double t0 = reloj();

    #pragma omp parallel for num_threads(P)
    for (int i = 0; i < N; i++) {
        #pragma omp critical
        {
            s += v[i];
        }
    }

    *r = (double)s;
    return reloj() - t0;
}

static double l_atomic(double *r)
{
    long long s = 0;
    double t0 = reloj();

    #pragma omp parallel for num_threads(P)
    for (int i = 0; i < N; i++) {
        #pragma omp atomic
        s += v[i];
    }

    *r = (double)s;
    return reloj() - t0;
}

static double l_reduction(double *r)
{
    long long s = 0;
    double t0 = reloj();

    #pragma omp parallel for num_threads(P) reduction(+:s)
    for (int i = 0; i < N; i++) {
        s += v[i];
    }

    *r = (double)s;
    return reloj() - t0;
}

/* Ejecuta una funcion REP veces y devuelve el menor tiempo.
   En *r queda el resultado de la ultima ejecucion. */
static double mejor(double (*f)(double *), double *r)
{
    double m = 1e30;

    for (int k = 0; k < REP; k++) {
        double t = f(r);

        if (t < m) {
            m = t;
        }
    }

    return m;
}

static int bien = 1;

/* Mide las cuatro versiones y comprueba sus resultados
   frente al acumulado secuencial del mismo tipo. */
static void tanda(
    const char *tipo,
    double (*sec)(double *),
    double (*cri)(double *),
    double (*ato)(double *),
    double (*red)(double *)
)
{
    double rs, rc, ra, rr;

    double ts = mejor(sec, &rs);
    double tc = mejor(cri, &rc);
    double ta = mejor(ato, &ra);
    double tr = mejor(red, &rr);

    printf("acumulador de tipo %s\n", tipo);
    printf(" secuencial %8.4f s\n", ts);
    printf(" critical   %8.4f s\n", tc);

    printf(
        " atomic     %8.4f s %6.2f veces mas rapido que critical\n",
        ta, tc / ta
    );

    printf(
        " reduction  %8.4f s %6.0f veces mas rapido que critical\n",
        tr, tc / tr
    );

    if (rc != rs || ra != rs || rr != rs) {
        bien = 0;
    }
}

int main(void)
{
    x = malloc(N * sizeof *x);
    v = malloc(N * sizeof *v);

    if (!x || !v) {
        fprintf(stderr, "sin memoria\n");
        free(x);
        free(v);
        return 1;
    }

    /* Ambos vectores contienen los mismos valores enteros. */
    for (int i = 0; i < N; i++) {
        v[i] = i % 7 + 1;
        x[i] = (double)v[i];
    }

    printf("%d hilos, %d sumas, mejor de %d\n", P, N, REP);

    tanda(
        "double",
        d_secuencial, d_critical, d_atomic, d_reduction
    );

    tanda(
        "long long",
        l_secuencial, l_critical, l_atomic, l_reduction
    );

    printf(
        "los ocho acumulados dan %s\n",
        bien ? "lo mismo" : "DISTINTO"
    );

    free(x);
    free(v);

    return 0;
}