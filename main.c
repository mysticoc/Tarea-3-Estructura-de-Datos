#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "tdas/list.h"
#include "tdas/extra.h"

#define TIEMPO_INICIAL 10

typedef struct {
  char nombre[100];
  int valor;
  intr peso;
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