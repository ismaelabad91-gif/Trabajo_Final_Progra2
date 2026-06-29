#include "playlist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "colores.h"

Playlist* crearPlaylist(char nombre[]) {
    Playlist* nueva = (Playlist*)malloc(sizeof(Playlist));
    if (nueva == NULL) return NULL;
    
    strcpy(nueva->nombre, nombre);
    nueva->canciones = NULL;
    nueva->sig = NULL;
    return nueva;
}

void agregarCancionAPlaylist(Playlist* playlist, Cancion* cancion) {
    if (playlist == NULL || cancion == NULL) return;
    
    NodoCancionPlaylist* nuevoNodo = (NodoCancionPlaylist*)malloc(sizeof(NodoCancionPlaylist));
    if (nuevoNodo == NULL) return;
    
    nuevoNodo->cancion = cancion;
    nuevoNodo->sig = NULL;
    
    cancion->enPlaylists++;
    
    if (playlist->canciones == NULL) {
        playlist->canciones = nuevoNodo;
    } else {
        NodoCancionPlaylist* actual = playlist->canciones;
        while (actual->sig != NULL) {
            actual = actual->sig;
        }
        actual->sig = nuevoNodo;
    }
}

void mostrarPlaylist(Playlist* playlist) {
    if (playlist == NULL) {
        printf(ROJO "Playlist no existe.\n" RESET);
        return;
    }
    
    printf(VERDE "\n=== Playlist: " RESET "%s ===\n" RESET, playlist->nombre);
    
    NodoCancionPlaylist* actual = playlist->canciones;
    if (actual == NULL) {
        printf(ROJO "Playlist vacia.\n" RESET);
        return;
    }
    
    int contador = 1;
    while (actual != NULL) {
        Cancion* cancion = actual->cancion;
        printf(AMARILLO "%d. %s " RESET "- %s (%d seg)\n", contador++, cancion->nombre, cancion->artista, cancion->duracionSegundos);
        actual = actual->sig;
    }
}

void liberarPlaylists(Playlist* playlist) {
    while (playlist != NULL) {
        Playlist* temp = playlist;
        playlist = playlist->sig;
        
        NodoCancionPlaylist* nodoActual = temp->canciones;
        while (nodoActual != NULL) {
            NodoCancionPlaylist* tempNodo = nodoActual;
            nodoActual = nodoActual->sig;
            free(tempNodo);
        }
        free(temp);
    }
}

void mostrarTodasPlaylists(Playlist* playlists) {
    if (playlists == NULL) {
        printf(AMARILLO "\n No hay playlists creadas.\n" RESET);
        return;
    }
    
    Playlist* actual = playlists;
    int contador = 1;
    while (actual != NULL) {
        printf(AMARILLO "\n%d. " RESET, contador++);
        mostrarPlaylist(actual);
        actual = actual->sig;
    }
}

int cancionEstaEnPlaylist(Playlist* playlists, char nombreCancion[], char artista[]) {
    Playlist* actualPlaylist = playlists;
    while (actualPlaylist != NULL) {
        NodoCancionPlaylist* actualNodo = actualPlaylist->canciones;
        while (actualNodo != NULL) {
            Cancion* cancion = actualNodo->cancion;
            if (strcmp(cancion->nombre, nombreCancion) == 0) {
                if (strlen(artista) == 0 || strcmp(cancion->artista, artista) == 0) {
                    return 1;
                }
            }
            actualNodo = actualNodo->sig;
        }
        actualPlaylist = actualPlaylist->sig;
    }
    return 0;
}

Playlist* eliminarPlaylist(Playlist* playlists, char nombre[]) {
    if (playlists == NULL) {
        printf(ROJO "No hay playlists para eliminar.\n" RESET);
        return NULL;
    }

    Playlist* actual = playlists;
    Playlist* anterior = NULL;

    while (actual != NULL && strcmp(actual->nombre, nombre) != 0) {
        anterior = actual;
        actual = actual->sig;
    }

    if (actual == NULL) {
        printf(ROJO "Playlist no encontrada.\n" RESET);
        return playlists;
    }

    if (anterior == NULL) {
        playlists = actual->sig;
    } else {
        anterior->sig = actual->sig;
    }

    NodoCancionPlaylist* nodoActual = actual->canciones;
    while (nodoActual != NULL) {
        NodoCancionPlaylist* temp = nodoActual;
        nodoActual = nodoActual->sig;
        free(temp);
    }

    free(actual);
    printf(VERDE "Playlist eliminada.\n" RESET);
    return playlists;
}