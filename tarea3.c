#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "tdas/list.h"
#include "tdas/extra.h"
#include "tdas/map.h"

#define TIEMPO_INICIAL 10

typedef struct {
  char nombre[100];
  int valor;
  int peso;
} Item;

typedef struct Escenario {
  int id;
  char nombre[100];
  char descripcion[300];
  List *items_originales;
  List *items_disponibles;
  int id_arriba;
  int id_abajo;
  int id_izquierda;
  int id_derecha;
  struct Escenario *arriba;
  struct Escenario *abajo;
  struct Escenario *izquierda;
  struct Escenario *derecha;
  bool es_final;
} Escenario;

typedef struct {
  Escenario *escenario_actual;
  List *inventario;
  int peso_total;
  int puntaje_acumulado;
  int tiempo_restante;
} Jugador;

int int_equal(void *key1, void *key2) {
  return (*(int *)key1 == *(int *)key2);
}

Item *clonar_item(Item *original) {
  Item *nuevo = (Item *)malloc(sizeof(Item));
  if (nuevo != NULL) {
    strcpy(nuevo->nombre, original->nombre);
    nuevo->valor = original->valor;
    nuevo->peso = original->peso;
  }
  return nuevo;
}

void limpiar_buffer() {
  int c;
  while ((c = getchar()) != '\n' && c != EOF);
}

bool es_afirmativo(const char *texto) {
  if (texto == NULL) return false;
  return (strstr(texto, "Si") != NULL || strstr(texto, "Sí") != NULL ||
          strstr(texto, "si") != NULL || strstr(texto, "sí") != NULL || 
          strcmp(texto, "1") == 0);
}

void restaurar_escenarios(Map *grafo) {
  MapPair *pair = map_first(grafo);
  while (pair != NULL) {
    Escenario *esc = (Escenario *)pair->value;
    Item *it = (Item *)list_first(esc->items_disponibles);
    while (it != NULL) {
      free(it);
      it = (Item *)list_next(esc->items_disponibles);
    }
    list_clean(esc->items_disponibles);
    Item *orig = (Item *)list_first(esc->items_originales);
    while (orig != NULL) {
      list_pushBack(esc->items_disponibles, clonar_item(orig));
      orig = (Item *)list_next(esc->items_originales);
    }
    pair = map_next(grafo);
  }
}

void liberar_grafo(Map *grafo) {
  if (grafo == NULL) return;
  MapPair *pair = map_first(grafo);
  while (pair != NULL) {
    Escenario *esc = (Escenario *)pair->value;
    Item *it = (Item *)list_first(esc->items_originales);
    while (it != NULL) {
      free(it);
      it = (Item *)list_next(esc->items_originales);
    }
    list_clean(esc->items_originales);
    free(esc->items_originales);

    it = (Item *)list_first(esc->items_disponibles);
    while (it != NULL) {
      free(it);
      it = (Item *)list_next(esc->items_disponibles);
    }
    list_clean(esc->items_disponibles);
    free(esc->items_disponibles);
    free(pair->key);
    free(esc);
    pair = map_next(grafo);
  } 
  map_clean(grafo);
  free(grafo);
}

void limpiar_inventario(Jugador *jugador) {
  Item *it = (Item *)list_first(jugador->inventario);
  while (it != NULL) {
    free(it);
    it = (Item *)list_next(jugador->inventario);
  }
  list_clean(jugador->inventario);
  jugador->peso_total = 0;
  jugador->puntaje_acumulado = 0;
}

Map *cargar_laberinto(const char *nombre_archivo) {
  FILE *archivo = fopen(nombre_archivo, "r");
  if (archivo == NULL) {
    printf("\n[Error] No se pudo abrir el archivo '%s'. Verifique que exista.\n", nombre_archivo);
    return NULL;
  }
  Map *grafo = map_create(int_equal);
  char **campos;

  campos = leer_linea_csv(archivo,  ',');
  if (campos == NULL) {
    fclose(archivo);
    free(grafo);
    return NULL;
  }
  while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
    Escenario *nuevo = (Escenario *)malloc(sizeof(Escenario));
    nuevo->id = atoi(campos[0]);
    strcpy(nuevo->nombre, campos[1]);
    strcpy(nuevo->descripcion, campos[2]);

    nuevo->items_originales = list_create();
    nuevo->items_disponibles = list_create();

    if (strlen(campos[3]) > 0) {
      List *lista_raw_items = split_string(campos[3], ";");
      char *item_str = (char *)list_first(lista_raw_items);
      while (item_str != NULL) {
        List *datos_item = split_string(item_str, ",");
        char *nom = (char *)list_first(datos_item);
        char *val_str = (char *)list_next(datos_item);
        char *peso_str = (char *)list_next(datos_item);

        if (nom && val_str && peso_str) {
          Item *item = (Item *)malloc(sizeof(Item));

          while (*nom == ' ') nom++;
          strcpy(item->nombre, nom);
          item->valor = atoi(val_str);
          item->peso = atoi(peso_str);

          list_pushBack(nuevo->items_originales, item);
          list_pushBack(nuevo->items_disponibles, clonar_item(item));
        }
        char *aux = (char *)list_first(datos_item);
        while (aux != NULL) {
          free(aux);
          aux = (char *)list_next(datos_item);
        }
        list_clean(datos_item);
        free(datos_item);
        item_str = (char *)list_next(lista_raw_items);
      }
      char *aux_raw = (char *)list_first(lista_raw_items);
      while (aux_raw != NULL) {
        free(aux_raw);
        aux_raw = (char *)list_next(lista_raw_items);
      }
      list_clean(lista_raw_items);
      free(lista_raw_items);
    }
    nuevo->id_arriba = atoi(campos[4]);
    nuevo->id_abajo = atoi(campos[5]);
    nuevo->id_izquierda = atoi(campos[6]);
    nuevo->id_derecha = atoi(campos[7]);

    nuevo->arriba = NULL;
    nuevo->abajo = NULL;
    nuevo->izquierda = NULL;
    nuevo->derecha = NULL;

    nuevo->es_final = es_afirmativo(campos[8]);
    
    int *clave_id = (int *)malloc(sizeof(int));
    *clave_id = nuevo->id;
    map_insert(grafo, clave_id, nuevo);
  }
  fclose(archivo);

  MapPair *pair = map_first(grafo);
  while (pair != NULL) {
    Escenario *actual = (Escenario *)pair->value;

    if (actual->id_arriba != -1) {
      MapPair *vecino = map_search(grafo, &actual->id_arriba);
      if (vecino) actual->arriba = (Escenario *)vecino->value;
    }
    if (actual->id_abajo != -1) {
      MapPair *vecino = map_search(grafo, &actual->id_abajo);
      if (vecino) actual->abajo = (Escenario *)vecino->value;
    }
    if (actual->id_izquierda != -1) {
      MapPair *vecino = map_search(grafo, &actual->id_izquierda);
      if (vecino) actual->izquierda = (Escenario *)vecino->value;
    }
    if (actual->id_derecha != -1) {
      MapPair *vecino = map_search(grafo, &actual->id_derecha);
      if (vecino) actual->derecha = (Escenario *)vecino->value;
    }
    pair = map_next(grafo);
  }
  return grafo;
}

void mostrar_estado(Jugador *jugador) {
  Escenario *esc = jugador->escenario_actual;
  printf("\n============================================================\n");
  printf(" ESCENARIO ACTUAL: [%d] %s\n", esc->id, esc->nombre);
  printf("============================================================\n");
  printf("Descripción: %s\n\n", esc->descripcion);
  printf("--- Ítems en este escenario ---\n");
  Item *it = (Item *)list_first(esc->items_disponibles);
  if (it == NULL) {
      printf("  (No hay ítems disponibles aquí)\n");
  } else {
      int idx = 1;
      while (it != NULL) {
          printf("  %d. %s (Valor: %d pts | Peso: %d kg)\n", idx++, it->nombre, it->valor, it->peso);
          it = (Item *)list_next(esc->items_disponibles);
      }
  }
  printf("\n--- Estado del Jugador ---\n");
  printf("  Tiempo restante   : %d\n", jugador->tiempo_restante);
  printf("  Peso total        : %d kg\n", jugador->peso_total);
  printf("  Puntaje acumulado : %d pts\n", jugador->puntaje_acumulado);
  printf("  Inventario        :\n");

  Item *inv_it = (Item *)list_first(jugador->inventario);
  if (inv_it == NULL) {
      printf("    (Inventario vacío)\n");
  } else {
      int idx = 1;
      while (inv_it != NULL) {
          printf("    %d. %s (%d pts, %d kg)\n", idx++, inv_it->nombre, inv_it->valor, inv_it->peso);
          inv_it = (Item *)list_next(jugador->inventario);
      }
  }
  printf("\n--- Direcciones Disponibles ---\n");
  if (esc->arriba)    printf("  [Arriba]    -> %s (ID: %d)\n", esc->arriba->nombre, esc->arriba->id);
  if (esc->abajo)     printf("  [Abajo]     -> %s (ID: %d)\n", esc->abajo->nombre, esc->abajo->id);
  if (esc->izquierda) printf("  [Izquierda] -> %s (ID: %d)\n", esc->izquierda->nombre, esc->izquierda->id);
  if (esc->derecha)   printf("  [Derecha]   -> %s (ID: %d)\n", esc->derecha->nombre, esc->derecha->id);
  printf("============================================================\n");
}





int main() {
  Map *grafo_laberinto = NULL;
  int opcion = 0;

  do {
    printf("\n============================================\n");
    printf("         BIENVENIDO A GRAPHQUEST            \n");
    printf("============================================\n");
    printf("1. Cargar Laberinto desde Archivo CSV\n");
    printf("2. Iniciar Partida\n");
    printf("3. Salir del Juego\n");
    printf("============================================\n");
    printf("Seleccione una opción: ");

    if (scanf("%d", &opcion) != 1) {
      limpiar_buffer();
      printf("\n[!] Entrada inválida. Ingrese un número del 1 al 3.\n");
      continue;
    }
    limpiar_buffer();
    switch (opcion) {
      case 1: {
      char ruta_archivo[256];
          printf("\nIngrese el nombre del archivo CSV (presione Enter para usar 'graphquest.csv'): ");
          if (fgets(ruta_archivo, sizeof(ruta_archivo), stdin) != NULL) {
              ruta_archivo[strcspn(ruta_archivo, "\r\n")] = '\0';
              if (strlen(ruta_archivo) == 0) {
                  strcpy(ruta_archivo, "graphquest.csv");
              }
          } else {
              strcpy(ruta_archivo, "graphquest.csv");
          }

          
          if (grafo_laberinto != NULL) {
              liberar_grafo(grafo_laberinto);
          }

          grafo_laberinto = cargar_laberinto(ruta_archivo);
          if (grafo_laberinto != NULL) {
              printf("\n[Éxito] Laberinto cargado correctamente desde '%s'.\n", ruta_archivo);
          }
          break;
      }
      default:
        printf("\n[!] Opción fuera de rango. Intente nuevamente.\n");
        break;
    }
  } while (opcion != 3);
  if (grafo_laberinto != NULL) {
    liberar_grafo(grafo_laberinto);
  }
  return 0;
}