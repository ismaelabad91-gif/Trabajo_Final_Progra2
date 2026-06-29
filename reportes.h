#ifndef REPORTES_H
#define REPORTES_H

#include "estructuras.h"

void reporteTopCanciones(Artista* raizArtistas);
void reporteArtistasPreferidos(Usuario* usuario);
void reporteTiempoTotal(Usuario* usuario);
void reporteAnunciosFree(Usuario* usuario);
void generarReportes(Usuario* raizUsuarios, Artista* raizArtistas);
void mostrarUsuariosPremium(Usuario* raiz);

#endif