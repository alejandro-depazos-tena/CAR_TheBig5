# Práctica 2 · Ejercicio 1

## Lo que cuesta nacer

Este documento acompaña a `src/ejercicio1/p2_ej1.c` y a los auxiliares indicados en el enunciado. El programa mide el coste medio por unidad de crear y esperar:

- 1000 hilos en tanda.
- 1000 hilos uno a uno.
- 200 procesos en tanda.
- 200 procesos uno a uno.

Cada medida se repite cinco veces y se muestran el mínimo y la media. El mínimo es el dato más representativo del coste del sistema cuando se reduce el ruido producido por el planificador, el antivirus y otras aplicaciones; la media se conserva para mostrar la variabilidad.

## Archivos

| Archivo | Función |
|---|---|
| `include/crono.h` | Reloj monotónico de alta resolución para Windows y Linux. |
| `src/ejercicio1/p2_ej1.c` | Medición de hilos y procesos en tanda y uno a uno. |
| `src/ejercicio1/nada.c` | Proceso mínimo que se lanza en Windows. |
| `src/ejercicio1/comprueba.c` | Verificación previa del reloj y de `pthread`. |

## Compilación y ejecución

Ejecuta los comandos desde `PracticaT2`.

### Linux o WSL

```console
gcc -O2 -Wall -Wextra -Iinclude -o comprueba src/ejercicio1/comprueba.c -pthread
./comprueba
gcc -O2 -Wall -Wextra -o p2_ej1 src/ejercicio1/p2_ej1.c -pthread
./p2_ej1
```

### Windows con MinGW

Compila primero el proceso auxiliar `nada.exe` en la carpeta desde la que vayas a ejecutar `p2_ej1.exe`:

```console
gcc -O2 -Wall -Wextra -Iinclude -o comprueba.exe src/ejercicio1/comprueba.c -lpthread
gcc -O2 -Wall -Wextra -o nada.exe src/ejercicio1/nada.c
gcc -O2 -Wall -Wextra -Iinclude -o p2_ej1.exe src/ejercicio1/p2_ej1.c -lpthread
p2_ej1.exe
```

En Linux no hace falta `nada.c`: el programa utiliza `fork()` y `waitpid()`.

## Tabla de resultados

Esta tabla contiene la salida de la ejecución realizada en el entorno del equipo. No deben copiarse las cifras de referencia de la guía: el resultado depende del sistema operativo, del compilador y de la carga de la máquina.

| Medida | Mínimo (µs) | Media (µs) | Repeticiones |
|---|---:|---:|---:|
| Hilo, en tanda | 97,2 | 100,1 | 5 |
| Hilo, uno a uno | 155,6 | 160,8 | 5 |
| Proceso, en tanda | 217,4 | 221,4 | 5 |
| Proceso, uno a uno | 587,9 | 618,2 | 5 |

Sistema operativo: `Ubuntu 24.04.4 LTS sobre WSL 2`  
Compilador y versión: `GCC 13.3.0`  
Procesador: `AMD Ryzen 7 5700U with Radeon Graphics`  
Resolución del reloj: `20 ns`

El comprobador previo produjo `4 de 4` hilos terminados, un tiempo de `0,632 ms` y el mensaje `ENTORNO LISTO`.

## Análisis

La creación de hilos en tanda mide principalmente el **caudal**, porque se crean varios hilos antes de esperar a cualquiera de ellos. La versión uno a uno mide la **latencia**, ya que cada creación debe completar antes de comenzar la siguiente.

En general, la versión uno a uno es más lenta por unidad porque no puede aprovechar el solapamiento entre creaciones. Los procesos también dependen mucho del sistema operativo: Windows crea el proceso mediante `CreateProcessA`, mientras que Linux utiliza `fork()`, que parte de una copia perezosa del proceso actual. Por ello no debe suponerse un factor fijo entre proceso e hilo.

El cociente que debes comentar con tus datos es:

$$
F_{tanda} = \frac{\text{proceso en tanda}}{\text{hilo en tanda}}, \qquad
F_{uno\ a\ uno} = \frac{\text{proceso uno a uno}}{\text{hilo uno a uno}}.
$$

### Conclusiones para completar

1. En mi sistema, un hilo cuesta aproximadamente `97,2 µs` en tanda y `155,6 µs` uno a uno. La versión uno a uno es aproximadamente un 60 % más lenta porque no puede solapar varias creaciones.
2. Un proceso cuesta aproximadamente `2,2` veces más que un hilo en tanda y `3,8` veces más uno a uno. El factor no es constante porque depende de la forma de creación y de la implementación del sistema operativo.
3. Crear procesos dentro de un bucle solo es razonable cuando el aislamiento del proceso compensa un coste medido de aproximadamente `0,59 ms` por proceso en este entorno; en otro caso conviene reutilizar trabajadores o usar hilos.

## Comprobación

Antes de medir, `comprueba` debe terminar con `ENTORNO LISTO`. La compilación debe producirse sin avisos con `-Wall -Wextra`. El resultado del ejercicio es válido únicamente si se conservan la salida, el número de repeticiones, el sistema operativo y el compilador utilizados.