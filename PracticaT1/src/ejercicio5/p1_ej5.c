#include <math.h>
#include <omp.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static size_t bytes(size_t n){return n*n*sizeof(double);}
static void init(double*a,double*b,size_t n){memset(a,0,bytes(n));memset(b,0,bytes(n));for(size_t j=0;j<n;++j)a[j]=b[j]=100.0;}
static double diff(const double*a,const double*b,size_t n){double m=0.0;for(size_t k=0;k<n*n;++k){double d=fabs(a[k]-b[k]);if(d>m)m=d;}return m;}
static omp_sched_t sched(const char*s){if(strcmp(s,"dynamic")==0)return omp_sched_dynamic;if(strcmp(s,"guided")==0)return omp_sched_guided;return omp_sched_static;}
static void run(double**a,double**b,size_t n,size_t steps){for(size_t s=0;s<steps;++s){
#pragma omp parallel for default(none) shared(a,b,n) schedule(runtime)
for(size_t i=1;i+1<n;++i)for(size_t j=1;j+1<n;++j){size_t k=i*n+j;(*b)[k]=.25*((*a)[k-n]+(*a)[k+n]+(*a)[k-1]+(*a)[k+1]);}
double*t=*a;*a=*b;*b=t;}}

static void run_case(const char *kind, int chunk, size_t n, size_t steps,
                     const double *reference)
{
    double *a=malloc(bytes(n)), *b=malloc(bytes(n)); if (!a || !b) exit(EXIT_FAILURE);
    init(a, b, n); omp_set_schedule(sched(kind), chunk);
    double t = omp_get_wtime(); run(&a, &b, n, steps);
    t = omp_get_wtime() - t;
    printf("%s,%d,%.9f,%.17g,%d\n", kind, chunk, t, diff(a, reference, n),
           diff(a, reference, n) <= 1e-12);
    free(a); free(b);
}

int main(int argc, char **argv)
{
    size_t n = argc > 1 ? strtoull(argv[1], NULL, 10) : 2048;
    size_t steps = argc > 2 ? strtoull(argv[2], NULL, 10) : 300;
    omp_set_dynamic(0); omp_set_num_threads(argc > 3 ? atoi(argv[3]) : 12);
    double *ref=malloc(bytes(n)), *tmp=malloc(bytes(n)); if (!ref || !tmp) return EXIT_FAILURE;
    init(ref, tmp, n); omp_set_schedule(omp_sched_static, 0); run(&ref, &tmp, n, steps);
    printf("schedule,chunk,time_s,max_diff,correct\n");
    run_case("static", 0, n, steps, ref);
    run_case("static", 1, n, steps, ref);
    run_case("static", 4, n, steps, ref);
    run_case("static", 16, n, steps, ref);
    run_case("static", 64, n, steps, ref);
    run_case("dynamic", 1, n, steps, ref);
    run_case("dynamic", 4, n, steps, ref);
    run_case("dynamic", 16, n, steps, ref);
    run_case("dynamic", 64, n, steps, ref);
    run_case("guided", 0, n, steps, ref);
    free(ref); free(tmp); return EXIT_SUCCESS;
}
