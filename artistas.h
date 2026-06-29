#ifndef ARTISTAS_H
#define ARTISTAS_H

#include "estructuras.h"

Artista* crearArtista(char nombre[]);
void insertarArtista(Artista** raiz, Artista* nuevo);
Artista* buscarArtista(Artista* raiz, char nombre[]);
void mostrarArtistas(Artista* raiz);
void mostrarArtistasConDetalles(Artista* raiz);
Artista* eliminarArtista(Artista* raiz, char nombre[]);
void liberarArtistas(Artista* raiz);

Disco* crearDisco(char nombre[], char fechaLanzamiento[]);
void agregarDisco(Artista* artista, Disco* nuevo);
Disco* buscarDisco(Artista* artista, char nombre[]);
void mostrarTodosDiscos(Artista* artista);

Cancion* crearCancion(char artista[], char nombre[], int duracionSegundos, char archivoOrigen[]);
void agregarCancion(Disco* disco, Cancion* nueva);
Cancion* buscarCancion(Disco* disco, char nombre[]);
void mostrarCanciones(Disco* disco);
int eliminarCancionDeDisco(Disco* disco, char nombreCancion[], Playlist* playlists);
void eliminarCancionDeSistema(Artista* raiz, char nombreArtista[], char nombreCancion[]);
void liberarCanciones(Cancion* cancion);

void mostrarTodosArtistasYDiscos(Artista* raiz);
int existeArtista(Artista* raiz, char nombre[]);
Cancion* buscarCancionSistema(Artista* raiz, char nombreArtista[], char nombreCancion[]);
int agregarCancionSistema(Artista** raiz, char nombreArtista[], char nombreDisco[], char nombreCancion[], int duracion, char archivoOrigen[]);
void mostrarTopCanciones(Artista* raiz);

#endif