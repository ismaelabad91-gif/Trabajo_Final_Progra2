#include "historial.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "colores.h"

NodoHistorial* crearNodoHistorial(Cancion* cancion, char fecha[]) {
    NodoHistorial* nuevo = (NodoHistorial*)malloc(sizeof(NodoHistorial));
    if (nuevo != NULL) {
        nuevo->cancion = cancion;
        strcpy(nuevo->fecha, fecha);
        nuevo->sig = NULL;
    }
    return nuevo;
}

void pushHistorial(Usuario* usuario, Cancion* cancion, char fecha[]) {
    if (usuario == NULL || cancion == NULL) return;
    
    NodoHistorial* nuevo = crearNodoHistorial(cancion, fecha);
    if (nuevo != NULL) {
        nuevo->sig = usuario->historial;
        usuario->historial = nuevo;
    }
}

void registrarEnHistorial(Usuario* usuario, Cancion* cancion) {
    char fecha[MAX_FECHA];
    time_t t = time(NULL);
    struct tm* tm = localtime(&t);
    strftime(fecha, MAX_FECHA, "%Y-%m-%d", tm);
    pushHistorial(usuario, cancion, fecha);
}

void mostrarHistorial(Usuario* usuario) {
    if (usuario == NULL) return;
    
    NodoHistorial* actual = usuario->historial;
    if (actual == NULL) {
        printf(AMARILLO "No hay historial.\n" RESET);
        return;
    }
    
    int pos = 1;
    while (actual != NULL) {
        printf(GRIS "%d. " BLANCO "%s  - " AMARILLO "%s\n" RESET, pos++, actual->cancion->nombre, actual->cancion->artista);
        actual = actual->sig;
    }
}

void liberarHistorial(NodoHistorial* historial) {
    while (historial != NULL) {
        NodoHistorial* temp = historial;
        historial = historial->sig;
        free(temp);
    }
}