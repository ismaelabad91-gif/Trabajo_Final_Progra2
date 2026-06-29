#ifndef ANUNCIOS_H
#define ANUNCIOS_H

#include "estructuras.h"

ColaAnuncios* crearColaAnuncios();
void encolarAnuncio(ColaAnuncios* cola, char contenido[], char anunciante[]);
void desencolarAnuncio(ColaAnuncios* cola);
void mostrarAnuncios(ColaAnuncios* cola);
void mostrarAnuncioActual(ColaAnuncios* cola);
void liberarCola(ColaAnuncios* cola);

#endif