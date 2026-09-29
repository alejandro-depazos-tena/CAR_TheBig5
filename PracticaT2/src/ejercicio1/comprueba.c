/* comprueba.c - validacion previa del entorno de la practica. */
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include "crono.h"

static atomic_long terminados;

static void *trabajador(void *arg)
{
    (void)arg;
    atomic_fetch_add(&terminados, 1);
    return NULL;
}

int main(void)
{
    pthread_t hilos[4];
    double inicio = reloj();
    for (int i = 0; i < 4; i++) {
        if (pthread_create(&hilos[i], NULL, trabajador, NULL) != 0) return 1;
    }
    for (int i = 0; i < 4; i++) {
        if (pthread_join(hilos[i], NULL) != 0) return 1;
    }

    double transcurrido = reloj() - inicio;
    double paso = 1.0;
    for (int i = 0; i < 1000; i++) {
        double anterior = reloj(), actual;
        do actual = reloj(); while (actual == anterior);
        if (actual - anterior < paso) paso = actual - anterior;
    }

    long total = atomic_load(&terminados);
    printf("hilos terminados        : %ld de 4\n", total);
    printf("crear y esperar 4 hilos : %.3f ms\n", transcurrido * 1e3);
    printf("resolucion del reloj    : %.0f ns\n", paso * 1e9);
    printf("%s\n", (total == 4 && paso < 1e-6)
                       ? "ENTORNO LISTO"
                       : "ALGO FALLA: avisa antes de la sesion");
    return (total == 4 && paso < 1e-6) ? 0 : 1;
}