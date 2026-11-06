# Medidas anteriores de los ejercicios 2 a 6 (Intel Core Ultra 5 225U, 2026-10-15)

Estos ficheros son las salidas que había en `resultados/ejercicio2` a
`resultados/ejercicio6` antes de completar el laboratorio. Se conservan como
referencia y **no se mezclan** con las medidas actuales, por tres motivos:

- Se midieron en otra máquina (Intel Core Ultra 5 225U, 14 hilos lógicos; el
  entorno está en `../entorno.txt`), con la versión anterior del código
  (commit `d33b2f0`).
- Faltaba la malla pequeña del Ejercicio 3 (`fuerte_256.csv` quedó vacío porque
  la ejecución se cortó), y comparar las dos curvas exige medirlas en la misma
  máquina y sesión.
- No había registro literal de las salidas de consola.

Las medidas válidas de los ejercicios 2 a 6 son las de `resultados/ejercicioN/`,
obtenidas en una sola sesión con `scripts/ejecutar_ej2_en_adelante.sh`; su
registro completo está en `resultados/salidas_ejercicios_2_en_adelante.txt`.
