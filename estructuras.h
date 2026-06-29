#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct Usuario Usuario;

#define MAX_CORREO 80
#define MAX_NOMBRE 80
#define MAX_PAIS 50
#define MAX_NICKNAME 50
#define MAX_CONTRASENA 50
#define MAX_PLAN 15
#define MAX_FECHA 15
#define MAX_ORIGEN 150
#define MAX_ANUNCIO 150
#define MAX_ANUNCIANTE 80

typedef struct {
    int dia;
    int mes;
    int anio;
} Fecha;

typedef struct Cancion {
    char artista[MAX_NOMBRE];
    char nombre[MAX_NOMBRE];
    int duracionSegundos;
    char archivoOrigen[MAX_ORIGEN];
    int reproducciones;
    int enPlaylists;
    struct Cancion *sig;
} Cancion;

typedef struct Disco {
    char nombre[MAX_NOMBRE];
    char fechaLanzamiento[MAX_FECHA];
    Cancion *listaCanciones;
    struct Disco *sig;
} Disco;

typedef struct Artista {
    char nombre[MAX_NOMBRE];
    Disco *listaDiscos;
    struct Artista *izq;
    struct Artista *der;
} Artista;

typedef struct NodoCancionPlaylist {
    Cancion *cancion;
    struct NodoCancionPlaylist *sig;
} NodoCancionPlaylist;

typedef struct Playlist {
    char nombre[MAX_NOMBRE];
    NodoCancionPlaylist *canciones;
    struct Playlist *sig;
} Playlist;

typedef struct NodoHistorial {
    Cancion *cancion;
    char fecha[MAX_FECHA];
    struct NodoHistorial *sig;
} NodoHistorial;

typedef struct NodoAmigo {
    Usuario *usuarioAmigo;
    struct NodoAmigo *sig;
} NodoAmigo;

typedef struct Anuncio {
    char contenido[MAX_ANUNCIO];
    char anunciante[MAX_ANUNCIANTE];
    struct Anuncio *sig;
} Anuncio;

typedef struct ColaAnuncios {
    Anuncio *frente;
    Anuncio *final;
} ColaAnuncios;

struct Usuario {
    char correo[MAX_CORREO];
    char nombre[MAX_NOMBRE];
    char pais[MAX_PAIS];
    char nickname[MAX_NICKNAME];
    char contrasena[MAX_CONTRASENA];
    char plan[MAX_PLAN];
    int premiumActivo;
    char fechaVencimientoPremium[MAX_FECHA];
    float valorPremium;
    int cancionesEscuchadasDesdeAnuncio;
    int anunciosMostrados;
    int tiempoTotalReproduccion;
    Playlist *playlists;
    NodoHistorial *historial;
    NodoAmigo *amigos;
    struct Usuario *izq;
    struct Usuario *der;
};

#endif