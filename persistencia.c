#include "persistencia.h"
#include "usuarios.h"
#include "artistas.h"
#include "playlist.h"
#include "historial.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void guardarUsuariosRec(Usuario* raiz, FILE* f) {
    if (raiz == NULL) {
        return;
    }

    fprintf(f, "%s,%s,%s,%s,%s,%s,%d,%s,%.2f,%d,%d,%d\n",
            raiz->nickname,
            raiz->nombre,
            raiz->correo,
            raiz->pais,
            raiz->contrasena,
            raiz->plan,
            raiz->premiumActivo,
            raiz->fechaVencimientoPremium,
            raiz->valorPremium,
            raiz->cancionesEscuchadasDesdeAnuncio,
            raiz->anunciosMostrados,
            raiz->tiempoTotalReproduccion);

    guardarUsuariosRec(raiz->izq, f);
    guardarUsuariosRec(raiz->der, f);
}

void guardarArtistasRec(Artista* raiz, FILE* f) {
    if (raiz == NULL) {
        return;
    }

    fprintf(f, "ARTISTA,%s\n", raiz->nombre);

    Disco* d = raiz->listaDiscos;

    while (d != NULL) {
        fprintf(f, "DISCO,%s,%s\n", d->nombre, d->fechaLanzamiento);

        Cancion* c = d->listaCanciones;

        while (c != NULL) {
            fprintf(f, "CANCION,%s,%s,%d,%s,%d\n",
                    c->nombre,
                    c->artista,
                    c->duracionSegundos,
                    c->archivoOrigen,
                    c->reproducciones);

            c = c->sig;
        }

        d = d->sig;
    }

    guardarArtistasRec(raiz->izq, f);
    guardarArtistasRec(raiz->der, f);
}

void guardarPlaylistsRec(Usuario* raiz, FILE* f) {
    if (raiz == NULL) return;
    Playlist* p = raiz->playlists;
    while (p != NULL) {
        NodoCancionPlaylist* cp = p->canciones;
        while (cp != NULL) {
            if (cp->cancion != NULL) {
                fprintf(f, "%s,%s,%s,%s\n", raiz->nickname, p->nombre, cp->cancion->nombre, cp->cancion->artista);
            }
            cp = cp->sig;
        }
        p = p->sig;
    }
    guardarPlaylistsRec(raiz->izq, f);
    guardarPlaylistsRec(raiz->der, f);
}

void guardarHistorialRec(Usuario* raiz, FILE* f) {
    if (raiz == NULL) return;
    NodoHistorial* h = raiz->historial;
    while (h != NULL) {
        if (h->cancion != NULL) {
            fprintf(f, "%s,%s,%s,%s\n", raiz->nickname, h->fecha, h->cancion->nombre, h->cancion->artista);
        }
        h = h->sig;
    }
    guardarHistorialRec(raiz->izq, f);
    guardarHistorialRec(raiz->der, f);
}

void guardarAmigosRec(Usuario* raiz, FILE* f) {
    if (raiz == NULL) return;
    NodoAmigo* a = raiz->amigos;
    while (a != NULL) {
        if (a->usuarioAmigo != NULL) {
            fprintf(f, "%s,%s\n", raiz->nickname, a->usuarioAmigo->nickname);
        }
        a = a->sig;
    }
    guardarAmigosRec(raiz->izq, f);
    guardarAmigosRec(raiz->der, f);
}

void guardarDatos(Usuario* raizUsuarios, Artista* raizArtistas) {
    FILE* fU = fopen("usuarios.txt", "w");
    if (fU != NULL) { guardarUsuariosRec(raizUsuarios, fU); fclose(fU); }
    
    FILE* fA = fopen("catalogo.txt", "w");
    if (fA != NULL) { guardarArtistasRec(raizArtistas, fA); fclose(fA); }
    
    FILE* fP = fopen("playlists.txt", "w");
    if (fP != NULL) { guardarPlaylistsRec(raizUsuarios, fP); fclose(fP); }
    
    FILE* fH = fopen("historial.txt", "w");
    if (fH != NULL) { guardarHistorialRec(raizUsuarios, fH); fclose(fH); }
    
    FILE* fAm = fopen("amigos.txt", "w");
    if (fAm != NULL) { guardarAmigosRec(raizUsuarios, fAm); fclose(fAm); }
}

void cargarDatos(Usuario** raizUsuarios, Artista** raizArtistas) {
    FILE* fU = fopen("usuarios.txt", "r");

    if (fU != NULL) {
        char linea[400];

        while (fgets(linea, 400, fU) != NULL) {
            Usuario* u = crearUsuarioVacio();

            if (u == NULL) {
                continue;
            }

            /* Valores por defecto por si el archivo viejo no tiene todos los campos */
            strcpy(u->fechaVencimientoPremium, "");
            u->valorPremium = 0.0;
            u->cancionesEscuchadasDesdeAnuncio = 0;
            u->anunciosMostrados = 0;
            u->tiempoTotalReproduccion = 0;

            char* t = strtok(linea, ",");
            if (t) strcpy(u->nickname, t);

            t = strtok(NULL, ",");
            if (t) strcpy(u->nombre, t);

            t = strtok(NULL, ",");
            if (t) strcpy(u->correo, t);

            t = strtok(NULL, ",");
            if (t) strcpy(u->pais, t);

            t = strtok(NULL, ",");
            if (t) strcpy(u->contrasena, t);

            t = strtok(NULL, ",");
            if (t) strcpy(u->plan, t);

            t = strtok(NULL, ",");
            if (t) u->premiumActivo = atoi(t);

            t = strtok(NULL, ",");
            if (t) strcpy(u->fechaVencimientoPremium, t);

            t = strtok(NULL, ",");
            if (t) u->valorPremium = (float)atof(t);

            t = strtok(NULL, ",");
            if (t) u->cancionesEscuchadasDesdeAnuncio = atoi(t);

            t = strtok(NULL, ",");
            if (t) u->anunciosMostrados = atoi(t);

            t = strtok(NULL, "\n");
            if (t) u->tiempoTotalReproduccion = atoi(t);

            insertarUsuario(raizUsuarios, u);
        }

        fclose(fU);
    }
    
    FILE* fA = fopen("catalogo.txt", "r");
    if (fA != NULL) {
        char linea[300];
        Artista* artistaActual = NULL;
        Disco* discoActual = NULL;
        while (fgets(linea, 300, fA) != NULL) {
            char* tipo = strtok(linea, ",");
            if (strcmp(tipo, "ARTISTA") == 0) {
                char* nombre = strtok(NULL, "\n");
                artistaActual = crearArtista(nombre);
                insertarArtista(raizArtistas, artistaActual);
            } else if (strcmp(tipo, "DISCO") == 0 && artistaActual != NULL) {
                char* nombre = strtok(NULL, ",");
                char* fecha = strtok(NULL, "\n");
                discoActual = crearDisco(nombre, fecha);
                agregarDisco(artistaActual, discoActual);
            } else if (strcmp(tipo, "CANCION") == 0 && discoActual != NULL) {
                char* nombre = strtok(NULL, ",");
                char* artista = strtok(NULL, ",");
                char* dur = strtok(NULL, ",");
                char* arch = strtok(NULL, ",");
                char* reps = strtok(NULL, "\n");

                if (nombre && artista && dur && arch) {
                    arch[strcspn(arch, "\n")] = '\0';

                    Cancion* c = crearCancion(artista, nombre, atoi(dur), arch);

                    if (c != NULL) {
                        if (reps != NULL) {
                            c->reproducciones = atoi(reps);
                        } else {
                            c->reproducciones = 0;
                        }

                        agregarCancion(discoActual, c);
                    }
                }
            }
        }
        fclose(fA);
    }

    FILE* fP = fopen("playlists.txt", "r");
    if (fP != NULL) {
        char linea[300];
        while (fgets(linea, 300, fP) != NULL) {
            char* nick = strtok(linea, ",");
            char* nomPlay = strtok(NULL, ",");
            char* nomCan = strtok(NULL, ",");
            char* artCan = strtok(NULL, "\n");
            if (nick && nomPlay && nomCan && artCan) {
                Usuario* u = buscarUsuario(*raizUsuarios, nick);
                Cancion* c = buscarCancionSistema(*raizArtistas, artCan, nomCan);
                if (u != NULL && c != NULL) {
                    Playlist* p = u->playlists;
                    while (p != NULL && strcmp(p->nombre, nomPlay) != 0) p = p->sig;
                    if (p == NULL) {
                        p = crearPlaylist(nomPlay);
                        p->sig = u->playlists;
                        u->playlists = p;
                    }
                    agregarCancionAPlaylist(p, c);
                }
            }
        }
        fclose(fP);
    }
    
    FILE* fH = fopen("historial.txt", "r");
    if (fH != NULL) {
        char linea[300];
        while (fgets(linea, 300, fH) != NULL) {
            char* nick = strtok(linea, ",");
            char* fecha = strtok(NULL, ",");
            char* nomCan = strtok(NULL, ",");
            char* artCan = strtok(NULL, "\n");
            if (nick && fecha && nomCan && artCan) {
                Usuario* u = buscarUsuario(*raizUsuarios, nick);
                Cancion* c = buscarCancionSistema(*raizArtistas, artCan, nomCan);
                if (u != NULL && c != NULL) {
                    pushHistorial(u, c, fecha);
                }
            }
        }
        fclose(fH);
    }
    
    FILE* fAm = fopen("amigos.txt", "r");
    if (fAm != NULL) {
        char linea[150];
        while (fgets(linea, 150, fAm) != NULL) {
            char* nick1 = strtok(linea, ",");
            char* nick2 = strtok(NULL, "\n");
            if (nick1 && nick2) {
                Usuario* u1 = buscarUsuario(*raizUsuarios, nick1);
                Usuario* u2 = buscarUsuario(*raizUsuarios, nick2);
                if (u1 != NULL && u2 != NULL) {
                    NodoAmigo* n = (NodoAmigo*)malloc(sizeof(NodoAmigo));
                    n->usuarioAmigo = u2;
                    n->sig = u1->amigos;
                    u1->amigos = n;
                }
            }
        }
        fclose(fAm);
    }
}