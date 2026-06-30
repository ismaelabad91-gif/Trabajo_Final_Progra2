#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <mmsystem.h> // Librería multimedia de Windows
#include <string.h>
#include <time.h>
#include "menus.h"
#include "estructuras.h"
#include "usuarios.h"
#include "artistas.h"
#include "playlist.h"
#include "anuncios.h"
#include "historial.h"
#include "reportes.h"
#include "colores.h"
#include "interfaz.h"


extern Artista *raizArtistas;
extern Playlist *playlists;
extern ColaAnuncios *colaAnuncios;
extern Usuario *raizUsuarios;

void limpiarBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

void pausa(void) {
    printf(GRIS_O "\n Presione ENTER para continuar..." RESET);
    fflush(stdin);
    getchar();
}

int menuPrincipal(void) {
    int opcion;
    
    printf(BLANCO "\n========================================\n");
    printf(VERDE "           S P O T I F Y \n");
    printf(BLANCO "========================================\n" RESET);
    dibujarEncabezado("MENU PRINCIPAL");
    printf(" 1. " BLANCO"Iniciar Sesion\n" RESET);
    printf(" 2. " BLANCO "Crear Cuenta\n" RESET);
    printf(" 3. " BLANCO "Modo Desarrollador\n" RESET);
    printf(" 4. " ROJO "Salir\n" RESET);
    printf(BLANCO "========================================\n" RESET);
    printf("Seleccione una opcion: ");
    
    if (scanf("%d", &opcion) != 1) {
        limpiarBuffer();
        return -1;
    }
    
    limpiarBuffer();
    return opcion;
}

int textoVacio(char texto[]) {
    return strlen(texto) == 0;
}

int correoValido(char correo[]) {
    int i;
    int tieneArroba = 0;
    int tienePunto = 0;

    if (strlen(correo) < 5) {
        return 0;
    }

    for (i = 0; correo[i] != '\0'; i++) {
        if (correo[i] == '@') {
            tieneArroba = 1;
        }

        if (correo[i] == '.') {
            tienePunto = 1;
        }
    }

    return tieneArroba && tienePunto;
}

void leerTextoObligatorio(char mensaje[], char destino[], int tam) {
    do {
        printf("%s", mensaje);
        fgets(destino, tam, stdin);
        destino[strcspn(destino, "\n")] = '\0';

        if (textoVacio(destino)) {
            printf(ROJO " [!] Este campo no puede estar vacio.\n" RESET);
        }

    } while (textoVacio(destino));
}

int leerEnteroPositivo(char mensaje[]) {
    int numero;

    do {
        printf("%s", mensaje);

        if (scanf("%d", &numero) != 1) {
            limpiarBuffer();
            printf(ROJO " [!] Debe ingresar un numero valido.\n" RESET);
            numero = -1;
        } else {
            limpiarBuffer();

            if (numero <= 0) {
                printf(ROJO " [!] El numero debe ser mayor a 0.\n" RESET);
            }
        }

    } while (numero <= 0);

    return numero;
}

Usuario* registrarNuevoUsuario(void) {
    Usuario* nuevo = crearUsuarioVacio();
    
    if (nuevo == NULL) {
        printf(ROJO "Error al crear usuario.\n" RESET);
        return NULL;
    }
    
    do {
        leerTextoObligatorio(CYAN " Correo: " RESET, nuevo->correo, MAX_CORREO);

        if (!correoValido(nuevo->correo)) {
            printf(ROJO " [!] Correo invalido. Debe contener @ y punto.\n" RESET);
        }

    } while (!correoValido(nuevo->correo));

    leerTextoObligatorio(CYAN " Nombre: " RESET, nuevo->nombre, MAX_NOMBRE);
    leerTextoObligatorio(CYAN " Pais:   " RESET, nuevo->pais, MAX_PAIS);
    leerTextoObligatorio(CYAN " Nick:   " RESET, nuevo->nickname, MAX_NICKNAME);
    leerTextoObligatorio(CYAN " Clave:  " RESET, nuevo->contrasena, MAX_CONTRASENA);
    
    int planOpcion;
    do {
        printf(CYAN "\n +--------------------------+ \n");
        printf(VERDE " | SELECCIONE SU PLAN       | \n");
        printf(CYAN " +--------------------------+ \n" RESET);
        printf(" 1. " AMARILLO "Free " RESET "(Con anuncios)\n");
        printf(" 2. " AMARILLO "Premium " RESET "($9.99 - Sin anuncios)\n");
        printf(CYAN " Opcion: " RESET);
        scanf("%d", &planOpcion);
        limpiarBuffer();
        
        if (planOpcion == 1) {
            strcpy(nuevo->plan, "free");
            nuevo->premiumActivo = 0;
            printf(VERDE "\n Plan Free registrado.\n" RESET);
            printf("   Escucharas anuncios cada 3 canciones.\n");
        } else if (planOpcion == 2) {
            strcpy(nuevo->plan, "premium");
            nuevo->premiumActivo = 1;
            
            time_t t = time(NULL);
            struct tm* tm = localtime(&t);
            tm->tm_mon += 1;
            strftime(nuevo->fechaVencimientoPremium, MAX_FECHA, "%Y-%m-%d", tm);
            nuevo->valorPremium = 9.99;
            printf("\nPlan Premium seleccionado.\n");
            printf(VERDE "\n Plan Premium activo hasta: %s\n" RESET, nuevo->fechaVencimientoPremium);
        } else {
            printf(ROJO "Opcion invalida. Intente de nuevo.\n" RESET);
        }
    } while (planOpcion != 1 && planOpcion != 2);
    
    nuevo->cancionesEscuchadasDesdeAnuncio = 0;
    nuevo->anunciosMostrados = 0;
    nuevo->tiempoTotalReproduccion = 0;
    nuevo->playlists = NULL;
    nuevo->historial = NULL;
    nuevo->amigos = NULL;
    nuevo->izq = NULL;
    nuevo->der = NULL;
    
    return nuevo;
}

Playlist* buscarPlaylistUsuario(Usuario* usuario, char nombre[]) {
    Playlist* actual;

    if (usuario == NULL) {
        return NULL;
    }

    actual = usuario->playlists;

    while (actual != NULL) {
        if (strcmp(actual->nombre, nombre) == 0) {
            return actual;
        }

        actual = actual->sig;
    }

    return NULL;
}

void crearPlaylistUsuario(Usuario* usuario) {
    char nombre[MAX_NOMBRE];
    Playlist* existente;
    Playlist* nueva;

    if (usuario == NULL) {
        return;
    }

    limpiarPantalla();
    dibujarEncabezado("CREAR NUEVA PLAYLIST");

    printf(CYAN " Nombre de la playlist: " RESET);
    fgets(nombre, sizeof(nombre), stdin);
    nombre[strcspn(nombre, "\n")] = '\0';

    if (strlen(nombre) == 0) {
        printf(ROJO "\n [!] El nombre de la playlist no puede estar vacio.\n" RESET);
        return;
    }

    existente = buscarPlaylistUsuario(usuario, nombre);

    if (existente != NULL) {
        printf(ROJO "\n [!] Ya tienes una playlist con ese nombre.\n" RESET);
        return;
    }

    nueva = crearPlaylist(nombre);

    if (nueva != NULL) {
        nueva->sig = usuario->playlists;
        usuario->playlists = nueva;

        printf(VERDE "\n [OK] Playlist '%s' creada con exito.\n" RESET, nombre);
    } else {
        printf(ROJO "\n [!] No se pudo crear la playlist.\n" RESET);
    }
}

void eliminarPlaylistUsuario(Usuario* usuario) {
    char nombre[MAX_NOMBRE];

    if (usuario == NULL) {
        return;
    }

    limpiarPantalla();
    dibujarEncabezado("ELIMINAR PLAYLIST");

    if (usuario->playlists == NULL) {
        printf(AMARILLO "\n [!] No tienes playlists para eliminar.\n" RESET);
        return;
    }

    printf(CYAN "\nTus playlists:\n" RESET);
    mostrarTodasPlaylists(usuario->playlists);

    printf(CYAN "\nNombre de la playlist a eliminar: " RESET);
    fgets(nombre, sizeof(nombre), stdin);
    nombre[strcspn(nombre, "\n")] = '\0';

    if (strlen(nombre) == 0) {
        printf(ROJO "\n [!] El nombre no puede estar vacio.\n" RESET);
        return;
    }

    usuario->playlists = eliminarPlaylist(usuario->playlists, nombre);
}

void agregarCancionAPlaylistUsuario(Usuario* usuario) {
    if (usuario == NULL) return;
    
    char nombrePlaylist[MAX_NOMBRE];
    char nombreArtista[MAX_NOMBRE];
    char nombreCancion[MAX_NOMBRE];
    
    limpiarPantalla();
    dibujarEncabezado("AGREGAR CANCION A PLAYLIST");
    
    if (usuario->playlists == NULL) {
        printf(ROJO " [!] No tienes playlists. Crea una primero.\n" RESET);
        return;
    }
    
    printf(CYAN " Tus playlists:\n" RESET);
    Playlist* p = usuario->playlists;
    int i = 1;
    
    while (p != NULL) {
        printf(" " AMARILLO "%d." RESET " %s\n", i++, p->nombre);
        p = p->sig;
    }
    
    printf(CYAN "\nNombre de la playlist: " RESET);
    fgets(nombrePlaylist, sizeof(nombrePlaylist), stdin);
    nombrePlaylist[strcspn(nombrePlaylist, "\n")] = '\0';
    
    Playlist* playlist = usuario->playlists;
    
    while (playlist != NULL && strcmp(playlist->nombre, nombrePlaylist) != 0) {
        playlist = playlist->sig;
    }
    
    if (playlist == NULL) {
        printf(ROJO " [!] Playlist no encontrada.\n" RESET);
        return;
    }
    
    printf(CYAN "\n--- Seleccion de Cancion ---\n" RESET);
    mostrarTodosArtistasYDiscos(raizArtistas);
    
    printf(GRIS "\nNombre del artista: " RESET);
    fgets(nombreArtista, sizeof(nombreArtista), stdin);
    nombreArtista[strcspn(nombreArtista, "\n")] = '\0';
    
    printf(GRIS "Nombre de la cancion: " RESET);
    fgets(nombreCancion, sizeof(nombreCancion), stdin);
    nombreCancion[strcspn(nombreCancion, "\n")] = '\0';
    
    Cancion* cancion = buscarCancionSistema(raizArtistas, nombreArtista, nombreCancion);
    if (cancion != NULL) {
        agregarCancionAPlaylist(playlist, cancion);
        printf(VERDE "\n [OK] Cancion agregada exitosamente.\n" RESET);
    } else {
        printf(ROJO "\n [!] Cancion o artista no encontrado.\n" RESET);
    }
}

void agregarAmigo(Usuario* usuario) {
    if (usuario == NULL) return;
    
    char nicknameAmigo[MAX_NICKNAME];
    limpiarPantalla();
    dibujarEncabezado("AGREGAR AMIGO");
    printf(CYAN "Nickname del amigo: " RESET);
    fgets(nicknameAmigo, sizeof(nicknameAmigo), stdin);
    nicknameAmigo[strcspn(nicknameAmigo, "\n")] = '\0';
    
    if (strcmp(nicknameAmigo, usuario->nickname) == 0) {
        printf(ROJO " [!] No puedes agregarte a ti mismo.\n" RESET);
        return;
    }
    
    Usuario* amigo = buscarUsuario(raizUsuarios, nicknameAmigo);
    
    if (amigo == NULL) {
        printf(ROJO " [!] Usuario '%s' no encontrado.\n" RESET, nicknameAmigo);
        return;
    }
    
    NodoAmigo* actual = usuario->amigos;
    
    while (actual != NULL) {
        if (strcmp(actual->usuarioAmigo->nickname, nicknameAmigo) == 0) {
            printf(AMARILLO "\n [i] '%s' ya esta en tu lista de amigos.\n" RESET, nicknameAmigo);
            return;
        }
        actual = actual->sig;
    }
    
    NodoAmigo* nuevo = (NodoAmigo*)malloc(sizeof(NodoAmigo));
    
    if (nuevo != NULL) {
        nuevo->usuarioAmigo = amigo;
        nuevo->sig = usuario->amigos;
        usuario->amigos = nuevo;
        printf(VERDE "\n [OK] '%s' agregado con exito.\n" RESET, nicknameAmigo);
    }
}

void mostrarRecomendaciones(Usuario* usuario) {
    if (usuario == NULL) return;
    limpiarPantalla();
    dibujarEncabezado("RECOMENDACIONES PARA TI");
    
    if (usuario->amigos == NULL) {
        printf(AMARILLO " [!] Aun no tienes amigos.\n" RESET);
        printf(" Agrega amigos para ver que estan escuchando!\n");
        return;
    }
    
    printf(BLANCO " Canciones que tus amigos estan escuchando:\n" RESET);
    NodoAmigo* amigo = usuario->amigos;
    int encontradas = 0;
    
    while (amigo != NULL) {
        NodoHistorial* hist = amigo->usuarioAmigo->historial;
        if (hist != NULL) {
            printf(BLANCO "\n > De  %s :\n" RESET , amigo->usuarioAmigo->nickname);
            int count = 0;
            NodoHistorial* temp = hist;
            while (temp != NULL && count < 3) {
                printf(GRIS "   - %s " RESET "por " AMARILLO "%s\n" RESET, 
                       temp->cancion->nombre, temp->cancion->artista);
                temp = temp->sig;
                count++;
                encontradas++;
            }
        }
        amigo = amigo->sig;
    }
    
    if (encontradas == 0) {
        printf(AMARILLO " [!] Tus amigos no tienen historial de reproduccion.\n" RESET);
    }
}

void menuIngresar(void) {
    int opcion;
    char nickname[MAX_NICKNAME];
    char contrasena[MAX_CONTRASENA];
    Usuario* usuarioActual = NULL;
    int intentos = 0;
    int maxIntentos = 3;
    
    do {
        limpiarPantalla();
        dibujarEncabezado("INICIO DE SESION");

        printf(GRIS " Usuario: " RESET);
        fgets(nickname, sizeof(nickname), stdin);
        nickname[strcspn(nickname, "\n")] = '\0';
        
        printf(GRIS " Contrasena: " RESET);
        fgets(contrasena, sizeof(contrasena), stdin);
        contrasena[strcspn(contrasena, "\n")] = '\0';
        
        usuarioActual = buscarUsuario(raizUsuarios, nickname);
        
        if (usuarioActual != NULL) {
            if (strcmp(usuarioActual->contrasena, contrasena) == 0) {
                printf(VERDE "\n [OK] Bienvenido %s!\n" RESET, usuarioActual->nombre);
                printf(AMARILLO "   Plan: "RESET "%s\n" RESET, usuarioActual->plan);
                printf(AMARILLO "   Premium: "RESET "%s\n" RESET, usuarioActual->premiumActivo ? "Activo" : "Inactivo");
                pausa();
                break;
            } else {
                printf(ROJO "\n [!] Contrasena incorrecta.\n" RESET);
                intentos++;
                if (intentos < maxIntentos) {
                    printf(AMARILLO "   Intentos restantes: %d\n" RESET, maxIntentos - intentos);
                    pausa();
                }
                usuarioActual = NULL;
            }
        } else {
            printf(ROJO "\n [!] Usuario '%s' no encontrado.\n" RESET, nickname);
            printf(AMARILLO "\nDesea registrarse? (s/n): " RESET);
            char respuesta;
            scanf(" %c", &respuesta);
            limpiarBuffer();
            
            if (respuesta == 's' || respuesta == 'S') {
                usuarioActual = registrarNuevoUsuario();
                if (usuarioActual != NULL) {
                    insertarUsuario(&raizUsuarios, usuarioActual);
                    printf(VERDE "\nUsuario registrado exitosamente!\n" RESET);
                    printf(AMARILLO "   Bienvenido %s\n" RESET, usuarioActual->nombre);
                    pausa();
                    break;
                }
            } else {
                printf(AMARILLO "\nVolviendo al menu principal...\n" RESET);
                pausa();
                return;
            }
        }
        
        if (intentos >= maxIntentos && usuarioActual == NULL) {
            printf(ROJO "\n [!] Demasiados intentos fallidos.\n" RESET);
            printf(AMARILLO "   Volviendo al menu principal...\n" RESET);
            pausa();
            return;
        }
        
    } while (usuarioActual == NULL && intentos < maxIntentos);
    
    if (usuarioActual == NULL) {
        return;
    }
    
    do {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
        
        limpiarPantalla();
        dibujarEncabezado("MENU DE USUARIO");

        printf(GRIS "Usuario: %s (%s)\n" RESET, usuarioActual->nickname, usuarioActual->nombre);
        printf(GRIS "Plan: %s \n" RESET, usuarioActual->plan);
        printf("========================================\n");
        printf(" 1. " AMARILLO "Ver Perfil\n" RESET);
        printf(" 2. " AMARILLO "Mis Playlists\n" RESET);
        printf(" 3. " AMARILLO "Crear Playlist\n" RESET);
        printf(" 4. " AMARILLO "Agregar Cancion\n" RESET);
        printf("5. " AMARILLO "Eliminar Playlist\n" RESET);
        printf(" 6. " AMARILLO "Reproducir Musica\n" RESET);
        printf(" 7. " AMARILLO "Premium\n" RESET);
        printf(" 8. " AMARILLO "Agregar Amigo\n" RESET);
        printf(" 9. " AMARILLO "Recomendaciones\n" RESET);
        printf(" 10. " AMARILLO "Historial\n" RESET);
        printf(" 0. " ROJO "Cerrar Sesion\n" RESET);
        printf("========================================\n");
        printf(GRIS "Opcion: " RESET);
        scanf("%d", &opcion);
        limpiarBuffer();
        
        switch(opcion) {
            case 1:
                mostrarUsuarioDetallado(usuarioActual);
                pausa();
                break;
                
            case 2:
                mostrarPlaylistsUsuario(usuarioActual);
                pausa();
                break;
                
            case 3:
                crearPlaylistUsuario(usuarioActual);
                pausa();
                break;
                
            case 4:
                agregarCancionAPlaylistUsuario(usuarioActual);
                pausa();
                break;
                
            case 5:
                eliminarPlaylistUsuario(usuarioActual);
                pausa();
                break;
                
            case 6:
                reproducirMusica(usuarioActual);
                break;
                
            case 7:
                comprarPremium(usuarioActual);
                pausa();
                break;
                
            case 8:
                agregarAmigo(usuarioActual);
                pausa();
                break;
                
            case 9:
                mostrarRecomendaciones(usuarioActual);
                pausa();
                break;
            case 10:
                mostrarHistorial(usuarioActual);
                pausa();
                break;
            case 0:
                printf(GRIS "\nCerrando sesion...\n" RESET);
                break;
                
            default:
                printf(ROJO "\nOpcion invalida.\n" RESET);
                pausa();
        }
        
    } while(opcion != 0);
}

int autenticarDesarrollador(void) {
    char usuario[50];
    char contrasena[50];
    int intentos = 0;

    while (intentos < 3) {
        limpiarPantalla();
        dibujarEncabezado("ACCESO MODO DESARROLLADOR");

        printf(CYAN "Usuario: " RESET);
        fgets(usuario, sizeof(usuario), stdin);
        usuario[strcspn(usuario, "\n")] = '\0';

        printf(CYAN "Contrasena: " RESET);
        fgets(contrasena, sizeof(contrasena), stdin);
        contrasena[strcspn(contrasena, "\n")] = '\0';

        if (strcmp(usuario, "grupo1") == 0 && strcmp(contrasena, "Abad_Chuchuca") == 0) {
            printf(VERDE "\n[OK] Acceso concedido.\n" RESET);
            pausa();
            return 1;
        }

        intentos++;

        printf(ROJO "\n[!] Usuario o contrasena incorrectos.\n" RESET);

        if (intentos < 3) {
            printf(AMARILLO "Intentos restantes: %d\n" RESET, 3 - intentos);
            pausa();
        }
    }

    printf(ROJO "\n[!] Acceso denegado. Volviendo al menu principal.\n" RESET);
    pausa();

    return 0;
}

void menuDesarrollador(void) {
    int opcion;

        if (!autenticarDesarrollador()) {
        return;
    }

    do {
        limpiarPantalla();
        dibujarEngranaje();
        dibujarEncabezado("MODO DESARROLLADOR");

        printf(BLANCO " 1. " AMARILLO "Gestion de Usuarios\n" RESET);
        printf(BLANCO " 2. " AMARILLO "Gestion de Artistas\n" RESET);
        printf(BLANCO " 3. " AMARILLO "Gestion de Playlists\n" RESET);
        printf(BLANCO " 4. " AMARILLO "Gestion de Anuncios\n" RESET);
        printf(BLANCO " 5. " AMARILLO "Generar Reportes\n" RESET);
        printf(BLANCO " 6. " AMARILLO "Estadisticas del Sistema\n" RESET);
        printf(BLANCO " 7. " AMARILLO "Cargar Datos de Prueba\n" RESET);
        printf(BLANCO " 0. " ROJO "Volver al Menu Principal\n" RESET);
        printf("========================================\n");
        printf(BLANCO "\n Seleccione una opcion: " RESET);
        scanf("%d", &opcion);
        limpiarBuffer();
        
        switch(opcion) {
            case 1:
                menuUsuarios();
                break;
            case 2:
                menuArtistas();
                break;
            case 3:
                menuPlaylists();
                break;
            case 4:
                menuAnuncios();
                break;
            case 5:
                menuReportes();
                break;
            case 6:
                limpiarPantalla();
                dibujarEncabezado("ESTADISTICAS");
                mostrarEstadisticas();
                pausa();
                break;
            case 7:
                inicializarDatosPrueba();
                printf(VERDE "\n [OK] Datos de prueba cargados correctamente.\n" RESET);
                pausa();
                break;
            case 0:
                printf(GRIS "\nSaliendo del modo desarrollador...\n" RESET);
                break;
            default:
                printf(ROJO "\n Opcion invalida.\n" RESET);
                pausa();
        }
        
        if (opcion != 0) {
#ifdef _WIN32
            system("cls");
#else
            system("clear");
#endif
        }
        
    } while(opcion != 0);
}

void menuUsuarios(void) {
    int opcion;
    char correo[MAX_CORREO], nombre[MAX_NOMBRE], pais[MAX_PAIS];
    char nickname[MAX_NICKNAME], contrasena[MAX_CONTRASENA], plan[MAX_PLAN];
    Usuario* usuario;
    
    do {
        limpiarPantalla();
        dibujarUsuarioIcono();
        dibujarEncabezado("GESTION DE USUARIOS");
        printf(BLANCO " 1. " AMARILLO "Registrar nuevo usuario\n" RESET);
        printf(BLANCO " 2. " AMARILLO "Buscar usuario\n" RESET);
        printf(BLANCO " 3. " AMARILLO "Mostrar todos los usuarios\n" RESET);
        printf(BLANCO " 4. " AMARILLO "Eliminar usuario\n" RESET);
        printf(BLANCO " 5. " AMARILLO "Ver detalles de usuario\n" RESET);
        printf(BLANCO " 6. " AMARILLO "Modificar usuario\n" RESET);
        printf(BLANCO " 0. " ROJO "Volver\n" RESET);
        printf(BLANCO "\n Seleccione una opcion: " RESET);
        scanf("%d", &opcion);
        limpiarBuffer();
        
        switch(opcion) {
            case 1: {
                limpiarPantalla();
                dibujarEncabezado("REGISTRO MANUAL");
                Usuario* nuevo = crearUsuarioVacio();
                do {
                    leerTextoObligatorio("Correo: ", correo, sizeof(correo));

                    if (!correoValido(correo)) {
                    printf(ROJO " [!] Correo invalido. Debe contener @ y punto.\n" RESET);
                    }

                } while (!correoValido(correo));

                leerTextoObligatorio("Nombre: ", nombre, sizeof(nombre));
                leerTextoObligatorio("Pais: ", pais, sizeof(pais));
                leerTextoObligatorio("Nickname: ", nickname, sizeof(nickname));
                leerTextoObligatorio("Contrasena: ", contrasena, sizeof(contrasena));

                do {
                    leerTextoObligatorio("Plan (free/premium): ", plan, sizeof(plan));

                    if (strcmp(plan, "free") != 0 && strcmp(plan, "premium") != 0) {
                        printf(ROJO " [!] Plan invalido. Use free o premium.\n" RESET);
                    }

                } while (strcmp(plan, "free") != 0 && strcmp(plan, "premium") != 0);
                
                strcpy(nuevo->correo, correo);
                strcpy(nuevo->nombre, nombre);
                strcpy(nuevo->pais, pais);
                strcpy(nuevo->nickname, nickname);
                strcpy(nuevo->contrasena, contrasena);
                strcpy(nuevo->plan, plan);
                nuevo->premiumActivo = (strcmp(plan, "premium") == 0);
                nuevo->cancionesEscuchadasDesdeAnuncio = 0;
                nuevo->anunciosMostrados = 0;
                nuevo->tiempoTotalReproduccion = 0;
                nuevo->playlists = NULL;
                nuevo->historial = NULL;
                nuevo->amigos = NULL;
                nuevo->izq = NULL;
                nuevo->der = NULL;
                
                insertarUsuario(&raizUsuarios, nuevo);
                break;
            }
            case 2: {
                printf(CYAN "\n Nickname a buscar: " RESET);
                fgets(nickname, sizeof(nickname), stdin);
                nickname[strcspn(nickname, "\n")] = '\0';
                usuario = buscarUsuario(raizUsuarios, nickname);
                
                if (usuario != NULL) {
                    printf(VERDE "\n Usuario encontrado:\n" RESET);
                    mostrarUsuario(usuario);
                } else {
                    printf(ROJO "\n Usuario no encontrado.\n" RESET);
                }
                pausa();
                break;
            }
            case 3:
                limpiarPantalla();
                dibujarEncabezado("LISTADO TOTAL");
                mostrarUsuarios(raizUsuarios);
                pausa();
                break;
            case 4: {
                printf(CYAN "\n Nickname a ELIMINAR: " RESET);
                fgets(nickname, sizeof(nickname), stdin);
                nickname[strcspn(nickname, "\n")] = '\0';

                if (eliminarUsuarioSistema(&raizUsuarios, nickname)) {
                    printf(VERDE "\n [OK] Operacion completada.\n" RESET);
                } else {
                    printf(AMARILLO "\n [i] No se elimino ningun usuario.\n" RESET);
                }

                pausa();
                break;
            }
            case 5: {
                printf(CYAN "\n Nickname: " RESET);
                fgets(nickname, sizeof(nickname), stdin);
                nickname[strcspn(nickname, "\n")] = '\0';
                usuario = buscarUsuario(raizUsuarios, nickname);
                
                if (usuario != NULL) {
                    mostrarUsuarioDetallado(usuario);
                } else {
                    printf(ROJO "\n Usuario no encontrado.\n" RESET);
                }
                pausa();
                break;
            }
            case 6: {
                printf(CYAN "\n Nickname: " RESET);
                fgets(nickname, sizeof(nickname), stdin);
                nickname[strcspn(nickname, "\n")] = '\0';
                usuario = buscarUsuario(raizUsuarios, nickname);

                if (usuario != NULL) {
                    modificarUsuario(usuario);
                } else {
                    printf(ROJO "\n Usuario no encontrado.\n" RESET);
                }
                pausa();
                break;
            }
        }
    } while(opcion != 0);
}

void menuArtistas(void) {
    int opcion;
    char nombre[MAX_NOMBRE], nombreDisco[MAX_NOMBRE], fecha[MAX_FECHA];
    char nombreCancion[MAX_NOMBRE], archivo[MAX_ORIGEN];
    int duracion;
    Artista* artista;
    Disco* disco;

    do {
        limpiarPantalla();
        dibujarMusicaIcono();
        dibujarEncabezado("GESTION DE ARTISTAS");
        printf(BLANCO " 1. " AMARILLO "Agregar artista\n" RESET);
        printf(BLANCO " 2. " AMARILLO "Agregar disco a artista\n" RESET);
        printf(BLANCO " 3. " AMARILLO "Agregar cancion a disco\n" RESET);
        printf(BLANCO " 4. " AMARILLO "Mostrar todos los artistas\n" RESET);
        printf(BLANCO " 5. " AMARILLO "Eliminar artista\n" RESET);
        printf(BLANCO " 6. " AMARILLO "Buscar artista\n" RESET);
        printf(BLANCO " 7. " AMARILLO "Eliminar cancion\n" RESET);
        printf(BLANCO " 8. " AMARILLO "Modificar cancion\n" RESET);
        printf(BLANCO " 0. " ROJO "Volver\n" RESET);
        printf(BLANCO "Opcion: " RESET);
        scanf("%d", &opcion);
        limpiarBuffer();

        switch(opcion) {
            case 1:
                printf(CYAN "Nombre del artista: " RESET);
                fgets(nombre, sizeof(nombre), stdin);
                nombre[strcspn(nombre, "\n")] = '\0';
                if (strlen(nombre) > 0) {
                    insertarArtista(&raizArtistas, crearArtista(nombre));
                    printf(VERDE " [OK] Artista insertado.\n" RESET);
                }
                pausa();
                break;
            case 2:
                printf(CYAN "Nombre del artista: " RESET);
                fgets(nombre, sizeof(nombre), stdin);
                nombre[strcspn(nombre, "\n")] = '\0';
                artista = buscarArtista(raizArtistas, nombre);
                if (artista != NULL) {
                    printf(CYAN "Nombre del disco: " RESET);
                    fgets(nombreDisco, sizeof(nombreDisco), stdin);
                    nombreDisco[strcspn(nombreDisco, "\n")] = '\0';
                    do {
                        printf(CYAN "Fecha de lanzamiento (DD/MM/AAAA): " RESET);
                        fgets(fecha, sizeof(fecha), stdin);
                        fecha[strcspn(fecha, "\n")] = '\0';

                        if (!validarFecha(fecha)) {
                            printf(ROJO " [!] Fecha invalida. Use el formato DD/MM/AAAA.\n" RESET);
                            printf(AMARILLO "     Ejemplo valido: 23/12/2026\n" RESET);
                        }

                    } while (!validarFecha(fecha));

                    agregarDisco(artista, crearDisco(nombreDisco, fecha));
                    printf(VERDE " [OK] Disco agregado.\n" RESET);
                } else {
                    printf(ROJO " [!] Artista no encontrado.\n" RESET);
                }
                pausa();
                break;
            case 3:
                printf(CYAN "Nombre del artista: " RESET);
                fgets(nombre, sizeof(nombre), stdin);
                nombre[strcspn(nombre, "\n")] = '\0';
                artista = buscarArtista(raizArtistas, nombre);
                if (artista != NULL) {
                    printf(CYAN "Nombre del disco: " RESET);
                    fgets(nombreDisco, sizeof(nombreDisco), stdin);
                    nombreDisco[strcspn(nombreDisco, "\n")] = '\0';
                    disco = buscarDisco(artista, nombreDisco);
                    if (disco != NULL) {
                        leerTextoObligatorio(CYAN "Nombre de la cancion: " RESET, nombreCancion, sizeof(nombreCancion));
                        duracion = leerEnteroPositivo(CYAN "Duracion en segundos: " RESET);
                        leerTextoObligatorio(CYAN "Archivo de origen: " RESET, archivo, sizeof(archivo));
                        agregarCancion(disco, crearCancion(nombre, nombreCancion, duracion, archivo));
                        printf(VERDE " [OK] Cancion agregada.\n" RESET);
                    } else {
                        printf(ROJO " [!] Disco no encontrado.\n" RESET);
                    }
                } else {
                    printf(ROJO " [!] Artista no encontrado.\n" RESET);
                }
                pausa();
                break;
            case 4:
                limpiarPantalla();
                dibujarEncabezado("CATALOGO COMPLETO");
                mostrarTodosArtistasYDiscos(raizArtistas);
                pausa();
                break;
            case 5:
                printf(CYAN "Nombre del artista a eliminar: " RESET);
                fgets(nombre, sizeof(nombre), stdin);
                nombre[strcspn(nombre, "\n")] = '\0';
                raizArtistas = eliminarArtista(raizArtistas, nombre);
                printf(VERDE " [OK] Artista eliminado.\n" RESET);
                pausa();
                break;
            case 6:
                printf(CYAN "Nombre del artista: " RESET);
                fgets(nombre, sizeof(nombre), stdin);
                nombre[strcspn(nombre, "\n")] = '\0';
                artista = buscarArtista(raizArtistas, nombre);
                if (artista != NULL) {
                    mostrarTodosDiscos(artista);
                } else {
                    printf(ROJO " [!] Artista no encontrado.\n" RESET);
                }
                pausa();
                break;
            case 7:
                printf(CYAN "Nombre del artista: " RESET);
                fgets(nombre, sizeof(nombre), stdin);
                nombre[strcspn(nombre, "\n")] = '\0';

                printf(CYAN "Nombre de la cancion a eliminar: " RESET);
                fgets(nombreCancion, sizeof(nombreCancion), stdin);
                nombreCancion[strcspn(nombreCancion, "\n")] = '\0';

                if (eliminarCancionDeSistema(raizArtistas, nombre, nombreCancion)) {
                    printf(VERDE "\n[OK] Operacion completada.\n" RESET);
                } else {
                    printf(AMARILLO "\n[i] No se elimino ninguna cancion.\n" RESET);
                }

    pausa();
    break;
            case 8:
                printf(CYAN "Nombre del artista: " RESET);
                fgets(nombre, sizeof(nombre), stdin);
                nombre[strcspn(nombre, "\n")] = '\0';
                printf(CYAN "Nombre de la cancion a modificar: " RESET);
                fgets(nombreCancion, sizeof(nombreCancion), stdin);
                nombreCancion[strcspn(nombreCancion, "\n")] = '\0';
                modificarCancionSistema(raizArtistas, nombre, nombreCancion);
                pausa();
                break;
        }
    } while(opcion != 0);
}

void menuPlaylists(void) {
    int opcion;
    char nombre[MAX_NOMBRE];
    do {
        limpiarPantalla();
        printf(CYAN "      .---. \n     |  ≡  |  " VERDE "GESTION DE\n     |     |  " VERDE "PLAYLISTS\n      '---' \n" RESET);
        dibujarEncabezado("PLAYLISTS GLOBALES");

        printf(CYAN " 1. " AMARILLO "Crear nueva playlist\n" RESET);
        printf(CYAN " 2. " AMARILLO "Mostrar todas las playlists\n" RESET);
        printf(CYAN " 3. " ROJO "Eliminar playlist\n" RESET);
        printf(CYAN " 0. " GRIS_O "Volver\n" RESET);

        printf(CYAN "\n Opcion: " RESET);
        scanf("%d", &opcion);
        limpiarBuffer();

        switch(opcion) {
            case 1:
                printf(CYAN "Nombre de la playlist: " RESET);
                fgets(nombre, sizeof(nombre), stdin);
                nombre[strcspn(nombre, "\n")] = '\0';
                Playlist* nueva = crearPlaylist(nombre);
                if (nueva != NULL) {
                    nueva->sig = playlists;
                    playlists = nueva;
                    printf(VERDE " [OK] Playlist global creada.\n" RESET);
                }
                pausa();
                break;
            case 2:
                dibujarEncabezado("PLAYLISTS REGISTRADAS");
                mostrarTodasPlaylists(playlists);
                pausa();
                break;
            case 3:
                printf(CYAN "Nombre de la playlist a eliminar: " RESET);
                fgets(nombre, sizeof(nombre), stdin);
                nombre[strcspn(nombre, "\n")] = '\0';
                playlists = eliminarPlaylist(playlists, nombre);
                printf(VERDE " [OK] Playlist eliminada.\n" RESET);
                pausa();
                break;
        }
    } while(opcion != 0);
}

void menuAnuncios(void) {
    int opcion;
    char contenido[MAX_ANUNCIO];
    char anunciante[MAX_ANUNCIANTE];
    do {
        limpiarPantalla();
        printf(MAGENTA "      [!]  " VERDE "MODULO DE\n      [!]  " VERDE "PUBLICIDAD\n\n" RESET);
        dibujarEncabezado("GESTION DE ANUNCIOS");

        printf(BLANCO " 1. " AMARILLO "Crear nuevo anuncio\n" RESET);
        printf(BLANCO " 2. " AMARILLO "Ver cola de anuncios\n" RESET);
        printf(BLANCO " 0. " ROJO "Volver\n" RESET);

        printf(CYAN "\n Opcion: " RESET);
        scanf("%d", &opcion);
        limpiarBuffer();

        switch(opcion) {
            case 1:
                printf(CYAN "Contenido del anuncio: " RESET);
                fgets(contenido, sizeof(contenido), stdin);
                contenido[strcspn(contenido, "\n")] = '\0';
                printf(CYAN "Nombre del anunciante: " RESET);
                fgets(anunciante, sizeof(anunciante), stdin);
                anunciante[strcspn(anunciante, "\n")] = '\0';
                encolarAnuncio(colaAnuncios, contenido, anunciante);
                printf(VERDE " [OK] Anuncio agregado a la cola.\n" RESET);
                pausa();
                break;
            case 2:
                dibujarEncabezado("COLA DE ANUNCIOS");
                mostrarAnuncios(colaAnuncios);
                pausa();
                break;
        }
    } while(opcion != 0);
}

Usuario* pedirUsuarioParaReporte(void) {
    char nickname[MAX_NICKNAME];
    Usuario* usuario;

    printf(CYAN "\nIngrese nickname del usuario: " RESET);
    fgets(nickname, sizeof(nickname), stdin);
    nickname[strcspn(nickname, "\n")] = '\0';

    usuario = buscarUsuario(raizUsuarios, nickname);

    if (usuario == NULL) {
        printf(ROJO "\n [!] Usuario no encontrado.\n" RESET);
    }

    return usuario;
}

void menuReportes(void) {
    int opcion;
    Usuario* usuario;

    do {
        limpiarPantalla();
        printf(CYAN "      .---. \n");
        printf("     /     \\ \n");
        printf("    |  R R  |   " VERDE "CENTRO DE\n");
        printf("    |       |   " VERDE "REPORTES\n");
        printf(CYAN "     '---' \n" RESET);

        dibujarEncabezado("REPORTES DEL SISTEMA");

        printf(CYAN " 1. " AMARILLO "Usuarios Premium\n" RESET);
        printf(CYAN " 2. " AMARILLO "Top 5 Canciones\n" RESET);
        printf(CYAN " 3. " AMARILLO "Artista preferido por usuario\n" RESET);
        printf(CYAN " 4. " AMARILLO "Tiempo total por usuario\n" RESET);
        printf(CYAN " 5. " AMARILLO "Anuncios mostrados por usuario free\n" RESET);
        printf(CYAN " 6. " AMARILLO "Generar todos los reportes .txt\n" RESET);
        printf(CYAN " 0. " GRIS_O "Volver\n" RESET);

        printf(CYAN "\n Opcion: " RESET);

        if (scanf("%d", &opcion) != 1) {
            limpiarBuffer();
            opcion = -1;
        } else {
            limpiarBuffer();
        }

        switch(opcion) {
            case 1:
                limpiarPantalla();
                dibujarEncabezado("USUARIOS PREMIUM");
                mostrarUsuariosPremium(raizUsuarios);
                pausa();
                break;

            case 2:
                reporteTopCanciones(raizArtistas);
                pausa();
                break;

            case 3:
                limpiarPantalla();
                dibujarEncabezado("ARTISTA PREFERIDO POR USUARIO");
                usuario = pedirUsuarioParaReporte();

                if (usuario != NULL) {
                    reporteArtistasPreferidos(usuario);
                }

                pausa();
                break;

            case 4:
                limpiarPantalla();
                dibujarEncabezado("TIEMPO TOTAL POR USUARIO");
                usuario = pedirUsuarioParaReporte();

                if (usuario != NULL) {
                    reporteTiempoTotal(usuario);
                }

                pausa();
                break;

            case 5:
                limpiarPantalla();
                dibujarEncabezado("ANUNCIOS USUARIO FREE");
                usuario = pedirUsuarioParaReporte();

                if (usuario != NULL) {
                    if (usuario->premiumActivo) {
                        printf(AMARILLO "\nEste usuario es premium. No tiene reporte de anuncios free.\n" RESET);
                    } else {
                        reporteAnunciosFree(usuario);
                    }
                }

                pausa();
                break;

            case 6:
                limpiarPantalla();
                dibujarEncabezado("GENERAR TODOS LOS REPORTES");
                generarReportes(raizUsuarios, raizArtistas);
                pausa();
                break;

            case 0:
                printf(GRIS "\nVolviendo al menu anterior...\n" RESET);
                break;

            default:
                printf(ROJO "\nOpcion invalida.\n" RESET);
                pausa();
        }

    } while(opcion != 0);
}

////OJOOOOOOOOOO

void reproducirAudioAnuncio(void) {
    MCIERROR errorAudio;
    char mensajeError[256];

    mciSendString("close anuncio_alias", NULL, 0, NULL);

    errorAudio = mciSendString("open \"anuncio.mp3\" type mpegvideo alias anuncio_alias", NULL, 0, NULL);

    if (errorAudio != 0) {
        mciGetErrorString(errorAudio, mensajeError, 256);
        printf(ROJO "\n[DEBUG] No se pudo abrir anuncio.mp3: %s\n" RESET, mensajeError);
        printf(AMARILLO "El anuncio se mostrara solo como texto.\n" RESET);

        Sleep(3000);
        return;
    }

    printf(AMARILLO "\nReproduciendo anuncio...\n" RESET);

    errorAudio = mciSendString("play anuncio_alias wait", NULL, 0, NULL);

    if (errorAudio != 0) {
        mciGetErrorString(errorAudio, mensajeError, 256);
        printf(ROJO "\n[DEBUG] Error al reproducir anuncio: %s\n" RESET, mensajeError);
    }

    mciSendString("close anuncio_alias", NULL, 0, NULL);
}

//ojooooooo
int reproducirAudioCancion(Cancion* cancion) {
    MCIERROR errorAudio;
    char mensajeError[256];
    char comando[400];
    char rutaAbsoluta[MAX_PATH];
    FILE* archivo;

    if (cancion == NULL) {
        return 0;
    }

    if (strlen(cancion->archivoOrigen) == 0) {
        printf(ROJO "\n[!] Esta cancion no tiene archivo de origen.\n" RESET);
        return 0;
    }

    GetFullPathNameA(cancion->archivoOrigen, MAX_PATH, rutaAbsoluta, NULL);

    archivo = fopen(rutaAbsoluta, "rb");

    if (archivo == NULL) {
        printf(ROJO "\n[!] No se encontro el archivo de audio.\n" RESET);
        printf(AMARILLO "Archivo guardado: %s\n" RESET, cancion->archivoOrigen);
        printf(AMARILLO "Ruta buscada: %s\n" RESET, rutaAbsoluta);
        return 0;
    }

    fclose(archivo);

    mciSendString("close musica_alias", NULL, 0, NULL);

    sprintf(comando, "open \"%s\" type mpegvideo alias musica_alias", rutaAbsoluta);

    errorAudio = mciSendString(comando, NULL, 0, NULL);

    if (errorAudio != 0) {
        mciGetErrorString(errorAudio, mensajeError, 256);
        printf(ROJO "\n[DEBUG] Error al abrir audio: %s\n" RESET, mensajeError);
        return 0;
    }

    errorAudio = mciSendString("play musica_alias", NULL, 0, NULL);

    if (errorAudio != 0) {
        mciGetErrorString(errorAudio, mensajeError, 256);
        printf(ROJO "\n[DEBUG] Error al reproducir audio: %s\n" RESET, mensajeError);
        return 0;
    }

    return 1;
}
///////////////////////////




void reproducirMusica(Usuario* usuario) {
    int opcionCancion;
    char mensajeError[256];
    char comando[300];
    MCIERROR errorAudio;

    limpiarPantalla();
    dibujarEncabezado("REPRODUCTOR DE MUSICA");

    if (raizArtistas == NULL) {
        printf(ROJO "\nNo hay artistas ni canciones registradas.\n" RESET);
        pausa();
        return;
    }

    mostrarCancionesSistema(raizArtistas);

    printf(CYAN "\nSeleccione el numero de la cancion: " RESET);
    if (scanf("%d", &opcionCancion) != 1) {
        limpiarBuffer();
        printf(ROJO "\nOpcion invalida.\n" RESET);
        pausa();
        return;
    }
    limpiarBuffer();

    Cancion* cancion = seleccionarCancionPorNumero(raizArtistas, opcionCancion);

    if (cancion == NULL) {
        printf(ROJO "\nCancion no encontrada.\n" RESET);
        pausa();
        return;
    }

    if (!usuario->premiumActivo) {
        usuario->cancionesEscuchadasDesdeAnuncio++;

        if (usuario->cancionesEscuchadasDesdeAnuncio >= 3) {
            printf(MAGENTA "\n [!] ESPACIO PUBLICITARIO:\n" RESET);
            mostrarAnuncioActual(colaAnuncios);

            usuario->anunciosMostrados++;
            usuario->cancionesEscuchadasDesdeAnuncio = 0;

            printf(AMARILLO "\nUsuario Free: debe escuchar un anuncio cada 3 canciones.\n" RESET);

            reproducirAudioAnuncio();

            printf(VERDE "\nAnuncio finalizado. Continuando con la cancion...\n" RESET);
            Sleep(1000);
        }
    }

    printf(VERDE "\n >> Reproduciendo: " RESET "%s " GRIS "por" RESET " %s\n",
           cancion->nombre,
           cancion->artista);

    printf(GRIS " Archivo de origen: %s\n" RESET, cancion->archivoOrigen);

    mciSendString("close musica_alias", NULL, 0, NULL);


    if (!reproducirAudioCancion(cancion)) {
        printf(AMARILLO "\nNo se pudo abrir el audio, pero se registrara la reproduccion como simulada.\n" RESET);
    }
 



    cancion->reproducciones++;
    usuario->tiempoTotalReproduccion += cancion->duracionSegundos;
    registrarEnHistorial(usuario, cancion);

    printf(VERDE "\nReproduccion registrada correctamente.\n" RESET);
    printf(GRIS "Reproducciones de esta cancion: %d\n" RESET, cancion->reproducciones);
    printf(GRIS "Tiempo total del usuario: %d segundos\n" RESET, usuario->tiempoTotalReproduccion);

    printf(AMARILLO "\nPresiona ENTER para detener/continuar...\n" RESET);
    pausa();

    mciSendString("close musica_alias", NULL, 0, NULL);
}

void comprarPremium(Usuario* usuario) {
    int opcion;
    time_t t;
    struct tm* tm;

    if (usuario == NULL) {
        return;
    }

    limpiarPantalla();
    dibujarEncabezado("PLAN PREMIUM");

    printf(CYAN " Usuario: " RESET "%s\n", usuario->nickname);
    printf(CYAN " Plan actual: " RESET "%s\n", usuario->plan);
    printf(CYAN " Estado Premium: " RESET "%s\n", usuario->premiumActivo ? "Activo" : "Inactivo");

    if (usuario->premiumActivo) {
        printf(CYAN " Vence: " RESET "%s\n", usuario->fechaVencimientoPremium);
    }

    printf("\n");
    printf(AMARILLO " Valor del plan: $9.99\n" RESET);
    printf(AMARILLO " Validez: 1 mes\n" RESET);

    if (usuario->premiumActivo) {
        printf("\n1. Renovar Premium por 1 mes\n");
        printf("0. Cancelar\n");
    } else {
        printf("\n1. Comprar Premium por 1 mes\n");
        printf("0. Cancelar\n");
    }

    printf(CYAN "\nOpcion: " RESET);

    if (scanf("%d", &opcion) != 1) {
        limpiarBuffer();
        printf(ROJO "\nOpcion invalida.\n" RESET);
        return;
    }

    limpiarBuffer();

    if (opcion == 0) {
        printf(AMARILLO "\nOperacion cancelada.\n" RESET);
        return;
    }

    if (opcion != 1) {
        printf(ROJO "\nOpcion invalida.\n" RESET);
        return;
    }

    t = time(NULL);
    tm = localtime(&t);

    /*
       Por simplicidad, la nueva fecha se calcula desde hoy.
       En la sustentacion puedes decir:
       - Si es free: compra premium desde hoy.
       - Si ya es premium: renueva el plan por un mes adicional.
    */
    tm->tm_mon += 1;

    strcpy(usuario->plan, "premium");
    usuario->premiumActivo = 1;
    usuario->valorPremium += 9.99;
    strftime(usuario->fechaVencimientoPremium, MAX_FECHA, "%Y-%m-%d", tm);

    printf(VERDE "\n[OK] Operacion realizada correctamente.\n" RESET);
    printf(CYAN "Plan: " RESET "%s\n", usuario->plan);
    printf(CYAN "Premium activo hasta: " RESET "%s\n", usuario->fechaVencimientoPremium);
    printf(CYAN "Valor acumulado pagado: " RESET "$%.2f\n", usuario->valorPremium);
}

void inicializarDatosPrueba(void) {
    if (colaAnuncios == NULL) {
        colaAnuncios = crearColaAnuncios();
        encolarAnuncio(colaAnuncios, "Compra el nuevo celular X", "TechCorp");
        encolarAnuncio(colaAnuncios, "No te pierdas el concierto de ARTISTA Y", "Ticketmaster");
        encolarAnuncio(colaAnuncios, "Descubre nuevos artistas", "Spotify");
    }
    printf(VERDE " [OK] Entorno de prueba configurado.\n" RESET);
}

int contarArtistas(Artista* raiz) {
    if (raiz == NULL) return 0;
    return 1 + contarArtistas(raiz->izq) + contarArtistas(raiz->der);
}

int contarPlaylists(Playlist* p) {
    int count = 0;
    while (p != NULL) {
        count++;
        p = p->sig;
    }
    return count;
}

int contarUsuarios(Usuario* raiz) {
    if (raiz == NULL) return 0;
    return 1 + contarUsuarios(raiz->izq) + contarUsuarios(raiz->der);
}

int contarAnuncios(ColaAnuncios* cola) {
    if (cola == NULL || cola->frente == NULL) return 0;
    int count = 0;
    Anuncio* aux = cola->frente;
    while (aux != NULL) {
        count++;
        aux = aux->sig;
    }
    return count;
}

void mostrarEstadisticas(void) {
    limpiarPantalla();
    dibujarEncabezado("PANEL DE CONTROL");
    printf(BLANCO " =========================================\n" RESET);
    printf(BLANCO " | " AMARILLO "Usuarios:" RESET " %20d |\n", contarUsuarios(raizUsuarios));
    printf(BLANCO " | " AMARILLO "Artistas:" RESET " %20d |\n", contarArtistas(raizArtistas));
    printf(BLANCO " | " AMARILLO "Playlists:" RESET " %19d |\n", contarPlaylists(playlists));
    printf(BLANCO " | " AMARILLO "Anuncios:" RESET " %20d |\n", contarAnuncios(colaAnuncios));
    printf(BLANCO " =========================================\n" RESET);
    
    printf(GRIS "\n [i] Datos actualizados en tiempo real.\n" RESET);
}

void mostrarEstadisticasRapidas(void) {
    printf(GRIS "\n --- Resumen del Sistema ---\n" RESET);
    printf(GRIS " Usuarios: " RESET "%d" GRIS " | Artistas: " RESET "%d\n", 
           contarUsuarios(raizUsuarios), contarArtistas(raizArtistas));
}