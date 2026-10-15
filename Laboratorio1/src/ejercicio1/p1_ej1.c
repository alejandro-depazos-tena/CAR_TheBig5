/* TermoHPC - Ejercicio 1: referencia secuencial en C11.
 * Compilar: gcc -Wall -Wextra -O2 -std=c11 -fopenmp termo.c -o termo -lm
 * Uso: ./termo N pasos tolerancia medida|simulacion [comprobar]
 * OpenMP se usa aqui exclusivamente para omp_get_wtime().
 */
#include <errno.h>
#include <math.h>
#include <omp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    size_t pasos;
    double residuo;
    int bordes_ok;
    int rango_ok;
    int residuo_ok;
} Resultado;

/* Cada malla es un vector contiguo: celda (i,j) -> i*n+j.
 * Convenio para las esquinas superiores: pertenecen al borde caliente.
 * No participan en el estarcido de ninguna celda interior.
 */
static void inicializar(double *u, double *nueva, size_t n)
{
    for (size_t k = 0; k < n * n; ++k) {
        u[k] = 0.0;
        nueva[k] = 0.0;
    }
    for (size_t j = 0; j < n; ++j) {
        u[j] = 100.0;
        nueva[j] = 100.0;
    }
}

/* Solo se lee u y solo se escribe el interior de nueva. */
static void paso_jacobi(const double *u, double *nueva, size_t n)
{
    for (size_t i = 1; i < n - 1; ++i) {
        for (size_t j = 1; j < n - 1; ++j) {
            size_t k = i * n + j;
            nueva[k] = 0.25 * (u[k - n] + u[k + n]
                              + u[k - 1] + u[k + 1]);
        }
    }
}

/* Mayor cambio absoluto entre dos pasos, restringido al interior. */
static double calcular_residuo(const double *u, const double *nueva, size_t n)
{
    double r = 0.0;
    for (size_t i = 1; i < n - 1; ++i) {
        for (size_t j = 1; j < n - 1; ++j) {
            size_t k = i * n + j;
            double diferencia = fabs(nueva[k] - u[k]);
            if (diferencia > r) {
                r = diferencia;
            }
        }
    }
    return r;
}

static double calcular_checksum(const double *u, size_t n)
{
    double suma = 0.0;
    for (size_t i = 1; i < n - 1; ++i) {
        for (size_t j = 1; j < n - 1; ++j) {
            suma += u[i * n + j];
        }
    }
    return suma;
}

static void comprobar_estado(const double *u, size_t n, Resultado *r)
{
    for (size_t j = 0; j < n; ++j) {
        if (u[j] != 100.0 || u[(n - 1) * n + j] != 0.0) {
            r->bordes_ok = 0;
        }
    }
    for (size_t i = 1; i < n; ++i) {
        if (u[i * n] != 0.0 || u[i * n + n - 1] != 0.0) {
            r->bordes_ok = 0;
        }
    }
    for (size_t i = 1; i < n - 1; ++i) {
        for (size_t j = 1; j < n - 1; ++j) {
            double t = u[i * n + j];
            if (!isfinite(t) || t < 0.0 || t > 100.0) {
                r->rango_ok = 0;
            }
        }
    }
}

/* Devuelve en *actual la malla final, tambien con un numero impar de pasos.
 * La comprobacion completa se activa en una ejecucion separada sin cronometrar.
 */
static Resultado simular(double **actual, double **auxiliar, size_t n,
                         size_t limite, double tolerancia,
                         int por_tolerancia, int comprobar)
{
    double *u = *actual;
    double *nueva = *auxiliar;
    Resultado r = {0, INFINITY, 1, 1, 1};

    if (comprobar) {
        comprobar_estado(u, n, &r);
        comprobar_estado(nueva, n, &r);
    }
    for (size_t paso = 0; paso < limite; ++paso) {
        paso_jacobi(u, nueva, n);
        double residuo = calcular_residuo(u, nueva, n);

        if (comprobar) {
            comprobar_estado(nueva, n, &r);
            /* Margen absoluto de 1e-12 para el redondeo en double.
             * No es la tolerancia de convergencia de la simulacion.
             */
            if (!isfinite(residuo) || residuo > r.residuo + 1e-12) {
                r.residuo_ok = 0;
            }
        }
        r.residuo = residuo;
        ++r.pasos;

        /* Intercambio de direcciones: no se copia ninguna malla. */
        double *temporal = u;
        u = nueva;
        nueva = temporal;

        if (por_tolerancia && residuo < tolerancia) {
            break;
        }
    }
    *actual = u;
    *auxiliar = nueva;
    return r;
}

static int leer_entero(const char *texto, size_t *valor)
{
    char *fin;
    errno = 0;
    if (texto[0] < '0' || texto[0] > '9') {
        return 0;
    }
    unsigned long long numero = strtoull(texto, &fin, 10);
    if (errno || *fin != '\0' || numero == 0 || numero > SIZE_MAX) {
        return 0;
    }
    *valor = (size_t)numero;
    return 1;
}

int main(int argc, char **argv)
{
    size_t n, limite;

    if (argc < 5 || argc > 7) {
        fprintf(stderr,
                "Uso: %s N pasos tolerancia medida|simulacion [comprobar] [csv]\n",
                argv[0]);
        return EXIT_FAILURE;
    }

    if (!leer_entero(argv[1], &n) || n < 3
        || !leer_entero(argv[2], &limite)) {
        fprintf(stderr,
                "N debe ser >= 3 y pasos debe ser un entero positivo.\n");
        return EXIT_FAILURE;
    }

    char *fin;
    errno = 0;
    double tolerancia = strtod(argv[3], &fin);

    if (errno || fin == argv[3] || *fin != '\0'
        || !isfinite(tolerancia) || tolerancia <= 0.0) {
        fprintf(stderr, "La tolerancia debe ser positiva y finita.\n");
        return EXIT_FAILURE;
    }

    int por_tolerancia = strcmp(argv[4], "simulacion") == 0;

    if (!por_tolerancia && strcmp(argv[4], "medida") != 0) {
        fprintf(stderr, "Modo desconocido: usa medida o simulacion.\n");
        return EXIT_FAILURE;
    }

    /* Opciones de comprobacion y formato de salida. */
    int comprobar = 0;
    int salida_csv = 0;

    for (int i = 5; i < argc; ++i) {
        if (strcmp(argv[i], "comprobar") == 0 && !comprobar) {
            comprobar = 1;
        } else if (strcmp(argv[i], "csv") == 0 && !salida_csv) {
            salida_csv = 1;
        } else {
            fprintf(stderr, "Opcion desconocida o repetida: %s\n", argv[i]);
            return EXIT_FAILURE;
        }
    }

    /* Evita desbordamientos al calcular n*n*sizeof(double). */
    if (n > SIZE_MAX / n || n * n > SIZE_MAX / sizeof(double)) {
        fprintf(stderr, "Tamano de malla demasiado grande.\n");
        return EXIT_FAILURE;
    }

    size_t bytes = n * n * sizeof(double);
    double *u = malloc(bytes);
    double *nueva = malloc(bytes);

    if (u == NULL || nueva == NULL) {
        fprintf(stderr, "No hay memoria suficiente para las dos mallas.\n");
        free(u);
        free(nueva);
        return EXIT_FAILURE;
    }

    /*
     * Conservamos el resultado de la validacion para mostrarlo al final.
     * Sus valores solo se consultan si se ha solicitado comprobar.
     */
    Resultado verificacion = {0, 0.0, 0, 0, 0};

    /* Validacion en todos los pasos, fuera de la ejecucion medida. */
    if (comprobar) {
        inicializar(u, nueva, n);

        verificacion = simular(&u, &nueva, n, limite, tolerancia,
                               por_tolerancia, 1);

        if (!verificacion.bordes_ok
            || !verificacion.rango_ok
            || !verificacion.residuo_ok) {
            fprintf(stderr,
                    "ERROR: la simulacion no supera las comprobaciones.\n"
                    "Bordes constantes:     %s\n"
                    "Temperaturas en rango: %s\n"
                    "Residuo no creciente:  %s\n",
                    verificacion.bordes_ok ? "OK" : "FALLO",
                    verificacion.rango_ok ? "OK" : "FALLO",
                    verificacion.residuo_ok ? "OK" : "FALLO");

            free(u);
            free(nueva);
            return EXIT_FAILURE;
        }
    }

    /* El cronometro excluye reserva, inicializacion, checksum e impresion. */
    inicializar(u, nueva, n);

    double inicio = 0.0;

    if (!por_tolerancia) {
        inicio = omp_get_wtime();
    }

    Resultado r = simular(&u, &nueva, n, limite, tolerancia,
                         por_tolerancia, 0);

    double tiempo = 0.0;

    if (!por_tolerancia) {
        tiempo = omp_get_wtime() - inicio;
    }

    double checksum = calcular_checksum(u, n);

    int convergencia = por_tolerancia && r.residuo < tolerancia;

    if (salida_csv) {
        /* Formato procesable automaticamente, con precision completa. */
        puts("N,modo,pasos,tolerancia,residuo,checksum,tiempo_s,comprobaciones,parada");

        printf("%zu,%s,%zu,%.17g,%.17g,%.17g,",
               n, argv[4], r.pasos, tolerancia, r.residuo, checksum);

        if (por_tolerancia) {
            printf("NA,");
        } else {
            printf("%.9f,", tiempo);
        }

        printf("%s,%s\n",
               comprobar ? "OK" : "no_ejecutadas",
               convergencia ? "tolerancia" : "limite_pasos");

    } else {
        /* Resumen legible para la terminal. */
        printf("\nSIMULACIÓN TÉRMICA — VERSIÓN SECUENCIAL\n\n");

        printf("Tamaño de la malla:      %zu × %zu\n", n, n);
        printf("Modo:                   %s\n",
               por_tolerancia
                   ? "Simulación (parada por tolerancia)"
                   : "Medida (pasos fijos)");

        printf("Pasos ejecutados:       %zu\n", r.pasos);
        printf("Tolerancia configurada: %.6g", tolerancia);

        if (!por_tolerancia) {
            printf(" (no se aplica en modo medida)");
        }

        printf("\n");
        printf("Residuo final:          %.12g\n", r.residuo);
        printf("Checksum interior:      %.17g\n", checksum);

        if (por_tolerancia) {
            printf("Tiempo de ejecución:    No cronometrado (modo simulación)\n");
        } else {
            printf("Tiempo de ejecución:    %.9f s\n", tiempo);
        }

        printf("Motivo de parada:       %s\n",
               convergencia
                   ? "Tolerancia alcanzada"
                   : "Límite de pasos alcanzado");

        printf("\nCOMPROBACIONES\n");

        if (comprobar) {
            printf("Bordes constantes:      %s\n",
                   verificacion.bordes_ok ? "OK" : "FALLO");

            printf("Temperaturas en rango:  %s\n",
                   verificacion.rango_ok ? "OK" : "FALLO");

            printf("Residuo no creciente:   %s\n",
                   verificacion.residuo_ok ? "OK" : "FALLO");

            printf("Resultado global:       OK\n");
        } else {
            printf("Estado:                 No ejecutadas\n");
        }

        printf("\n");
    }

    free(u);
    free(nueva);
    return EXIT_SUCCESS;
}
