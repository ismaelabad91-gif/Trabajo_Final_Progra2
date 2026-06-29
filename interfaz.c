#include <stdio.h>
#include <stdlib.h>
#include "colores.h"
#include "interfaz.h"

void dibujarEncabezado(char* titulo) {
    printf(VERDE "\n +------------------------------------------+ \n");
    printf(VERDE " | %-40s | \n", titulo);
    printf(VERDE " +------------------------------------------+ \n" RESET);
}

void limpiarPantalla() {
    system("cls");
}

void dibujarEngranaje() {
    printf(CYAN "      .---. \n");
    printf("     /     \\ \n");
    printf("    | () () |  " VERDE "CONFIGURACION\n");
    printf("     \\  ^  /   " VERDE "DEL SISTEMA\n");
    printf(CYAN "      ||||| \n" RESET);
}

void dibujarUsuarioIcono() {
    printf(CYAN "      .---. \n");
    printf("     /     \\ \n");
    printf("    |  o o  |   " VERDE "ADMINISTRACION\n");
    printf("     \\  -  /    " VERDE "DE USUARIOS\n");
    printf(CYAN "      '---' \n" RESET);
}

void dibujarMusicaIcono() {
    printf(AMARILLO "      .---. \n");
    printf("     /     \\ \n");
    printf("    |  ♪ ♪  |   " VERDE "BIBLIOTECA\n");
    printf("     \\  -  /    " VERDE "MUSICAL\n");
    printf(AMARILLO "      '---' \n" RESET);
}