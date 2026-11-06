#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    const int N = 17;
    int rango, procesos;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rango);
    MPI_Comm_size(MPI_COMM_WORLD, &procesos);

    if (procesos != 5) {
        if (rango == 0)
            fprintf(stderr, "Ejecuta el programa con 5 procesos.\n");
        MPI_Finalize();
        return 1;
    }

    /* Reparto por bloques: division entera. */
    int inicio = (rango * N) / procesos;
    int fin = ((rango + 1) * N) / procesos;

    /* Los turnos solo sirven para ordenar la salida. */
    for (int turno = 0; turno < procesos; turno++) {
        if (rango == turno) {
            printf("Proceso %d\n", rango);

            printf("  Bloques: ");
            for (int i = inicio; i < fin; i++)
                printf("%d ", i);
            printf("| total = %d\n", fin - inicio);

            printf("  Ciclico: ");
            int cantidad = 0;
            for (int i = rango; i < N; i += procesos) {
                printf("%d ", i);
                cantidad++;
            }
            printf("| total = %d\n", cantidad);

            fflush(stdout);
        }

        MPI_Barrier(MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}