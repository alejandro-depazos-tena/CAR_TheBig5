/* p4_ej2.c - el interbloqueo, su umbral exacto y tres formas de arreglarlo.
   Dos procesos se intercambian un mensaje de M doubles.
   mpicc -O2 -Wall -Wextra -o p4_ej2 p4_ej2.c
   mpirun -np 2 ./p4_ej2 <M> <modo>      modo: send | sendrecv | orden | noblo */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define REP 20

static double *envio, *recibo;

/* Los dos mandan primero y reciben despues: esto es lo que se cuelga. */
static void con_send(long m, int yo, int otro)
{
    (void)yo;
    MPI_Send(envio, m, MPI_DOUBLE, otro, 0, MPI_COMM_WORLD);
    MPI_Recv(recibo, m, MPI_DOUBLE, otro, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

/* Arreglo 2: uno manda mientras el otro recibe, y luego al reves. */
static void con_orden(long m, int yo, int otro)
{
    if (yo == 0) {
        MPI_Send(envio, m, MPI_DOUBLE, otro, 0, MPI_COMM_WORLD);
        MPI_Recv(recibo, m, MPI_DOUBLE, otro, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    } else {
        MPI_Recv(recibo, m, MPI_DOUBLE, otro, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Send(envio, m, MPI_DOUBLE, otro, 0, MPI_COMM_WORLD);
    }
}

/* Arreglo 3: se anuncian las dos operaciones y se espera al final. */
static void con_noblo(long m, int yo, int otro)
{
    MPI_Request p[2];
    MPI_Status e[2];
    (void)yo;
    MPI_Irecv(recibo, m, MPI_DOUBLE, otro, 0, MPI_COMM_WORLD, &p[0]);
    MPI_Isend(envio, m, MPI_DOUBLE, otro, 0, MPI_COMM_WORLD, &p[1]);
    MPI_Waitall(2, p, e);
}

/* Arreglo 1: una sola llamada que manda y recibe a la vez.
   EJERCICIO 2: escribela con MPI_Sendrecv (apartado 4.2.6). */
static void con_sendrecv(long m, int yo, int otro)
{
    (void)yo;   /* los dos rangos hacen exactamente lo mismo: no hace falta */
    /* Primero los cinco argumentos del envio y despues los cinco de la
       recepcion. El destino y el origen son el mismo proceso, 'otro', y las
       dos etiquetas valen 0 porque el otro rango tambien manda con la 0: si
       no coincidieran, el mensaje no se emparejaria y se quedaria esperando.
       Cada sentido tiene su propio buffer: no se puede recibir encima de lo
       que se esta mandando (para eso existe MPI_Sendrecv_replace).
       MPI hace avanzar el envio y la recepcion a la vez, asi que nunca se
       queda bloqueado esperando a que el otro reciba (apartado 4.2.6). */
    MPI_Sendrecv(envio,  m, MPI_DOUBLE, otro, 0,
                 recibo, m, MPI_DOUBLE, otro, 0,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    int yo, cuantos;
    MPI_Comm_rank(MPI_COMM_WORLD, &yo);
    MPI_Comm_size(MPI_COMM_WORLD, &cuantos);
    if (cuantos != 2) {
        if (yo == 0) fprintf(stderr, "este programa se lanza con -np 2\n");
        MPI_Finalize();
        return 1;
    }
    long m = (argc > 1) ? atol(argv[1]) : 1;
    const char *modo = (argc > 2) ? argv[2] : "send";
    int otro = 1 - yo;
    envio = malloc(m * sizeof *envio);
    recibo = malloc(m * sizeof *recibo);
    if (!envio || !recibo) {
        fprintf(stderr, "sin memoria\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    for (long i = 0; i < m; i++) envio[i] = (double)(yo + 1);

    MPI_Barrier(MPI_COMM_WORLD);
    double t0 = MPI_Wtime();
    for (int r = 0; r < REP; r++) {
        if (!strcmp(modo, "send")) con_send(m, yo, otro);
        else if (!strcmp(modo, "sendrecv")) con_sendrecv(m, yo, otro);
        else if (!strcmp(modo, "orden")) con_orden(m, yo, otro);
        else con_noblo(m, yo, otro);
    }
    double t = (MPI_Wtime() - t0) / REP;

    int bien = (recibo[0] == (double)(otro + 1) && recibo[m - 1] == (double)(otro + 1));
    if (yo == 0)
        printf("%-9s m = %9ld doubles (%8.3f MB) %8.1f us por intercambio %s\n",
               modo, m, (double)m * sizeof(double) / 1048576.0, t * 1e6,
               bien ? "dato correcto" : "DATO MAL");
    free(envio);
    free(recibo);
    MPI_Finalize();
    return 0;
}
