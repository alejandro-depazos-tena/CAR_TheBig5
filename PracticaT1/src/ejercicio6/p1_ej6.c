#include <math.h>
#include <omp.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static size_t bytes(size_t n){return n*n*sizeof(double);}
static void init(double*a,double*b,size_t n){memset(a,0,bytes(n));memset(b,0,bytes(n));for(size_t j=0;j<n;++j)a[j]=b[j]=100.0;}
static double step(const double*a,double*b,size_t n){double r=0.0;
#pragma omp parallel for default(none) shared(a,b,n) reduction(max:r) schedule(static)
for(size_t i=1;i+1<n;++i){double row=0.0;for(size_t j=1;j+1<n;++j){size_t k=i*n+j;double v=.25*(a[k-n]+a[k+n]+a[k-1]+a[k+1]);b[k]=v;if(fabs(v-a[k])>row)row=fabs(v-a[k]);}if(row>r)r=row;}return r;}
static double collapse_step(const double*a,double*b,size_t n){double r=0.0;
#pragma omp parallel for default(none) shared(a,b,n) reduction(max:r) collapse(2) schedule(static)
for(size_t i=1;i+1<n;++i)for(size_t j=1;j+1<n;++j){size_t k=i*n+j;double v=.25*(a[k-n]+a[k+n]+a[k-1]+a[k+1]);b[k]=v;if(fabs(v-a[k])>r)r=fabs(v-a[k]);}return r;}

static double measure(size_t n, size_t steps, int mode, int threads)
{
    double *a=malloc(bytes(n)), *b=malloc(bytes(n)); if (!a || !b) exit(EXIT_FAILURE);
    init(a, b, n); omp_set_dynamic(0); omp_set_num_threads(threads);
    omp_set_schedule(omp_sched_static, 0);
    double t = omp_get_wtime();
    if (mode == 0) for (size_t s=0;s<steps;++s) { step(a,b,n); double*x=a;a=b;b=x; }
    else if (mode == 1) for (size_t s=0;s<steps;++s) { collapse_step(a,b,n); double*x=a;a=b;b=x; }
    else {
        size_t s;
#pragma omp parallel default(none) shared(a, b, n, steps) private(s)
        {
            for (s = 0; s < steps; ++s) {
#pragma omp for schedule(static)
                for (size_t i = 1; i + 1 < n; ++i)
                    for (size_t j = 1; j + 1 < n; ++j) {
                        size_t k = i * n + j;
                        b[k] = 0.25 * (a[k - n] + a[k + n] + a[k - 1] + a[k + 1]);
                    }
#pragma omp barrier
#pragma omp single
                { double *x = a; a = b; b = x; }
#pragma omp barrier
            }
        }
    }
    t = omp_get_wtime() - t; free(a); free(b); return t;
}

int main(int argc, char **argv)
{
    size_t n = argc > 1 ? strtoull(argv[1], NULL, 10) : 2048;
    size_t steps = argc > 2 ? strtoull(argv[2], NULL, 10) : 300;
    int p = argc > 3 ? atoi(argv[3]) : 12;
    double base = measure(n, steps, 0, p);
    printf("variant,time_s,speedup,improvement,bytes_per_step,estimated_Bps\n");
    for (int mode = 0; mode < 3; ++mode) {
        double t = mode == 0 ? base : measure(n, steps, mode, p);
        double moved = (double)(n - 2) * (n - 2) * 6.0 * sizeof(double);
        printf("%s,%.9f,%.6f,%.6f,%.0f,%.6g\n",
               mode == 0 ? "baseline" : mode == 1 ? "collapse2" : "single_region",
               t, base / t, (base - t) / base, moved, moved * steps / t);
    }
    puts("Nota: medir tambien una variante de residuo cada k pasos y afinidad "
         "OMP_PROC_BIND para documentar una iteracion que no mejore.");
    return EXIT_SUCCESS;
}
