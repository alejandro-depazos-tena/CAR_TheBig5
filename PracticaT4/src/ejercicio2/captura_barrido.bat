@echo off
rem Muestra la busqueda del umbral (resultados/ejercicio2/umbral.txt); la ventana se queda abierta.
title Practica 4 - Ejercicio 2 - umbral
wsl.exe --cd "%~dp0..\.." bash -c "bash src/ejercicio2/captura_ej2.sh barrido; exec bash"
