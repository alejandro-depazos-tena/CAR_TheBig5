# Medidas anteriores en otra máquina (Intel Core Ultra 5 225U, 2026-10-15)

Estos ficheros son las salidas que había en `resultados/` antes de volver a medir
todo el laboratorio en una sola máquina. Se conservan como referencia y **no se
mezclan** con las medidas actuales.

- `ejercicio1/`: tiempos, pruebas y gráfica del Ejercicio 1 generados por
  `scripts/ejecutar_todo.sh` el 2026-10-15 en esta máquina (commit `d33b2f0`).
- `ejercicio2/` a `ejercicio6/`: salidas de los ejercicios 2 a 6 de la misma
  ejecución, con la versión anterior del código. Faltaba la malla pequeña del
  Ejercicio 3 (`fuerte_256.csv` quedó vacío porque la ejecución se cortó) y no
  había registro literal de las salidas de consola.
- `entorno.txt` y `compilacion.txt`: entorno (Intel Core Ultra 5 225U, 14 hilos
  lógicos) y órdenes de compilación de esa ejecución.

Las medidas válidas son las de `resultados/ejercicio1/` a `resultados/ejercicio6/`,
hechas en un Intel Core i7-1355U el 2026-11-06:

- El Ejercicio 1 con `scripts/ejecutar_ej1.sh`; su entorno está en
  `resultados/entorno.txt`.
- Los ejercicios 2 a 6, en una sola sesión, con
  `scripts/ejecutar_ej2_en_adelante.sh`; su registro completo está en
  `resultados/salidas_ejercicios_2_en_adelante.txt`.
