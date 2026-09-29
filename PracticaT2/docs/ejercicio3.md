# Práctica 2 · Ejercicio 3

## Tres arreglos, tres precios

Este ejercicio compara cuatro formas de sumar `K = 5.000.000` incrementos por hilo:

- `suelto`: incremento sin proteger. Es solo una referencia y puede producir un resultado incorrecto.
- `mutex`: toma y libera un cerrojo alrededor de cada incremento.
- `atomico`: usa `atomic_fetch_add` sobre un `atomic_long`.
- `privado`: cada hilo incrementa su acumulador local y suma el resultado global una sola vez.

El programa prueba `P = 1, 2, 4, 8, 12, 24` hilos y conserva el mejor tiempo de cinco repeticiones. Las tres versiones correctas deben producir exactamente `P * K`.

## Compilación y ejecución

Ejecuta los comandos desde `PracticaT2`.

### Linux o WSL

```console
gcc -O2 -Wall -Wextra -Iinclude -o p2_ej3 src/ejercicio3/p2_ej3.c -pthread
./p2_ej3
```

### Windows con MinGW

```console
gcc -O2 -Wall -Wextra -Iinclude -o p2_ej3.exe src/ejercicio3/p2_ej3.c -lpthread
p2_ej3.exe
```

La opción `-O2` es intencionada. El acumulador local se declara `volatile` para impedir que el compilador sustituya el bucle por una asignación y falsee la comparación de tiempos.

## Qué analizar

Conserva la tabla completa, el sistema operativo, el compilador y las cinco repeticiones. Para las tres versiones correctas calcula, con `P = 24`, el coste por incremento:

$$
\text{coste} = \frac{\text{tiempo}}{24 \cdot 5.000.000}
$$

La conclusión esperada es que el mutex empeora al aumentar los hilos por la contención, el atómico evita el cerrojo explícito pero sigue coordinando accesos a memoria, y el acumulador privado escala mejor porque solo sincroniza una vez por hilo.