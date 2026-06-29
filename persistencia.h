#ifndef PERSISTENCIA_H
#define PERSISTENCIA_H

#include "estructuras.h"

void guardarDatos(Usuario* raizUsuarios, Artista* raizArtistas);
void cargarDatos(Usuario** raizUsuarios, Artista** raizArtistas);

#endif