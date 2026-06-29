#ifndef HISTORIAL_H
#define HISTORIAL_H

#include "estructuras.h"

NodoHistorial* crearNodoHistorial(Cancion* cancion, char fecha[]);
void pushHistorial(Usuario* usuario, Cancion* cancion, char fecha[]);
void registrarEnHistorial(Usuario* usuario, Cancion* cancion);
void mostrarHistorial(Usuario* usuario);
void liberarHistorial(NodoHistorial* historial);

#endif