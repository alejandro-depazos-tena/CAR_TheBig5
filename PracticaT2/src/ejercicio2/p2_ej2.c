/* p2_ej2.c - con que frecuencia falla una carrera (apartado 2.2.1). */
#include <stdio.h>
#include <pthread.h>
#define PRUEBAS 20

static long long contador; /* volatile: obliga a ir a memoria */
static int vueltas_por_hilo;

static void *sube(void *arg)
{
    (void)arg;
    for (int i = 0; i < vueltas_por_hilo; i++)
        contador++; /* parece una operacion; son tres */
    return NULL;
}
/* Devuelve el valor final del contador para P hilos y K vueltas cada uno. */
static long long una_ejecucion(int P, int K)
{
    pthread_t h[32];
    contador = 0;
    vueltas_por_hilo = K;
    for (int i = 0; i < P; i++) pthread_create(&h[i], NULL, sube, NULL);
    for (int i = 0; i < P; i++) pthread_join(h[i], NULL);
    return contador;
}

int main(void)
{
    int hilos[] = {2, 4, 8, 12, 24};
    int vueltas[] = {1000, 100000, 10000000};
    for (int k = 0; k < 3; k++) {
        printf("K = %d incrementos por hilo\n", vueltas[k]);
        printf(" hilos esperado minimo obtenido fallan perdida media\n");
        for (int p = 0; p < 5; p++) {
            long long esperado = (long long)hilos[p] * vueltas[k];
            long long minimo = esperado;
            int fallan = 0;
            double perdida = 0;
            for (int r = 0; r < PRUEBAS; r++) {
                long long v = una_ejecucion(hilos[p], vueltas[k]);
                if (v != esperado) fallan++;
                if (v < minimo) minimo = v;
                perdida += 100.0 * (double)(esperado - v) / (double)esperado;
            }
            printf(" %5d %11lld %17lld %3d/%d %10.1f %%\n", hilos[p], esperado,
            minimo, fallan, PRUEBAS, perdida / PRUEBAS);
        }
    }
    return 0;
}