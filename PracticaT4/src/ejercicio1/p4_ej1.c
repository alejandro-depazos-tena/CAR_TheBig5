/* p4_ej1.c - el entorno, la salida desordenada y el reparto por rango.
   mpicc -O2 -Wall -Wextra -o p4_ej1 p4_ej1.c -lm ; mpirun -np 4 ./p4_ej1 */
#include <stdio.h>
#include <math.h>
#include <mpi.h>

#define N 200000000L                /* sumandos en total */
#define ET_PARCIAL 1                /* etiqueta de los mensajes de recogida */

/* El trozo que le toca al rango 'yo' cuando hay 'cuantos' procesos: los
   primeros N%cuantos procesos se llevan un sumando mas. */
static void reparto(long n, int yo, int cuantos, long *inicio, long *fin)
{
    long base = n / cuantos, sobran = n % cuantos;
    *inicio = yo * base + (yo < sobran ? yo : sobran);
    *fin = *inicio + base + (yo < sobran ? 1 : 0);
}

static double trozo(long inicio, long fin)
{
    double s = 0.0;
    for (long i = inicio; i < fin; i++) s += sqrt((double)i + 0.5);
    return s;
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    int yo, cuantos;
    MPI_Comm_rank(MPI_COMM_WORLD, &yo);
    MPI_Comm_size(MPI_COMM_WORLD, &cuantos);

    printf("hola, soy el rango %d de %d\n", yo, cuantos);
    fflush(stdout);

    long inicio, fin;
    reparto(N, yo, cuantos, &inicio, &fin);
    double t0 = MPI_Wtime();
    double mio = trozo(inicio, fin);
    double t = MPI_Wtime() - t0;
    /* EJERCICIO 1: que cada rango mande su resultado parcial al rango 0 y que
       el rango 0 los sume todos e imprima el total junto con su propio tiempo.
       Con MPI_Send y MPI_Recv, apartado 4.2.1. */

    /* Cada rango manda en UN solo mensaje su suma parcial y su tiempo: dos
       double cuestan lo mismo que uno (4.2.5) y el rango 0 puede saber
       cuanto tardo el mas lento, que es lo que tarda el programa (4.5.2). */
    double parcial[2] = { mio, t };

    if (yo != 0) {
        MPI_Send(parcial, 2, MPI_DOUBLE, 0, ET_PARCIAL, MPI_COMM_WORLD);
    } else {
        double total = mio;
        double t_max = t;
        int rango_lento = 0;
        double recibido[2];
        /* El proceso 0 recibe los valores de los demas en orden fijo de rango: la suma sale
           siempre en el mismo orden y es reproducible bit a bit*/
        for (int origen = 1; origen < cuantos; origen++) {
            MPI_Recv(recibido, 2, MPI_DOUBLE, origen, ET_PARCIAL,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            total += recibido[0];
            if (recibido[1] > t_max) {
                t_max = recibido[1];
                rango_lento = origen;
            }
        }
        printf("suma con %d procesos = %e  el rango 0 tardo %.4f s\n",
               cuantos, total, t);
        printf("  suma completa = %.15e  el mas lento fue el rango %d: %.4f s\n",
               total, rango_lento, t_max);
    }

    MPI_Finalize();
    return 0;
}
