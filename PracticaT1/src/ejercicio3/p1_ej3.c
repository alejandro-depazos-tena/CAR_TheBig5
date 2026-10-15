#include <math.h>
#include <omp.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static size_t bytes(size_t n){return n*n*sizeof(double);}
static void init(double*a,double*b,size_t n){memset(a,0,bytes(n));memset(b,0,bytes(n));for(size_t j=0;j<n;++j)a[j]=b[j]=100.0;}
static void step_seq(const double*a,double*b,size_t n){for(size_t i=1;i+1<n;++i)for(size_t j=1;j+1<n;++j){size_t k=i*n+j;b[k]=.25*(a[k-n]+a[k+n]+a[k-1]+a[k+1]);}}
static double step_omp(const double*a,double*b,size_t n){double r=0.0;
#pragma omp parallel for default(none) shared(a,b,n) reduction(max:r) schedule(static)
for(size_t i=1;i+1<n;++i){double row=0.0;for(size_t j=1;j+1<n;++j){size_t k=i*n+j;double v=.25*(a[k-n]+a[k+n]+a[k-1]+a[k+1]);b[k]=v;if(fabs(v-a[k])>row)row=fabs(v-a[k]);}if(row>r)r=row;}return r;}
static void run_seq(double**a,double**b,size_t n,size_t steps){for(size_t s=0;s<steps;++s){step_seq(*a,*b,n);double*t=*a;*a=*b;*b=t;}}
static void run_omp(double**a,double**b,size_t n,size_t steps){for(size_t s=0;s<steps;++s){step_omp(*a,*b,n);double*t=*a;*a=*b;*b=t;}}

static double arithmetic_benchmark(size_t n, size_t reps, int threads)
{
    double *a = malloc(n * sizeof(*a)); double sum = 0.0;
    for (size_t i = 0; i < n; ++i) a[i] = 1.0;
    omp_set_num_threads(threads);
    double t = omp_get_wtime();
#pragma omp parallel for default(none) shared(a, n, reps) reduction(+:sum)
    for (size_t i = 0; i < n; ++i) {
        double x = a[i];
        for (size_t r = 0; r < reps; ++r) x = x * 1.0000001 + 0.0000001;
        sum += x;
    }
    t = omp_get_wtime() - t; free(a); return (double)n * reps * 2.0 / t;
}

int main(int argc, char **argv)
{
    size_t n = argc > 1 ? strtoull(argv[1], NULL, 10) : 2048;
    size_t steps = argc > 2 ? strtoull(argv[2], NULL, 10) : 300;
    const int counts[] = {1,2,4,8,12,16,20,24};
    printf("threads,N,steps,time_s,speedup,efficiency,arith_ops_s\n");
    double *sa, *sb;
    sa=malloc(bytes(n)); sb=malloc(bytes(n)); if (!sa || !sb) return EXIT_FAILURE;
    init(sa, sb, n);
    double t0 = omp_get_wtime();
    run_seq(&sa, &sb, n, steps);
    double baseline = omp_get_wtime() - t0;
    free(sa); free(sb);
    for (size_t q = 0; q < sizeof(counts)/sizeof(counts[0]); ++q) {
        int p = counts[q]; double *a, *b;
        a=malloc(bytes(n)); b=malloc(bytes(n)); if (!a || !b) return EXIT_FAILURE;
        init(a, b, n); omp_set_dynamic(0); omp_set_num_threads(p);
        double t = omp_get_wtime(); run_omp(&a, &b, n, steps);
        t = omp_get_wtime() - t;
        printf("%d,%zu,%zu,%.9f,%.6f,%.6f,%.6g\n", p, n, steps, t,
               baseline / t, baseline / (t * p),
               arithmetic_benchmark(n * 1024, 1024, p));
        free(a); free(b);
    }
    return EXIT_SUCCESS;
}
