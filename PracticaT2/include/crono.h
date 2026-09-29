/* crono.h - reloj monotono de pared, portable entre Windows y Linux. */
#ifndef CRONO_H
#define CRONO_H

#ifdef _WIN32
#include <windows.h>
static double reloj(void)
{
    LARGE_INTEGER frecuencia, instante;
    QueryPerformanceFrequency(&frecuencia);
    QueryPerformanceCounter(&instante);
    return (double)instante.QuadPart / (double)frecuencia.QuadPart;
}
#else
#include <time.h>
static double reloj(void)
{
    struct timespec instante;
    clock_gettime(CLOCK_MONOTONIC, &instante);
    return (double)instante.tv_sec + (double)instante.tv_nsec / 1e9;
}
#endif

#endif