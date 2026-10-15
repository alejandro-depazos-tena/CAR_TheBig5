#include <math.h>
#include <omp.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static size_t bytes(size_t n){return n*n*sizeof(double);}
static void init(double*a,double*b,size_t n){memset(a,0,bytes(n));memset(b,0,bytes(n));for(size_t j=0;j<n;++j)a[j]=b[j]=100.0;}
static void run(double**a,double**b,size_t n,size_t steps){for(size_t s=0;s<steps;++s){
#pragma omp parallel for default(none) shared(a,b,n) schedule(static)
for(size_t i=1;i+1<n;++i)for(size_t j=1;j+1<n;++j){size_t k=i*n+j;b[k]=.25*((*a)[k-n]+(*a)[k+n]+(*a)[k-1]+(*a)[k+1]);}
double*t=*a;*a=*b;*b=t;}}

int main(int argc, char **argv)
{
    size_t n1 = argc > 1 ? strtoull(argv[1], NULL, 10) : 1024;
    size_t steps = argc > 2 ? strtoull(argv[2], NULL, 10) : 100;
    const int counts[] = {1,2,4,8,12,16,20,24};
    double reference = 0.0;
    printf("threads,N,steps,time_s,weak_efficiency,bytes_per_grid\n");
    for (size_t q = 0; q < sizeof(counts)/sizeof(counts[0]); ++q) {
        int p = counts[q];
        size_t n = (size_t)llround((double)n1 * sqrt((double)p));
        double *a=malloc(bytes(n)), *b=malloc(bytes(n)); if (!a || !b) return EXIT_FAILURE;
        init(a, b, n); omp_set_dynamic(0); omp_set_num_threads(p);
        double t = omp_get_wtime(); run(&a, &b, n, steps);
        t = omp_get_wtime() - t; if (p == 1) reference = t;
        printf("%d,%zu,%zu,%.9f,%.6f,%zu\n", p, n, steps, t,
               reference / t, bytes(n) * 2);
        free(a); free(b);
    }
    return EXIT_SUCCESS;
}
