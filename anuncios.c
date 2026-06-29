#include "anuncios.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "colores.h"

ColaAnuncios* crearColaAnuncios() {
    ColaAnuncios* cola = (ColaAnuncios*)malloc(sizeof(ColaAnuncios));
    if (cola != NULL) {
        cola->frente = NULL;
        cola->final = NULL;
    }
    return cola;
}

void encolarAnuncio(ColaAnuncios* cola, char contenido[], char anunciante[]) {
    if (cola == NULL) return;
    
    Anuncio* nuevo = (Anuncio*)malloc(sizeof(Anuncio));
    if (nuevo == NULL) return;
    
    strcpy(nuevo->contenido, contenido);
    strcpy(nuevo->anunciante, anunciante);
    nuevo->sig = NULL;

    if (cola->final == NULL) {
        cola->frente = nuevo;
        cola->final = nuevo;
    } else {
        cola->final->sig = nuevo;
        cola->final = nuevo;
    }
}

void desencolarAnuncio(ColaAnuncios* cola) {
    if (cola == NULL || cola->frente == NULL) {
        return;
    }
    
    Anuncio* temp = cola->frente;
    cola->frente = cola->frente->sig;
    
    if (cola->frente == NULL) {
        cola->final = NULL;
    }
    free(temp);
}

void mostrarAnuncios(ColaAnuncios* cola) {
    if (cola == NULL) {
        printf(AMARILLO "Cola no inicializada.\n" RESET);
        return;
    }
    
    Anuncio* actual = cola->frente;
    if (actual == NULL) {
        printf(AMARILLO "No hay anuncios en la cola.\n" RESET);
        return;
    }
    
    int contador = 1;
    while (actual != NULL) {
        printf(BLANCO "Anuncio #%d:"RESET " %s (Anunciante: %s)\n" RESET, contador++, actual->contenido, actual->anunciante);
        actual = actual->sig;
    }
}

void mostrarAnuncioActual(ColaAnuncios* cola) {
    if (cola != NULL && cola->frente != NULL) {
printf(MAGENTA "\n +==========================================+ \n");
        printf(MAGENTA " | " VERDE "ESPACIO PUBLICITARIO" MAGENTA "                 | \n");
        printf(MAGENTA " +==========================================+ \n" RESET);
        
        printf(BLANCO " > %s\n" RESET, cola->frente->contenido);
        printf(GRIS_O "   Patrocinado por: %s\n" RESET, cola->frente->anunciante);
        
        printf(MAGENTA " +==========================================+ \n\n" RESET);
        
        char contenidoTemp[MAX_ANUNCIO];
        char anuncianteTemp[MAX_ANUNCIANTE];
        strcpy(contenidoTemp, cola->frente->contenido);
        strcpy(anuncianteTemp, cola->frente->anunciante);
        
        desencolarAnuncio(cola);
        encolarAnuncio(cola, contenidoTemp, anuncianteTemp);
    }
}

void liberarCola(ColaAnuncios* cola) {
    if (cola == NULL) return;
    while (cola->frente != NULL) {
        desencolarAnuncio(cola);
    }
    free(cola);
}