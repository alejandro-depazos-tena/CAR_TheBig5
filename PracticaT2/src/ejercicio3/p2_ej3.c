#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>
#include "crono.h"

#define K 5000000
#define REP 5

static volatile long long c_suelto;
static long long c_mutex;
static pthread_mutex_t cerrojo = PTHREAD_MUTEX_INITIALIZER;
static atomic_long c_atomico;
static long long c_priv;
static pthread_mutex_t cerrojo_final = PTHREAD_MUTEX_INITIALIZER;

static void *v_suelto(void *arg)
{
    (void)arg;
    for (int i = 0; i < K; i++) {
        c_suelto++;
    }
    return NULL;
}

static void *v_mutex(void *arg)
{
    (void)arg;
    for (int i = 0; i < K; i++) {
        pthread_mutex_lock(&cerrojo);
        c_mutex++;
        pthread_mutex_unlock(&cerrojo);
    }
    return NULL;
}

static void *v_atomico(void *arg)
{
    (void)arg;
    for (int i = 0; i < K; i++) {
        atomic_fetch_add(&c_atomico, 1);
    }
    return NULL;
}

static void *v_priv(void *arg)
{
    (void)arg;
    volatile long long mio = 0;
    for (int i = 0; i < K; i++) {
        mio++;
    }
    pthread_mutex_lock(&cerrojo_final);
    c_priv += mio;
    pthread_mutex_unlock(&cerrojo_final);
    return NULL;
}

static void r_suelto(void)  { c_suelto = 0; }
static void r_mutex(void)   { c_mutex = 0; }
static void r_atomico(void) { atomic_store(&c_atomico, 0); }
static void r_priv(void)    { c_priv = 0; }

static double cronometra(void *(*funcion)(void *), void (*reinicia)(void), int hilos)
{
    double mejor = 1e30;
    pthread_t trabajadores[24];

    for (int repeticion = 0; repeticion < REP; repeticion++) {
        reinicia();
        double inicio = reloj();

        for (int i = 0; i < hilos; i++) {
            pthread_create(&trabajadores[i], NULL, funcion, NULL);
        }
        for (int i = 0; i < hilos; i++) {
            pthread_join(trabajadores[i], NULL);
        }

        double tiempo = reloj() - inicio;
        if (tiempo < mejor) {
            mejor = tiempo;
        }
    }
    return mejor;
}

int main(void)
{
    const int configuraciones[] = {1, 2, 4, 8, 12, 24};
    const int numero_configuraciones = (int)(sizeof configuraciones / sizeof configuraciones[0]);

    printf("   P  suelto(s)  mutex(s)  atomico(s)  privado(s)  correctas  suelto\n");
    for (int indice = 0; indice < numero_configuraciones; indice++) {
        int hilos = configuraciones[indice];
        long long esperado = (long long)hilos * K;
        double tiempo_suelto = cronometra(v_suelto, r_suelto, hilos);
        double tiempo_mutex = cronometra(v_mutex, r_mutex, hilos);
        double tiempo_atomico = cronometra(v_atomico, r_atomico, hilos);
        double tiempo_privado = cronometra(v_priv, r_priv, hilos);
        int correctas = c_mutex == esperado
                     && atomic_load(&c_atomico) == esperado
                     && c_priv == esperado;

        printf("%4d %10.4f %9.4f %11.4f %11.4f  %-9s  %s\n",
               hilos, tiempo_suelto, tiempo_mutex, tiempo_atomico, tiempo_privado,
               correctas ? "las 3 si" : "NO",
               c_suelto == esperado ? "si" : "NO");
    }
    return 0;
}