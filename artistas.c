#include "artistas.h"
#include "playlist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "colores.h"

extern Playlist* playlists;

Artista* crearArtista(char nombre[]) {
    Artista* nuevo = (Artista*)malloc(sizeof(Artista));
    if (nuevo == NULL) {
        printf(ROJO "Error: No se pudo crear el artista.\n" RESET);
        return NULL;
    }
    
    strcpy(nuevo->nombre, nombre);
    nuevo->listaDiscos = NULL;
    nuevo->izq = NULL;
    nuevo->der = NULL;
    
    return nuevo;
}

void insertarArtista(Artista** raiz, Artista* nuevo) {
    if (*raiz == NULL) {
        *raiz = nuevo;
        printf(VERDE "Artista '%s' agregado exitosamente.\n" RESET, nuevo->nombre);
        return;
    }
    
    int cmp = strcmp(nuevo->nombre, (*raiz)->nombre);
    if (cmp < 0) {
        insertarArtista(&(*raiz)->izq, nuevo);
    } else if (cmp > 0) {
        insertarArtista(&(*raiz)->der, nuevo);
    } else {
        printf(ROJO "El artista '%s' ya existe.\n" RESET, nuevo->nombre);
        free(nuevo);
    }
}

Artista* buscarArtista(Artista* raiz, char nombre[]) {
    if (raiz == NULL) {
        return NULL;
    }
    
    int cmp = strcmp(nombre, raiz->nombre);
    if (cmp == 0) {
        return raiz;
    } else if (cmp < 0) {
        return buscarArtista(raiz->izq, nombre);
    } else {
        return buscarArtista(raiz->der, nombre);
    }
}

int existeArtista(Artista* raiz, char nombre[]) {
    return buscarArtista(raiz, nombre) != NULL;
}

void mostrarArtistas(Artista* raiz) {
    if (raiz == NULL) {
        return;
    }
    
    mostrarArtistas(raiz->izq);
    printf("- %s\n", raiz->nombre);
    mostrarArtistas(raiz->der);
}

void mostrarArtistasConDetalles(Artista* raiz) {
    if (raiz == NULL) {
        return;
    }
    
    mostrarArtistasConDetalles(raiz->izq);
    
    printf(MAGENTA "\n--- Artista: %s ---\n" RESET, raiz->nombre);
    mostrarTodosDiscos(raiz);
    
    mostrarArtistasConDetalles(raiz->der);
}

Artista* encontrarMinimo(Artista* raiz) {
    Artista* actual = raiz;
    while (actual && actual->izq != NULL) {
        actual = actual->izq;
    }
    return actual;
}

Artista* eliminarArtista(Artista* raiz, char nombre[]) {
    if (raiz == NULL) {
        printf(ROJO "Artista no encontrado.\n" RESET);
        return NULL;
    }
    
    int cmp = strcmp(nombre, raiz->nombre);
    if (cmp < 0) {
        raiz->izq = eliminarArtista(raiz->izq, nombre);
    } else if (cmp > 0) {
        raiz->der = eliminarArtista(raiz->der, nombre);
    } else {
        if (raiz->listaDiscos != NULL) {
            printf(ROJO "No se puede eliminar el artista '%s' porque tiene discos asociados.\n" RESET, raiz->nombre);
            return raiz;
        }
        
        if (raiz->izq == NULL) {
            Artista* temp = raiz->der;
            free(raiz);
            printf(VERDE "Artista eliminado.\n" RESET);
            return temp;
        } else if (raiz->der == NULL) {
            Artista* temp = raiz->izq;
            free(raiz);
            printf(VERDE "Artista eliminado.\n" RESET);
            return temp;
        }
        
        Artista* temp = encontrarMinimo(raiz->der);
        strcpy(raiz->nombre, temp->nombre);
        raiz->listaDiscos = temp->listaDiscos;
        temp->listaDiscos = NULL;
        raiz->der = eliminarArtista(raiz->der, temp->nombre);
        printf(VERDE "Artista eliminado.\n" RESET);
    }
    
    return raiz;
}

void liberarArtistas(Artista* raiz) {
    if (raiz == NULL) {
        return;
    }
    
    liberarArtistas(raiz->izq);
    liberarArtistas(raiz->der);
    
    Disco* discoActual = raiz->listaDiscos;
    while (discoActual != NULL) {
        Disco* temp = discoActual;
        discoActual = discoActual->sig;
        
        Cancion* cancionActual = temp->listaCanciones;
        while (cancionActual != NULL) {
            Cancion* tempCancion = cancionActual;
            cancionActual = cancionActual->sig;
            free(tempCancion);
        }
        free(temp);
    }
    
    free(raiz);
}

Disco* crearDisco(char nombre[], char fechaLanzamiento[]) {
    Disco* nuevo = (Disco*)malloc(sizeof(Disco));
    if (nuevo == NULL) {
        printf(ROJO "Error: No se pudo crear el disco.\n" RESET);
        return NULL;
    }
    
    strcpy(nuevo->nombre, nombre);
    strcpy(nuevo->fechaLanzamiento, fechaLanzamiento);
    nuevo->listaCanciones = NULL;
    nuevo->sig = NULL;
    return nuevo;
}

void agregarDisco(Artista* artista, Disco* nuevo) {
    if (artista == NULL || nuevo == NULL) {
        printf(ROJO "Error: Artista o disco invalido.\n" RESET);
        return;
    }
    
    Disco* actual = artista->listaDiscos;
    while (actual != NULL) {
        if (strcmp(actual->nombre, nuevo->nombre) == 0) {
            printf(ROJO "El disco '%s' ya existe para este artista.\n" RESET, nuevo->nombre);
            free(nuevo);
            return;
        }
        actual = actual->sig;
    }
    
    if (artista->listaDiscos == NULL) {
        artista->listaDiscos = nuevo;
    } else {
        actual = artista->listaDiscos;
        while (actual->sig != NULL) {
            actual = actual->sig;
        }
        actual->sig = nuevo;
    }
    
    printf(VERDE "Disco '%s' agregado al artista '%s'.\n" RESET, nuevo->nombre, artista->nombre);
}

Disco* buscarDisco(Artista* artista, char nombre[]) {
    if (artista == NULL) {
        return NULL;
    }
    
    Disco* actual = artista->listaDiscos;
    while (actual != NULL) {
        if (strcmp(actual->nombre, nombre) == 0) {
            return actual;
        }
        actual = actual->sig;
    }
    return NULL;
}

void mostrarTodosDiscos(Artista* artista) {
    if (artista == NULL) {
        return;
    }
    
    Disco* actual = artista->listaDiscos;
    if (actual == NULL) {
        printf(AMARILLO "  No tiene discos asociados.\n" RESET);
        return;
    }
    
    printf(AMARILLO "  Discos:\n" RESET);
    while (actual != NULL) {
        printf(AMARILLO "    - %s (Lanzamiento: %s)\n" RESET, actual->nombre, actual->fechaLanzamiento);
        mostrarCanciones(actual);
        actual = actual->sig;
    }
}

Cancion* crearCancion(char artista[], char nombre[], int duracionSegundos, char archivoOrigen[]) {
    Cancion* nueva = (Cancion*)malloc(sizeof(Cancion));
    if (nueva == NULL) {
        printf(ROJO "Error: No se pudo crear la cancion.\n" RESET);
        return NULL;
    }
    
    strcpy(nueva->artista, artista);
    strcpy(nueva->nombre, nombre);
    nueva->duracionSegundos = duracionSegundos;
    if (strlen(archivoOrigen) == 0 || strcmp(archivoOrigen, "mp3") == 0) {
        strcpy(nueva->archivoOrigen, "sonido.mp3");
    } else {
        strcpy(nueva->archivoOrigen, archivoOrigen);
    }
    nueva->reproducciones = 0;
    nueva->enPlaylists = 0;
    nueva->sig = NULL;
    
    return nueva;
}

void agregarCancion(Disco* disco, Cancion* nueva) {
    if (disco == NULL || nueva == NULL) {
        printf(ROJO "Error: Disco o cancion invalida.\n" RESET);
        return;
    }
    
    Cancion* actual = disco->listaCanciones;
    while (actual != NULL) {
        if (strcmp(actual->nombre, nueva->nombre) == 0) {
            printf(ROJO "La cancion '%s' ya existe en este disco.\n" RESET, nueva->nombre);
            free(nueva);
            return;
        }
        actual = actual->sig;
    }
    
    if (disco->listaCanciones == NULL) {
        disco->listaCanciones = nueva;
    } else {
        actual = disco->listaCanciones;
        while (actual->sig != NULL) {
            actual = actual->sig;
        }
        actual->sig = nueva;
    }
    
    printf(VERDE "Cancion '%s' agregada al disco '%s'.\n" RESET, nueva->nombre, disco->nombre);
}

Cancion* buscarCancion(Disco* disco, char nombre[]) {
    if (disco == NULL) {
        return NULL;
    }
    
    Cancion* actual = disco->listaCanciones;
    while (actual != NULL) {
        if (strcmp(actual->nombre, nombre) == 0) {
            return actual;
        }
        actual = actual->sig;
    }
    return NULL;
}

void mostrarCanciones(Disco* disco) {
    if (disco == NULL) {
        return;
    }
    
    Cancion* actual = disco->listaCanciones;
    if (actual == NULL) {
        printf(ROJO "      No tiene canciones.\n" RESET);
        return;
    }
    
    printf(CYAN "      Canciones:\n" RESET);
    int contador = 1;
    while (actual != NULL) {
        printf("        %d. %s (Duracion: %d seg)\n", contador++, actual->nombre, actual->duracionSegundos);
        actual = actual->sig;
    }
}

int eliminarCancionDeDisco(Disco* disco, char nombreCancion[], Playlist* playlists) {
    if (disco == NULL || disco->listaCanciones == NULL) {
        printf(ROJO "Disco o lista de canciones vacia.\n" RESET);
        return 0;
    }
    
    if (cancionEstaEnPlaylist(playlists, nombreCancion, "")) {
        printf(ROJO "No se puede eliminar la cancion porque esta en una playlist.\n" RESET);
        return 0;
    }
    
    Cancion* actual = disco->listaCanciones;
    Cancion* anterior = NULL;
    while (actual != NULL) {
        if (strcmp(actual->nombre, nombreCancion) == 0) {
            if (anterior == NULL) {
                disco->listaCanciones = actual->sig;
            } else {
                anterior->sig = actual->sig;
            }
            free(actual);
            printf(VERDE "Cancion '%s' eliminada exitosamente.\n" RESET, nombreCancion);
            return 1;
        }
        anterior = actual;
        actual = actual->sig;
    }
    
    printf(ROJO "Cancion '%s' no encontrada en el disco.\n" RESET, nombreCancion);
    return 0;
}

void eliminarCancionDeSistema(Artista* raiz, char nombreArtista[], char nombreCancion[]) {
    Artista* artista = buscarArtista(raiz, nombreArtista);
    if (artista != NULL) {
        Disco* disco = artista->listaDiscos;
        while (disco != NULL) {
            if (eliminarCancionDeDisco(disco, nombreCancion, playlists)) {
                return;
            }
            disco = disco->sig;
        }
    }
    printf(ROJO "Cancion no encontrada en el sistema.\n" RESET);
}

void liberarCanciones(Cancion* cancion) {
    while (cancion != NULL) {
        Cancion* temp = cancion;
        cancion = cancion->sig;
        free(temp);
    }
}

void mostrarTodosArtistasYDiscos(Artista* raiz) {
    if (raiz == NULL) {
        printf(AMARILLO "\nNo hay artistas registrados.\n" RESET);
        return;
    }
    
    printf(MAGENTA "\n=== LISTA DE ARTISTAS ===\n" RESET);
    mostrarArtistasConDetalles(raiz);
}

int agregarCancionSistema(Artista** raiz, char nombreArtista[], char nombreDisco[], char nombreCancion[], int duracion, char archivoOrigen[]) {
    Artista* artista = buscarArtista(*raiz, nombreArtista);
    if (artista == NULL) {
        printf(ROJO "Artista '%s' no encontrado.\n" RESET, nombreArtista);
        return 0;
    }
    
    Disco* disco = buscarDisco(artista, nombreDisco);
    if (disco == NULL) {
        printf(ROJO "Disco '%s' no encontrado para el artista '%s'.\n" RESET, nombreDisco, nombreArtista);
        return 0;
    }
    
    Cancion* nueva = crearCancion(nombreArtista, nombreCancion, duracion, archivoOrigen);
    if (nueva == NULL) {
        return 0;
    }
    
    agregarCancion(disco, nueva);
    return 1;
}

Cancion* buscarCancionSistema(Artista* raiz, char nombreArtista[], char nombreCancion[]) {
    Artista* artista = buscarArtista(raiz, nombreArtista);
    if (artista == NULL) {
        return NULL;
    }
    
    Disco* disco = artista->listaDiscos;
    while (disco != NULL) {
        Cancion* cancion = buscarCancion(disco, nombreCancion);
        if (cancion != NULL) {
            return cancion;
        }
        disco = disco->sig;
    }
    return NULL;
}

void mostrarTopCanciones(Artista* raiz) {
    printf(AMARILLO "Reporte de top canciones en desarrollo...\n" RESET);
}

void mostrarCancionesSistemaRec(Artista* raiz, int* contador) {
    if (raiz == NULL) {
        return;
    }

    mostrarCancionesSistemaRec(raiz->izq, contador);

    Disco* disco = raiz->listaDiscos;
    while (disco != NULL) {
        Cancion* cancion = disco->listaCanciones;

        while (cancion != NULL) {
            printf(AMARILLO " %d. " RESET "%s " GRIS "- %s " CYAN "(%d seg)\n" RESET,
                   *contador,
                   cancion->nombre,
                   cancion->artista,
                   cancion->duracionSegundos);

            printf(GRIS "     Archivo: %s\n" RESET, cancion->archivoOrigen);

            (*contador)++;
            cancion = cancion->sig;
        }

        disco = disco->sig;
    }

    mostrarCancionesSistemaRec(raiz->der, contador);
}

void mostrarCancionesSistema(Artista* raiz) {
    int contador = 1;

    if (raiz == NULL) {
        printf(ROJO "\nNo hay canciones registradas en el sistema.\n" RESET);
        return;
    }

    printf(MAGENTA "\n========== CANCIONES DISPONIBLES ==========\n" RESET);
    mostrarCancionesSistemaRec(raiz, &contador);

    if (contador == 1) {
        printf(ROJO "\nNo hay canciones registradas en el sistema.\n" RESET);
    }
}

Cancion* seleccionarCancionPorNumeroRec(Artista* raiz, int objetivo, int* contador) {
    if (raiz == NULL) {
        return NULL;
    }

    Cancion* encontrada = seleccionarCancionPorNumeroRec(raiz->izq, objetivo, contador);
    if (encontrada != NULL) {
        return encontrada;
    }

    Disco* disco = raiz->listaDiscos;
    while (disco != NULL) {
        Cancion* cancion = disco->listaCanciones;

        while (cancion != NULL) {
            if (*contador == objetivo) {
                return cancion;
            }

            (*contador)++;
            cancion = cancion->sig;
        }

        disco = disco->sig;
    }

    return seleccionarCancionPorNumeroRec(raiz->der, objetivo, contador);
}

Cancion* seleccionarCancionPorNumero(Artista* raiz, int objetivo) {
    int contador = 1;
    return seleccionarCancionPorNumeroRec(raiz, objetivo, &contador);
}