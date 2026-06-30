#ifndef USUARIOS_H
#define USUARIOS_H

#include "estructuras.h"

Usuario* crearUsuarioVacio(void);
void insertarUsuario(Usuario** raiz, Usuario* nuevo);
Usuario* buscarUsuario(Usuario* raiz, char nickname[]);
void mostrarUsuario(Usuario* usuario);
void mostrarUsuarioDetallado(Usuario* usuario);
void mostrarUsuarios(Usuario* raiz);
Usuario* eliminarUsuario(Usuario* raiz, char nickname[]);
void mostrarPlaylistsUsuario(Usuario* usuario);
void liberarUsuarios(Usuario* raiz);
void modificarUsuario(Usuario* usuario);
int eliminarUsuarioSistema(Usuario** raizUsuarios, char nickname[]);

#endif