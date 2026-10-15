#include <math.h>
#include <omp.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { size_t steps; double residual; } ThermoResult;
static size_t bytes(size_t n) { return n * n * sizeof(double); }
static void init(double *a, double *b, size_t n) {
    memset(a, 0, bytes(n)); memset(b, 0, bytes(n));
    for (size_t j = 0; j < n; ++j) a[j] = b[j] = 100.0;
}
static void step_seq(const double *a, double *b, size_t n) {
    for (size_t i = 1; i + 1 < n; ++i)
        for (size_t j = 1; j + 1 < n; ++j) {
            size_t k = i*n+j;
            b[k] = 0.25*(a[k-n]+a[k+n]+a[k-1]+a[k+1]);
        }
}
static double step_omp(const double *a, double *b, size_t n) {
    double r = 0.0;
#pragma omp parallel for default(none) shared(a,b,n) reduction(max:r) schedule(static)
    for (size_t i = 1; i + 1 < n; ++i) {
        double row = 0.0;
        for (size_t j = 1; j + 1 < n; ++j) {
            size_t k = i*n+j; double value = 0.25*(a[k-n]+a[k+n]+a[k-1]+a[k+1]);
            b[k] = value; if (fabs(value-a[k]) > row) row = fabs(value-a[k]);
        }
        if (row > r) r = row;
    }
    return r;
}
static ThermoResult run_seq(double **a, double **b, size_t n, size_t steps) {
    ThermoResult result = {0, 0.0};
    for (size_t s=0; s<steps; ++s) { step_seq(*a,*b,n);
        result.residual=0.0;
        for (size_t i=1;i+1<n;++i) for (size_t j=1;j+1<n;++j) {
            size_t k=i*n+j; double d=fabs((*b)[k]-(*a)[k]);
            if (d>result.residual) result.residual=d;
        }
        double *tmp=*a; *a=*b; *b=tmp; ++result.steps;
    } return result;
}
static ThermoResult run_omp(double **a, double **b, size_t n, size_t steps) {
    ThermoResult result={0,0.0};
    for (size_t s=0;s<steps;++s) { result.residual=step_omp(*a,*b,n);
        double *tmp=*a; *a=*b; *b=tmp; ++result.steps; } return result;
}
static int checks(const double *a, size_t n) {
    for (size_t j=0;j<n;++j) if (a[j]!=100.0 || a[(n-1)*n+j]!=0.0) return 0;
    for (size_t i=1;i+1<n;++i) if (a[i*n]!=0.0 || a[i*n+n-1]!=0.0) return 0;
    for (size_t i=1;i+1<n;++i) for (size_t j=1;j+1<n;++j)
        if (!isfinite(a[i*n+j]) || a[i*n+j]<0.0 || a[i*n+j]>100.0) return 0;
    return 1;
}
static double difference(const double *a, const double *b, size_t n) {
    double dmax=0.0; for (size_t k=0;k<n*n;++k) {
        double d=fabs(a[k]-b[k]); if (d>dmax) dmax=d;
    } return dmax;
}
static double checksum(const double *a, size_t n) {
    double s=0.0; for (size_t i=1;i+1<n;++i) for (size_t j=1;j+1<n;++j) s+=a[i*n+j];
    return s;
}

static void in_situ(double *u, size_t n, size_t steps)
{
    for (size_t s = 0; s < steps; ++s) {
#pragma omp parallel for default(none) shared(u, n) schedule(static)
        for (size_t i = 1; i + 1 < n; ++i)
            for (size_t j = 1; j + 1 < n; ++j) {
                size_t k = i * n + j;
                u[k] = 0.25 * (u[k - n] + u[k + n] + u[k - 1] + u[k + 1]);
            }
    }
}

int main(int argc, char **argv)
{
    size_t n = argc > 1 ? strtoull(argv[1], NULL, 10) : 1024;
    size_t steps = argc > 2 ? strtoull(argv[2], NULL, 10) : 300;
    int threads = argc > 3 ? atoi(argv[3]) : omp_get_max_threads();
    double *seq_a, *seq_b, *par_a, *par_b, *bad;
    if (n < 3 || !(seq_a=malloc(bytes(n))) || !(seq_b=malloc(bytes(n))) ||
        !(par_a=malloc(bytes(n))) || !(par_b=malloc(bytes(n))) || !(bad=malloc(bytes(n)))) {
        fprintf(stderr, "No se pudo reservar la malla.\n"); return EXIT_FAILURE;
    }
    omp_set_dynamic(0); omp_set_num_threads(threads);
    omp_set_schedule(omp_sched_static, 0);
    init(seq_a, seq_b, n); init(par_a, par_b, n);
    double t0 = omp_get_wtime();
    run_seq(&seq_a, &seq_b, n, steps);
    double seq_time = omp_get_wtime() - t0;
    t0 = omp_get_wtime();
    run_omp(&par_a, &par_b, n, steps);
    double par_time = omp_get_wtime() - t0;
    double diff = difference(seq_a, par_a, n);
    printf("correcto,N,steps,threads,seq_s,omp_s,max_diff,checksum_seq,checksum_omp\n");
    printf("%d,%zu,%zu,%d,%.9f,%.9f,%.17g,%.17g,%.17g\n",
           diff <= 1e-12 && checks(par_a, n), n, steps,
           threads, seq_time, par_time, diff, checksum(seq_a, n), checksum(par_a, n));
    memcpy(bad, par_a, bytes(n)); init(bad, par_b, n);
    in_situ(bad, n, steps);
    printf("in_situ,%d,%.17g\n", difference(seq_a, bad, n) <= 1e-12, difference(seq_a, bad, n));
    free(seq_a); free(seq_b); free(par_a); free(par_b); free(bad);
    return EXIT_SUCCESS;
}
