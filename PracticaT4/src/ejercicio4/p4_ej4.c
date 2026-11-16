/* p4_ej4.c - latencia y ancho de banda mediante ping-pong MPI.
   mpicc -O2 -Wall -Wextra -o p4_ej4 p4_ej4.c
   mpiexec -n 2 ./p4_ej4 */
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define ETIQUETA 0

static int repeticiones(size_t bytes)
{
    if (bytes <= 2048U) return 10000;
    if (bytes <= 32768U) return 5000;
    if (bytes <= 524288U) return 1000;
    if (bytes <= 2097152U) return 200;
    return 50;
}

static double ping_pong(unsigned char *buffer, size_t bytes, int rep,
                        int yo, int otro)
{
    double inicio = MPI_Wtime();

    for (int r = 0; r < rep; r++) {
        if (yo == 0) {
            MPI_Send(buffer, (int)bytes, MPI_BYTE, otro, ETIQUETA,
                     MPI_COMM_WORLD);
            MPI_Recv(buffer, (int)bytes, MPI_BYTE, otro, ETIQUETA,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        } else {
            MPI_Recv(buffer, (int)bytes, MPI_BYTE, otro, ETIQUETA,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(buffer, (int)bytes, MPI_BYTE, otro, ETIQUETA,
                     MPI_COMM_WORLD);
        }
    }

    return MPI_Wtime() - inicio;
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int yo;
    int cuantos;
    MPI_Comm_rank(MPI_COMM_WORLD, &yo);
    MPI_Comm_size(MPI_COMM_WORLD, &cuantos);
    if (cuantos != 2) {
        if (yo == 0)
            fprintf(stderr, "Este programa necesita exactamente 2 procesos.\n");
        MPI_Finalize();
        return 1;
    }

    const int otro = 1 - yo;
    for (int exponente = 0; exponente <= 10; exponente++) {
        size_t bytes = 8U;
        for (int i = 0; i < exponente; i++) bytes *= 4U;

        unsigned char *buffer = malloc(bytes);
        if (buffer == NULL) {
            fprintf(stderr, "No hay memoria para %zu bytes.\n", bytes);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        for (size_t i = 0; i < bytes; i++) buffer[i] = (unsigned char)(i % 251U);

        int rep = repeticiones(bytes);
        int calentamiento = 10;
        (void)ping_pong(buffer, bytes, calentamiento, yo, otro);
        MPI_Barrier(MPI_COMM_WORLD);
        double segundos = ping_pong(buffer, bytes, rep, yo, otro);
        MPI_Barrier(MPI_COMM_WORLD);

        int correcto = (buffer[0] == 0U &&
                        buffer[bytes - 1] == (unsigned char)((bytes - 1U) % 251U));
        if (yo == 0) {
            double ida_vuelta_us = segundos * 1e6 / (double)rep;
            double una_direccion_us = ida_vuelta_us / 2.0;
            double megabytes_s = (double)bytes / (una_direccion_us * 1e-6) / 1e6;
            printf("%zu,%d,%.6f,%.6f,%.3f,%s\n",
                   bytes, rep, ida_vuelta_us, una_direccion_us, megabytes_s,
                   correcto ? "correcto" : "DATO_MAL");
            fflush(stdout);
        }
        free(buffer);
        if (!correcto) {
            MPI_Finalize();
            return 1;
        }
    }

    MPI_Finalize();
    return 0;
}
