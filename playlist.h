#ifndef PLAYLIST_H
#define PLAYLIST_H

#include "estructuras.h"

Playlist* crearPlaylist(char nombre[]);
void agregarCancionAPlaylist(Playlist* playlist, Cancion* cancion);
void mostrarPlaylist(Playlist* playlist);
void mostrarTodasPlaylists(Playlist* playlist);
int cancionEstaEnPlaylist(Playlist* playlists, char nombreCancion[], char artista[]);
void liberarPlaylists(Playlist* playlist);
Playlist* eliminarPlaylist(Playlist* playlists, char nombre[]);

#endif