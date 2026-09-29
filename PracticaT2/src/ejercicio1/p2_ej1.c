/* p2_ej1.c - coste de crear y esperar hilos y procesos. */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "crono.h"

#ifndef _WIN32
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

#define N_HILOS 1000
#define N_PROC 200
#define REP 5

static void error(const char *mensaje)
{
    fprintf(stderr, "%s\n", mensaje);
    exit(EXIT_FAILURE);
}

static void *nada_hilo(void *arg)
{
    (void)arg;
    return NULL;
}

static double hilos_tanda(int n)
{
    pthread_t *hilos = malloc((size_t)n * sizeof *hilos);
    if (hilos == NULL) error("no se pudo reservar memoria para los hilos");
    double inicio = reloj();
    for (int i = 0; i < n; i++)
        if (pthread_create(&hilos[i], NULL, nada_hilo, NULL) != 0)
            error("no se pudo crear un hilo");
    for (int i = 0; i < n; i++)
        if (pthread_join(hilos[i], NULL) != 0)
            error("no se pudo esperar un hilo");
    double tiempo = reloj() - inicio;
    free(hilos);
    return tiempo / n;
}

static double hilos_uno_a_uno(int n)
{
    pthread_t hilo;
    double inicio = reloj();
    for (int i = 0; i < n; i++) {
        if (pthread_create(&hilo, NULL, nada_hilo, NULL) != 0)
            error("no se pudo crear un hilo");
        if (pthread_join(hilo, NULL) != 0)
            error("no se pudo esperar un hilo");
    }
    return (reloj() - inicio) / n;
}

#ifdef _WIN32
#include <windows.h>

static void lanza(PROCESS_INFORMATION *proceso)
{
    STARTUPINFOA inicio;
    char linea[] = "nada.exe";
    ZeroMemory(&inicio, sizeof inicio);
    inicio.cb = sizeof inicio;
    ZeroMemory(proceso, sizeof *proceso);
    if (!CreateProcessA(NULL, linea, NULL, NULL, FALSE, CREATE_NO_WINDOW,
                        NULL, NULL, &inicio, proceso)) {
        fprintf(stderr, "no se puede lanzar nada.exe: compilalo primero\n");
        exit(EXIT_FAILURE);
    }
}

static void espera(PROCESS_INFORMATION *proceso)
{
    WaitForSingleObject(proceso->hProcess, INFINITE);
    CloseHandle(proceso->hProcess);
    CloseHandle(proceso->hThread);
}

static double procesos_tanda(int n)
{
    PROCESS_INFORMATION *procesos = malloc((size_t)n * sizeof *procesos);
    if (procesos == NULL) error("no se pudo reservar memoria para los procesos");
    double inicio = reloj();
    for (int i = 0; i < n; i++) lanza(&procesos[i]);
    for (int i = 0; i < n; i++) espera(&procesos[i]);
    double tiempo = reloj() - inicio;
    free(procesos);
    return tiempo / n;
}

static double procesos_uno_a_uno(int n)
{
    PROCESS_INFORMATION proceso;
    double inicio = reloj();
    for (int i = 0; i < n; i++) {
        lanza(&proceso);
        espera(&proceso);
    }
    return (reloj() - inicio) / n;
}

#else

static double procesos_tanda(int n)
{
    pid_t *procesos = malloc((size_t)n * sizeof *procesos);
    if (procesos == NULL) error("no se pudo reservar memoria para los procesos");
    double inicio = reloj();
    for (int i = 0; i < n; i++) {
        procesos[i] = fork();
        if (procesos[i] == 0) _exit(EXIT_SUCCESS);
        if (procesos[i] < 0) error("no se pudo crear un proceso");
    }
    for (int i = 0; i < n; i++)
        if (waitpid(procesos[i], NULL, 0) < 0)
            error("no se pudo esperar un proceso");
    double tiempo = reloj() - inicio;
    free(procesos);
    return tiempo / n;
}

static double procesos_uno_a_uno(int n)
{
    double inicio = reloj();
    for (int i = 0; i < n; i++) {
        pid_t proceso = fork();
        if (proceso == 0) _exit(EXIT_SUCCESS);
        if (proceso < 0) error("no se pudo crear un proceso");
        if (waitpid(proceso, NULL, 0) < 0)
            error("no se pudo esperar un proceso");
    }
    return (reloj() - inicio) / n;
}

#endif

static double mide(const char *nombre, double (*medida)(int), int n)
{
    double suma = 0.0, minimo = 1e30;
    for (int repeticion = 0; repeticion < REP; repeticion++) {
        double tiempo = medida(n);
        suma += tiempo;
        if (tiempo < minimo) minimo = tiempo;
    }
    printf("%-18s: %10.1f us %10.1f us\n", nombre,
           minimo * 1e6, suma / REP * 1e6);
    return minimo;
}

int main(void)
{
    printf("%-18s  %13s %13s   (%d repeticiones)\n",
           "", "minimo", "media", REP);
    double a = mide("hilo    en tanda", hilos_tanda, N_HILOS);
    double b = mide("hilo    uno a uno", hilos_uno_a_uno, N_HILOS);
    double c = mide("proceso en tanda", procesos_tanda, N_PROC);
    double d = mide("proceso uno a uno", procesos_uno_a_uno, N_PROC);
    printf("factor proceso/hilo en tanda  : %6.0f x\n", c / a);
    printf("factor proceso/hilo uno a uno : %6.0f x\n", d / b);
    return 0;
}