# Juego de Laberinto e Inventario GraphQuest
Este Proyecto es una aplicación interactiva en consola que carga un laberinto desde un archivo CSV y lo modela mediante un TDA Grafo Explícito, permitiendo al jugador recorrer los distintos escenarios, gestionar su inventario de ítems (considerando su peso y valor) y tomar decisiones estratégicas de ruta para llegar a la salida antes de que se agote el tiempo. 

## 1. Cómo compilar y ejecutar la tarea

El proyecto depende de los archivos genéricos de la estructura de datos ubicados en la carpeta tdas/. Asegúrese de tener GCC instalado en su sistema y el archivo graphquest.csv en la carpeta raíz del proyecto.

**Para compilar**
Abre tu terminal en el directorio raíz del proyecto (donde se encuentra 'tarea3.c') y ejecuta el siguiente comando:
```bash
gcc tdas/*.c tarea3.c -Wno-unused-result -o tarea3

```

**Para ejecutar**
Una vez finalizada la compilación, inicie la aplicación según su sistema operativo: 

### En Linux o Mac
```bash
./tarea3
```

### En Windows
```bash
tarea3.exe
```


## 2. Opciones y estado de las funcionalidades

Todas las funcionalidades requeridas operan correctamente y manejan la memoria de manera eficiente. No hay opciones defectuosas.

- **Cargar Laberinto desde Archivo CSV**: Funciona correctamente leyendo el archivo graphquest.csv, separando los múltiples ítems de cada habitación e indexando cada escenario en un Mapa en 'O(1)' durante la carga para luego construir el grafo explícito enlazando directamente los punteros de adyacencia (arriba, abajo, izquierda, derecha).

- **Iniciar Partida y Visualización de Estado**: Inicia el recorrido desde la "Entrada principal" (ID 1) y muestra en cada turno la descripción del escenario, los ítems disponibles en el lugar, el tiempo restante, el peso y puntaje acumulado del inventario, y las direcciones válidas hacia donde avanzar.

- **Recoger Ítem(s)**: Permite seleccionar uno o más ítems disponibles en el escenario actual y transferirlos a la lista del inventario del jugador, actualizando automáticamente el peso total y el puntaje acumulado, y descontando 1 unidad de tiempo por la acción.

- **Descartar Ítem(s)**: Permite eliminar uno o más ítems del inventario del jugador para reducir el peso transportado y moverse más rápido, liberando correctamente la memoria y descontando 1 unidad de tiempo.

- **Avanzar en una Dirección**: Mueve al jugador a través de los punteros del grafo hacia el escenario vecino seleccionado y descuenta el tiempo de forma dinámica según el peso del inventario aplicando la fórmula **[(Peso total + 1) / 10]**. También detecta automáticamente si el jugador alcanzó la Salida (victoria) o si se quedó sin tiempo (derrota).

- **Reiniciar Partida**: Restaura todos los ítems de los escenarios a su estado original en memoria sin necesidad de recargar el CSV, vacía el inventario del jugador y restablece el tiempo inicial en 10.

## 3. Ejemplo de interacción.

1. Se inicia la aplicación e ingresa al Menu Principal.
2. Se selecciona la **Opción 1**. La consola pide el nombre del archivo CSV, al presionar 'Enter' (o escribir 'graphquest.csv'), el programa construye le grafo del laberinto en memoria.
3. Se seleccciona la **Opcion 2** para iniciar la partida en el escenario '[1] Entrada principal' con 10 unidades de tiempo.
4. En el menú del jugador, se selecciona la **Opción 3** (Avanzar) y se elige ir hacia 'Abajo' para entrar a '[5] Cocina' (consume 1 unidad de tiempo).
5. Se selecciona la **Opción 1** (Recoger Ítem) para guardar el 'Cuchillo' (3 pts, 1 kg) en el inventario para luego apretar '0' para terminar de recoger (consume 1 unidad de tiempo).
6. Se continua avanzando estratégicamente por los escenarios ('Jardín' -> 'Dormitorio' -> 'Corredor' -> 'Cámara secreta' -> 'Salida') recogiendo ítems de alto valor como el 'Rubí' (20 pts) y el 'Mapa' (10 pts).
7. Al llegar al escenario '[16] Salida', el programa muestra un resumen del inventario final, el peso total transportado y el puntaje total acumulado. Finalmente, se selecciona la **Opción 3** en el menu principal y el programa libera toda la memoria y finaliza limoiamente.

## 4. Contribuciones
* Trabajo realizado de manera individual por **Nicolás Ignacio Garrido Vergara**, abarcando la totalidad de los módulos del proyecto:
* **Diseño del TDA Grafo Explícito y Estructuras**
* **Lectura y Parseo del CSV**
* **Lógica del Juego y Gestión del Inventario**
* **Gestión de Memoria y Reinicio Eficiente**
* **Documentación y Pruebas:** Redaccion del archivo 'README.md' y verificación de casos.
