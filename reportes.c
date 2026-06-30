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

void agregarConteoArtista(NodoConteo** lista, char artista[]) {
    NodoConteo* actual = *lista;

    while (actual != NULL) {
        if (strcmp(actual->artista, artista) == 0) {
            actual->cantidad++;
            return;
        }
        actual = actual->sig;
    }

    NodoConteo* nuevo = (NodoConteo*)malloc(sizeof(NodoConteo));

    if (nuevo == NULL) {
        return;
    }

    strcpy(nuevo->artista, artista);
    nuevo->cantidad = 1;
    nuevo->sig = *lista;
    *lista = nuevo;
}

void liberarConteoArtistas(NodoConteo* lista) {
    NodoConteo* temp;

    while (lista != NULL) {
        temp = lista;
        lista = lista->sig;
        free(temp);
    }
}

void escribirArtistaPreferidoUsuario(FILE* archivo, Usuario* usuario) {
    NodoHistorial* h;
    NodoConteo* listaConteo = NULL;
    NodoConteo* actual;
    NodoConteo* mayor;

    if (usuario == NULL) {
        return;
    }

    if (usuario->historial == NULL) {
        fprintf(archivo, "Usuario: %s (%s) -> Sin historial\n",
                usuario->nombre,
                usuario->nickname);
        return;
    }

    h = usuario->historial;

    while (h != NULL) {
        if (h->cancion != NULL) {
            agregarConteoArtista(&listaConteo, h->cancion->artista);
        }
        h = h->sig;
    }

    if (listaConteo == NULL) {
        fprintf(archivo, "Usuario: %s (%s) -> Sin historial valido\n",
                usuario->nombre,
                usuario->nickname);
        return;
    }

    mayor = listaConteo;
    actual = listaConteo;

    while (actual != NULL) {
        if (actual->cantidad > mayor->cantidad) {
            mayor = actual;
        }
        actual = actual->sig;
    }

    fprintf(archivo, "Usuario: %s (%s) -> Artista preferido: %s (%d veces)\n",
            usuario->nombre,
            usuario->nickname,
            mayor->artista,
            mayor->cantidad);

    liberarConteoArtistas(listaConteo);
}

void reporteArtistasPreferidosTodosRec(Usuario* raiz, FILE* archivo) {
    if (raiz == NULL) {
        return;
    }

    reporteArtistasPreferidosTodosRec(raiz->izq, archivo);
    escribirArtistaPreferidoUsuario(archivo, raiz);
    reporteArtistasPreferidosTodosRec(raiz->der, archivo);
}

void reporteArtistasPreferidosTodos(Usuario* raizUsuarios) {
    FILE* archivo = fopen("artistas_preferidos.txt", "w");

    if (archivo == NULL) {
        printf(ROJO "No se pudo crear artistas_preferidos.txt\n" RESET);
        return;
    }

    fprintf(archivo, "--- REPORTE DE ARTISTAS PREFERIDOS POR USUARIO ---\n\n");

    if (raizUsuarios == NULL) {
        fprintf(archivo, "No hay usuarios registrados.\n");
    } else {
        reporteArtistasPreferidosTodosRec(raizUsuarios, archivo);
    }

    fclose(archivo);

    printf(VERDE " [OK] Reporte generado: artistas_preferidos.txt\n" RESET);
}

void reporteTiempoTotalTodosRec(Usuario* raiz, FILE* archivo) {
    int minutos;
    int segundos;

    if (raiz == NULL) {
        return;
    }

    reporteTiempoTotalTodosRec(raiz->izq, archivo);

    minutos = raiz->tiempoTotalReproduccion / 60;
    segundos = raiz->tiempoTotalReproduccion % 60;

    fprintf(archivo, "Usuario: %s (%s)\n", raiz->nombre, raiz->nickname);
    fprintf(archivo, "Tiempo total: %d segundos\n", raiz->tiempoTotalReproduccion);
    fprintf(archivo, "Equivalente: %d minutos y %d segundos\n\n", minutos, segundos);

    reporteTiempoTotalTodosRec(raiz->der, archivo);
}

void reporteTiempoTotalTodos(Usuario* raizUsuarios) {
    FILE* archivo = fopen("tiempo_total.txt", "w");

    if (archivo == NULL) {
        printf(ROJO "No se pudo crear tiempo_total.txt\n" RESET);
        return;
    }

    fprintf(archivo, "--- REPORTE DE TIEMPO TOTAL DE REPRODUCCION ---\n\n");

    if (raizUsuarios == NULL) {
        fprintf(archivo, "No hay usuarios registrados.\n");
    } else {
        reporteTiempoTotalTodosRec(raizUsuarios, archivo);
    }

    fclose(archivo);

    printf(VERDE " [OK] Reporte generado: tiempo_total.txt\n" RESET);
}

void reporteAnunciosFreeTodosRec(Usuario* raiz, FILE* archivo) {
    if (raiz == NULL) {
        return;
    }

    reporteAnunciosFreeTodosRec(raiz->izq, archivo);

    if (strcmp(raiz->plan, "free") == 0 || raiz->premiumActivo == 0) {
        fprintf(archivo, "Usuario: %s (%s)\n", raiz->nombre, raiz->nickname);
        fprintf(archivo, "Plan: %s\n", raiz->plan);
        fprintf(archivo, "Anuncios mostrados: %d\n\n", raiz->anunciosMostrados);
    }

    reporteAnunciosFreeTodosRec(raiz->der, archivo);
}

void reporteAnunciosFreeTodos(Usuario* raizUsuarios) {
    FILE* archivo = fopen("reporte_anuncios.txt", "w");

    if (archivo == NULL) {
        printf(ROJO "No se pudo crear reporte_anuncios.txt\n" RESET);
        return;
    }

    fprintf(archivo, "--- REPORTE DE ANUNCIOS MOSTRADOS A USUARIOS FREE ---\n\n");

    if (raizUsuarios == NULL) {
        fprintf(archivo, "No hay usuarios registrados.\n");
    } else {
        reporteAnunciosFreeTodosRec(raizUsuarios, archivo);
    }

    fclose(archivo);

    printf(VERDE " [OK] Reporte generado: reporte_anuncios.txt\n" RESET);
}

void generarReportes(Usuario* raizUsuarios, Artista* raizArtistas) {
    printf(MAGENTA "\n========== GENERANDO REPORTES ==========\n" RESET);

    reporteTopCanciones(raizArtistas);
    reporteArtistasPreferidosTodos(raizUsuarios);
    reporteTiempoTotalTodos(raizUsuarios);
    reporteAnunciosFreeTodos(raizUsuarios);

    printf(VERDE "\n [OK] Todos los reportes fueron generados correctamente.\n" RESET);
}

void mostrarUsuariosPremium(Usuario* raiz) {
    if (raiz == NULL) return;
    mostrarUsuariosPremium(raiz->izq);
    if (raiz->premiumActivo) {
        printf("- %s (%s) - Vence: %s\n", raiz->nickname, raiz->nombre, raiz->fechaVencimientoPremium);
    }
    mostrarUsuariosPremium(raiz->der);
}