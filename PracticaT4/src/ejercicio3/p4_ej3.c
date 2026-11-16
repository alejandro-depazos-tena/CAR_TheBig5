/* p4_ej3.c - comparar una recogida manual con MPI_Reduce.
   mpicc -O2 -Wall -Wextra -o p4_ej3 p4_ej3.c
   mpiexec -n 8 ./p4_ej3 */
#include <stdio.h>
#include <mpi.h>

#define REP 2000
#define ET_SUMANDO 0

/* Cada rango manda su numero al 0, que los recibe y suma en orden de rango. */
static double a_mano(double mio, int yo, int cuantos, double *total)
{
    double t0 = MPI_Wtime();
    double suma = 0.0;

    for (int r = 0; r < REP; r++) {
        if (yo != 0) {
            MPI_Send(&mio, 1, MPI_DOUBLE, 0, ET_SUMANDO, MPI_COMM_WORLD);
        } else {
            suma = mio;
            for (int k = 1; k < cuantos; k++) {
                double ajeno;
                MPI_Recv(&ajeno, 1, MPI_DOUBLE, k, ET_SUMANDO,
                         MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                suma += ajeno;
            }
        }
    }

    *total = suma;
    return (MPI_Wtime() - t0) / REP;
}

/* Todos los rangos participan; solo el rango 0 recibe el resultado. */
static double con_reduce(double mio, int yo, int cuantos, double *total)
{
    double t0 = MPI_Wtime();
    double suma = 0.0;

    (void)yo;
    (void)cuantos;
    for (int r = 0; r < REP; r++) {
        MPI_Reduce(&mio, &suma, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    }

    *total = suma;
    return (MPI_Wtime() - t0) / REP;
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int yo;
    int cuantos;
    MPI_Comm_rank(MPI_COMM_WORLD, &yo);
    MPI_Comm_size(MPI_COMM_WORLD, &cuantos);

    double mio = (double)(yo + 1);
    double t_mano;
    double t_reduce;
    double suma_mano = 0.0;
    double suma_reduce = 0.0;

    MPI_Barrier(MPI_COMM_WORLD);
    t_mano = a_mano(mio, yo, cuantos, &suma_mano);
    MPI_Barrier(MPI_COMM_WORLD);
    t_reduce = con_reduce(mio, yo, cuantos, &suma_reduce);

    double esperado = (double)cuantos * (cuantos + 1) / 2.0;
    int correcto = (suma_mano == esperado && suma_reduce == esperado);
    if (yo == 0) {
        printf("%3d procesos   a mano %8.1f us   MPI_Reduce %8.1f us   "
               "%5.2fx   %s\n",
               cuantos, t_mano * 1e6, t_reduce * 1e6,
               t_mano / t_reduce, correcto ? "suma correcta" : "SUMA MAL");
    }

    MPI_Finalize();
    return correcto ? 0 : 1;
}
