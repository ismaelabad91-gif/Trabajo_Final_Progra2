#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <mmsystem.h>
#include "estructuras.h"
#include "menus.h"
#include "artistas.h"
#include "playlist.h"
#include "anuncios.h"
#include "usuarios.h"
#include "persistencia.h"
#include "colores.h"
#include "reportes.h"
#include "historial.h"
#include "colores.h"
#include "interfaz.h"

#define RESET "\x1b[0m"
#define VERDE "\x1b[32m"
#define GRIS "\x1b[90m"
#define BLANCO "\x1b[97m"

Artista *raizArtistas = NULL;
Playlist *playlists = NULL;
ColaAnuncios *colaAnuncios = NULL;
Usuario *raizUsuarios = NULL;

void mostrarLogoSpotify() {
    printf(VERDE "\n");
    printf("      ...:::...      \n");
    printf("   .:::::::::::::.   \n");
    printf(" .:::::::::::::::::. \n");
    printf(" ::::           :::: \n");
    printf(" :::::..     ..::::: \n");
    printf(" ::::::::   :::::::: \n");
    printf(" :::::         ::::: \n");
    printf(" :::::::.. ..::::::: \n");
    printf(" ::::::::: ::::::::: \n");
    printf(" ::::::       :::::: \n");
    printf("  :::::.......:::::  \n");
    printf("    :::::::::::::    \n");
    printf("      ...:::...      \n" RESET);
    printf(BLANCO "\n      S P O T I F Y  \n\n" RESET);
}

int main() {
    int opcion;
    Usuario* nuevoUsuario = NULL;

    limpiarPantalla();
    mostrarLogoSpotify();
    
    printf(GRIS "Inicializando sistema...\n" RESET);
    cargarDatos(&raizUsuarios, &raizArtistas);
    
    colaAnuncios = crearColaAnuncios();
    encolarAnuncio(colaAnuncios, "Compra Premium por $9.99 y dile adios a las pausas.", "Spotify");
    encolarAnuncio(colaAnuncios, "Disfruta de una bebida bien fria escuchando tu musica.", "Marca de Bebidas");
    encolarAnuncio(colaAnuncios, "Aprende a programar en C con los nuevos cursos.", "Universidad");
    
    printf(VERDE "Sistema inicializado correctamente.\n" RESET);
    
    pausa();
    limpiarPantalla();

    do {
        mostrarLogoSpotify();
        opcion = menuPrincipal();

        switch(opcion) {
            case 1:
                menuIngresar();
                break;
            case 2:
                nuevoUsuario = registrarNuevoUsuario();
                if (nuevoUsuario != NULL) {
                    insertarUsuario(&raizUsuarios, nuevoUsuario);
                    printf(VERDE "\nCuenta creada exitosamente. Ya puedes iniciar sesion.\n" RESET);
                    pausa();
                }
                break;
            case 3:
                menuDesarrollador();
                break;
            case 4:
                printf(VERDE "\nGracias por utilizar Spotify.\n" RESET);
                break;
            default:
                printf(GRIS "\nOpcion invalida.\n" RESET);
                pausa();
        }

        if(opcion != 4) {
            limpiarPantalla();
        }

    } while(opcion != 4);

    guardarDatos(raizUsuarios, raizArtistas);

    printf(GRIS "\nLiberando memoria...\n" RESET);
    liberarArtistas(raizArtistas);
    liberarPlaylists(playlists);
    liberarCola(colaAnuncios);
    liberarUsuarios(raizUsuarios);

    printf(VERDE "Programa finalizado correctamente.\n" RESET);
    
    return 0;
}