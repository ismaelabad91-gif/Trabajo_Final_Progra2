#ifndef MENUS_H
#define MENUS_H

#include "estructuras.h" 

extern Artista* raizArtistas;
extern Playlist* playlists;
extern ColaAnuncios* colaAnuncios;
extern Usuario* raizUsuarios;

int menuPrincipal(void);
void menuIngresar(void);
void menuDesarrollador(void);
void menuUsuarios(void);
void menuArtistas(void);
void menuPlaylists(void);
void menuAnuncios(void);
void menuReportes(void);
void mostrarEstadisticas(void);

void limpiarBuffer(void);
void pausa(void);
void inicializarDatosPrueba(void);
Usuario* registrarNuevoUsuario(void);
void reproducirMusica(Usuario* usuario);
void comprarPremium(Usuario* usuario);

#endif