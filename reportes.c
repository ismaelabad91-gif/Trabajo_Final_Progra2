#include "reportes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interfaz.h"
#include "colores.h"

typedef struct NodoConteo {
    char artista[MAX_NOMBRE];
    int cantidad;
    struct NodoConteo* sig;
} NodoConteo;

void actualizarTop5(Cancion* top[], Cancion* nueva) {
    int i;
    int j;
    for (i = 0; i < 5; i++) {
        if (top[i] == NULL || nueva->reproducciones > top[i]->reproducciones) {
            for (j = 4; j > i; j--) {
                top[j] = top[j-1];
            }
            top[i] = nueva;
            break;
        }
    }
}

void recorrerYBuscarTop(Artista* raiz, Cancion* top[]) {
    if (raiz == NULL) return;
    
    Disco* d = raiz->listaDiscos;
    while (d != NULL) {
        Cancion* c = d->listaCanciones;
        while (c != NULL) {
            actualizarTop5(top, c);
            c = c->sig;
        }
        d = d->sig;
    }
    
    recorrerYBuscarTop(raiz->izq, top);
    recorrerYBuscarTop(raiz->der, top);
}

void reporteTopCanciones(Artista* raizArtistas) {
    Cancion* top[5] = {NULL, NULL, NULL, NULL, NULL};
    int i;
    FILE* archivo;
    
    recorrerYBuscarTop(raizArtistas, top);
    
    limpiarPantalla();
    dibujarEncabezado("RANKING: TOP 5 CANCIONES");

    for (i = 0; i < 5; i++) {
        if (top[i] != NULL) {
            if (i == 0) printf(AMARILLO " [1] %-25s | %d reps\n" RESET, top[i]->nombre, top[i]->reproducciones);
            else if (i == 1) printf(BLANCO " [2] %-25s | %d reps\n" RESET, top[i]->nombre, top[i]->reproducciones);
            else if (i == 2) printf(GRIS_O " [3] %-25s | %d reps\n" RESET, top[i]->nombre, top[i]->reproducciones);
            else printf(CYAN " [%d] %-25s | %d reps\n" RESET, i+1, top[i]->nombre, top[i]->reproducciones);
        }
    }
    
    archivo = fopen("top_canciones.txt", "w");
    if (archivo != NULL) {
        fprintf(archivo, "--- Top 5 Canciones Mas Escuchadas ---\n");
        for (i = 0; i < 5; i++) {
            if (top[i] != NULL) {
                fprintf(archivo, "%d. %s - %s (%d reproducciones)\n", i+1, top[i]->nombre, top[i]->artista, top[i]->reproducciones);
            }
        }
        fclose(archivo);
        printf(VERDE "\n [OK] Reporte exportado a 'top_canciones.txt'.\n" RESET);
    }
}

void reporteArtistasPreferidos(Usuario* usuario) {
    FILE* archivo;
    NodoConteo* listaConteo = NULL;
    NodoHistorial* h;
    NodoConteo* maxNodo;
    NodoConteo* temp2;
    
    printf("\n--- Artista Preferido ---\n");
    
    if (usuario == NULL || usuario->historial == NULL) {
        printf("El usuario no tiene historial.\n");
        return;
    }
    
    h = usuario->historial;
    while (h != NULL) {
        if (h->cancion != NULL) {
            NodoConteo* temp = listaConteo;
            int encontrado = 0;
            while (temp != NULL) {
                if (strcmp(temp->artista, h->cancion->artista) == 0) {
                    temp->cantidad++;
                    encontrado = 1;
                    break;
                }
                temp = temp->sig;
            }
            if (encontrado == 0) {
                NodoConteo* nuevo = (NodoConteo*)malloc(sizeof(NodoConteo));
                strcpy(nuevo->artista, h->cancion->artista);
                nuevo->cantidad = 1;
                nuevo->sig = listaConteo;
                listaConteo = nuevo;
            }
        }
        h = h->sig;
    }
    
    maxNodo = listaConteo;
    temp2 = listaConteo;
    while (temp2 != NULL) {
        if (temp2->cantidad > maxNodo->cantidad) {
            maxNodo = temp2;
        }
        temp2 = temp2->sig;
    }
    
    printf("Usuario: %s\n", usuario->nickname);
    printf("Artista mas escuchado: %s (%d veces)\n", maxNodo->artista, maxNodo->cantidad);
    
    archivo = fopen("artistas_preferidos.txt", "w");
    if (archivo != NULL) {
        fprintf(archivo, "--- Artista Preferido ---\n");
        fprintf(archivo, "Usuario: %s\n", usuario->nickname);
        fprintf(archivo, "Artista mas escuchado: %s (%d veces)\n", maxNodo->artista, maxNodo->cantidad);
        fclose(archivo);
        printf("\nReporte guardado en artistas_preferidos.txt exitosamente.\n");
    }
    
    temp2 = listaConteo;
    while (temp2 != NULL) {
        NodoConteo* borrar = temp2;
        temp2 = temp2->sig;
        free(borrar);
    }
}

void reporteTiempoTotal(Usuario* usuario) {
    FILE* archivo = fopen("tiempo_total.txt", "w");
    if (archivo == NULL) return;

    fprintf(archivo, "--- Reporte de Tiempo Total de Reproduccion ---\n");
    if (usuario != NULL) {
        fprintf(archivo, "Usuario: %s (%s)\n", usuario->nombre, usuario->nickname);
        fprintf(archivo, "Tiempo total: %d segundos\n", usuario->tiempoTotalReproduccion);
        int minutos = usuario->tiempoTotalReproduccion / 60;
        int segundos = usuario->tiempoTotalReproduccion % 60;
        fprintf(archivo, "Equivalente a: %d minutos y %d segundos\n", minutos, segundos);
    } else {
        fprintf(archivo, "No hay usuario seleccionado.\n");
    }

    fclose(archivo);
    printf(" Reporte de tiempo total generado.\n");
}

void reporteAnunciosFree(Usuario* usuario) {
    FILE* archivo = fopen("reporte_anuncios.txt", "w");
    if (archivo == NULL) return;

    fprintf(archivo, "--- Reporte de Anuncios para Usuarios Free ---\n");
    if (usuario != NULL) {
        fprintf(archivo, "Usuario: %s (%s)\n", usuario->nombre, usuario->nickname);
        fprintf(archivo, "Plan: %s\n", usuario->plan);
        fprintf(archivo, "Anuncios mostrados: %d\n", usuario->anunciosMostrados);
    } else {
        fprintf(archivo, "No hay usuario seleccionado.\n");
    }

    fclose(archivo);
    printf(" Reporte de anuncios generado.\n");
}

void generarReportes(Usuario* raizUsuarios, Artista* raizArtistas) {
    printf("\n=== GENERANDO TODOS LOS REPORTES ===\n");
    reporteTopCanciones(raizArtistas);
    printf(" Todos los reportes generados exitosamente.\n");
}

void mostrarUsuariosPremium(Usuario* raiz) {
    if (raiz == NULL) return;
    mostrarUsuariosPremium(raiz->izq);
    if (raiz->premiumActivo) {
        printf("- %s (%s) - Vence: %s\n", raiz->nickname, raiz->nombre, raiz->fechaVencimientoPremium);
    }
    mostrarUsuariosPremium(raiz->der);
}