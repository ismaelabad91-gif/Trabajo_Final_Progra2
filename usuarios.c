#include "usuarios.h"
#include "playlist.h"
#include "historial.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "colores.h"
#include "interfaz.h"

Usuario* crearUsuarioVacio(void) {
    Usuario* nuevo = (Usuario*)malloc(sizeof(Usuario));
    if (nuevo != NULL) {
        nuevo->izq = NULL;
        nuevo->der = NULL;
        nuevo->playlists = NULL;
        nuevo->historial = NULL;
        nuevo->amigos = NULL;
        nuevo->cancionesEscuchadasDesdeAnuncio = 0;
        nuevo->anunciosMostrados = 0;
        nuevo->tiempoTotalReproduccion = 0;
    }
    return nuevo;
}

void insertarUsuario(Usuario** raiz, Usuario* nuevo) {
    if (*raiz == NULL) {
        *raiz = nuevo;
        printf(VERDE "\n Usuario '%s' registrado exitosamente.\n" RESET, nuevo->nickname);
        return;
    }
    
    int cmp = strcmp(nuevo->nickname, (*raiz)->nickname);
    if (cmp < 0) {
        insertarUsuario(&(*raiz)->izq, nuevo);
    } else if (cmp > 0) {
        insertarUsuario(&(*raiz)->der, nuevo);
    } else {
        printf(ROJO "\n El usuario '%s' ya existe.\n" RESET, nuevo->nickname);
        free(nuevo);
    }
}

Usuario* buscarUsuario(Usuario* raiz, char nickname[]) {
    if (raiz == NULL) return NULL;
    
    int cmp = strcmp(nickname, raiz->nickname);
    if (cmp == 0) return raiz;
    if (cmp < 0) return buscarUsuario(raiz->izq, nickname);
    return buscarUsuario(raiz->der, nickname);
}

void mostrarUsuario(Usuario* usuario) {
    if (usuario == NULL) return;
    printf(GRIS " +------------------------------------------+ \n" RESET);
    printf(CYAN "\n Usuario: " RESET "%s\n", usuario->nickname);
    printf(CYAN "  Nombre:   " RESET "%s\n", usuario->nombre);
    printf(CYAN "   Pais: " RESET "%s\n", usuario->pais);
    printf(CYAN "   Plan: " RESET "%s\n", usuario->plan);
    printf(CYAN "   Premium: " RESET "%s\n", usuario->premiumActivo ? " Activo" : " Inactivo");
    printf(GRIS " +------------------------------------------+ \n" RESET);
}

void mostrarUsuarioDetallado(Usuario* usuario) {
    if (usuario == NULL) return;
    limpiarPantalla();
    dibujarEncabezado("PERFIL DEL USUARIO");
    mostrarUsuario(usuario);
    printf(CYAN"   Correo: " RESET "%s\n" , usuario->correo);
    printf(CYAN "   Tiempo reproduccion: " RESET "%d\n" , usuario->tiempoTotalReproduccion);
    printf(CYAN "   Anuncios mostrados: " RESET "%d\n" , usuario->anunciosMostrados);
}

void mostrarUsuarios(Usuario* raiz) {
    if (raiz == NULL) {
        return;
    }
    
    mostrarUsuarios(raiz->izq);
    printf("- %s (%s) - %s\n", raiz->nickname, raiz->nombre, raiz->plan);
    mostrarUsuarios(raiz->der);
}

Usuario* eliminarUsuario(Usuario* raiz, char nickname[]) {
    if (raiz == NULL) {
        printf(ROJO "\n Usuario no encontrado.\n" RESET);
        return NULL;
    }
    
    int cmp = strcmp(nickname, raiz->nickname);
    if (cmp < 0) {
        raiz->izq = eliminarUsuario(raiz->izq, nickname);
    } else if (cmp > 0) {
        raiz->der = eliminarUsuario(raiz->der, nickname);
    } else {
        if (raiz->izq == NULL) {
            Usuario* temp = raiz->der;
            free(raiz);
            printf(VERDE "\n Usuario eliminado.\n" RESET);
            return temp;
        } else if (raiz->der == NULL) {
            Usuario* temp = raiz->izq;
            free(raiz);
            printf(VERDE "\n Usuario eliminado.\n" RESET);
            return temp;
        }
        
        Usuario* temp = raiz->der;
        while (temp->izq != NULL) {
            temp = temp->izq;
        }
        
        strcpy(raiz->nickname, temp->nickname);
        strcpy(raiz->nombre, temp->nombre);
        strcpy(raiz->correo, temp->correo);
        strcpy(raiz->pais, temp->pais);
        strcpy(raiz->contrasena, temp->contrasena);
        strcpy(raiz->plan, temp->plan);
        raiz->premiumActivo = temp->premiumActivo;
        
        raiz->der = eliminarUsuario(raiz->der, temp->nickname);
        printf(VERDE "\n Usuario eliminado.\n" RESET);
    }
    return raiz;
}

void mostrarPlaylistsUsuario(Usuario* usuario) {
    if (usuario == NULL) return;
    mostrarTodasPlaylists(usuario->playlists);
}

void liberarUsuarios(Usuario* raiz) {
    if (raiz == NULL) return;
    
    liberarUsuarios(raiz->izq);
    liberarUsuarios(raiz->der);
    
    liberarPlaylists(raiz->playlists);
    liberarHistorial(raiz->historial);
    
    NodoAmigo* actualAmigo = raiz->amigos;
    while (actualAmigo != NULL) {
        NodoAmigo* temp = actualAmigo;
        actualAmigo = actualAmigo->sig;
        free(temp);
    }
    
    free(raiz);
}