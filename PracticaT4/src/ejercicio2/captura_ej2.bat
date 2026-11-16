@echo off
rem Abre WSL en la carpeta PracticaT4 y lanza captura_ej2.sh; la ventana se queda abierta.
title Practica 4 - Ejercicio 2
wsl.exe --cd "%~dp0..\.." bash -c "bash src/ejercicio2/captura_ej2.sh; exec bash"
