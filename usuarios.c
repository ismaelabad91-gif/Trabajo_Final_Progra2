#include "usuarios.h"
#include "playlist.h"
#include "historial.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "colores.h"
#include "interfaz.h"
#include "menus.h"

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

void liberarDatosUsuario(Usuario* usuario) {
    NodoAmigo* actualAmigo;
    NodoAmigo* tempAmigo;

    if (usuario == NULL) {
        return;
    }

    liberarPlaylists(usuario->playlists);
    liberarHistorial(usuario->historial);

    actualAmigo = usuario->amigos;

    while (actualAmigo != NULL) {
        tempAmigo = actualAmigo;
        actualAmigo = actualAmigo->sig;
        free(tempAmigo);
    }

    usuario->playlists = NULL;
    usuario->historial = NULL;
    usuario->amigos = NULL;
}

void eliminarAmigoDeLista(Usuario* usuario, char nicknameEliminado[]) {
    NodoAmigo* actual;
    NodoAmigo* anterior;
    NodoAmigo* temp;

    if (usuario == NULL) {
        return;
    }

    actual = usuario->amigos;
    anterior = NULL;

    while (actual != NULL) {
        if (actual->usuarioAmigo != NULL &&
            strcmp(actual->usuarioAmigo->nickname, nicknameEliminado) == 0) {

            temp = actual;

            if (anterior == NULL) {
                usuario->amigos = actual->sig;
                actual = usuario->amigos;
            } else {
                anterior->sig = actual->sig;
                actual = anterior->sig;
            }

            free(temp);
        } else {
            anterior = actual;
            actual = actual->sig;
        }
    }
}

void eliminarUsuarioDeAmigosTodos(Usuario* raiz, char nicknameEliminado[]) {
    if (raiz == NULL) {
        return;
    }

    eliminarUsuarioDeAmigosTodos(raiz->izq, nicknameEliminado);
    eliminarAmigoDeLista(raiz, nicknameEliminado);
    eliminarUsuarioDeAmigosTodos(raiz->der, nicknameEliminado);
}

Usuario* extraerMinimoUsuario(Usuario* raiz, Usuario** minimo) {
    if (raiz->izq == NULL) {
        *minimo = raiz;
        return raiz->der;
    }

    raiz->izq = extraerMinimoUsuario(raiz->izq, minimo);
    return raiz;
}

Usuario* eliminarUsuario(Usuario* raiz, char nickname[]) {
    int cmp;
    Usuario* temp;
    Usuario* sucesor;

    if (raiz == NULL) {
        return NULL;
    }

    cmp = strcmp(nickname, raiz->nickname);

    if (cmp < 0) {
        raiz->izq = eliminarUsuario(raiz->izq, nickname);
    } else if (cmp > 0) {
        raiz->der = eliminarUsuario(raiz->der, nickname);
    } else {

        if (raiz->izq == NULL) {
            temp = raiz->der;
            liberarDatosUsuario(raiz);
            free(raiz);
            printf(VERDE "\n Usuario eliminado correctamente.\n" RESET);
            return temp;
        }

        if (raiz->der == NULL) {
            temp = raiz->izq;
            liberarDatosUsuario(raiz);
            free(raiz);
            printf(VERDE "\n Usuario eliminado correctamente.\n" RESET);
            return temp;
        }

        sucesor = NULL;
        raiz->der = extraerMinimoUsuario(raiz->der, &sucesor);

        sucesor->izq = raiz->izq;
        sucesor->der = raiz->der;

        liberarDatosUsuario(raiz);
        free(raiz);

        printf(VERDE "\n Usuario eliminado correctamente.\n" RESET);
        return sucesor;
    }

    return raiz;
}

int eliminarUsuarioSistema(Usuario** raizUsuarios, char nickname[]) {
    Usuario* encontrado;

    if (raizUsuarios == NULL || *raizUsuarios == NULL) {
        printf(ROJO "\n No hay usuarios registrados.\n" RESET);
        return 0;
    }

    encontrado = buscarUsuario(*raizUsuarios, nickname);

    if (encontrado == NULL) {
        printf(ROJO "\n Usuario no encontrado.\n" RESET);
        return 0;
    }

    eliminarUsuarioDeAmigosTodos(*raizUsuarios, nickname);
    *raizUsuarios = eliminarUsuario(*raizUsuarios, nickname);

    return 1;
}

void mostrarPlaylistsUsuario(Usuario* usuario) {
    if (usuario == NULL) return;
    mostrarTodasPlaylists(usuario->playlists);
}

void modificarUsuario(Usuario* usuario) {
    int opcion;
    char nuevoDato[MAX_CORREO];
    time_t t;
    struct tm* tm;

    if (usuario == NULL) {
        printf(ROJO "\n Usuario no valido.\n" RESET);
        return;
    }

    do {
        limpiarPantalla();
        dibujarEncabezado("MODIFICAR USUARIO");

        printf(CYAN " Usuario actual: " RESET "%s\n", usuario->nickname);
        printf(CYAN " 1. Nombre:     " RESET "%s\n", usuario->nombre);
        printf(CYAN " 2. Correo:     " RESET "%s\n", usuario->correo);
        printf(CYAN " 3. Pais:       " RESET "%s\n", usuario->pais);
        printf(CYAN " 4. Contrasena: " RESET "%s\n", usuario->contrasena);
        printf(CYAN " 5. Plan:       " RESET "%s\n", usuario->plan);
        printf(CYAN " 0. Volver\n" RESET);

        printf(CYAN "\nSeleccione el campo a modificar: " RESET);

        if (scanf("%d", &opcion) != 1) {
            limpiarBuffer();
            printf(ROJO "\nOpcion invalida.\n" RESET);
            continue;
        }

        limpiarBuffer();

        switch (opcion) {
            case 1:
                printf(CYAN "Nuevo nombre: " RESET);
                fgets(usuario->nombre, MAX_NOMBRE, stdin);
                usuario->nombre[strcspn(usuario->nombre, "\n")] = '\0';
                printf(VERDE "\nNombre actualizado.\n" RESET);
                break;

            case 2:
                printf(CYAN "Nuevo correo: " RESET);
                fgets(usuario->correo, MAX_CORREO, stdin);
                usuario->correo[strcspn(usuario->correo, "\n")] = '\0';
                printf(VERDE "\nCorreo actualizado.\n" RESET);
                break;

            case 3:
                printf(CYAN "Nuevo pais: " RESET);
                fgets(usuario->pais, MAX_PAIS, stdin);
                usuario->pais[strcspn(usuario->pais, "\n")] = '\0';
                printf(VERDE "\nPais actualizado.\n" RESET);
                break;

            case 4:
                printf(CYAN "Nueva contrasena: " RESET);
                fgets(usuario->contrasena, MAX_CONTRASENA, stdin);
                usuario->contrasena[strcspn(usuario->contrasena, "\n")] = '\0';
                printf(VERDE "\nContrasena actualizada.\n" RESET);
                break;

            case 5:
                printf(CYAN "Nuevo plan (free/premium): " RESET);
                fgets(nuevoDato, MAX_PLAN, stdin);
                nuevoDato[strcspn(nuevoDato, "\n")] = '\0';

                if (strcmp(nuevoDato, "free") == 0) {
                    strcpy(usuario->plan, "free");
                    usuario->premiumActivo = 0;
                    strcpy(usuario->fechaVencimientoPremium, "");
                    printf(VERDE "\nPlan cambiado a Free.\n" RESET);
                } else if (strcmp(nuevoDato, "premium") == 0) {
                    strcpy(usuario->plan, "premium");
                    usuario->premiumActivo = 1;

                    t = time(NULL);
                    tm = localtime(&t);
                    tm->tm_mon += 1;

                    strftime(usuario->fechaVencimientoPremium, MAX_FECHA, "%Y-%m-%d", tm);

                    if (usuario->valorPremium <= 0) {
                        usuario->valorPremium = 9.99;
                    }

                    printf(VERDE "\nPlan cambiado a Premium.\n" RESET);
                    printf(VERDE "Premium activo hasta: %s\n" RESET, usuario->fechaVencimientoPremium);
                } else {
                    printf(ROJO "\nPlan invalido. Use free o premium.\n" RESET);
                }
                break;

            case 0:
                printf(GRIS "\nVolviendo...\n" RESET);
                break;

            default:
                printf(ROJO "\nOpcion invalida.\n" RESET);
        }

        if (opcion != 0) {
            printf(GRIS "\nPresione ENTER para continuar..." RESET);
            getchar();
        }

    } while (opcion != 0);
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