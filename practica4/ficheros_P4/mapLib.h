#ifndef MAPLIB_H
#define MAPLIB_H

#include "controlVelocidad.c"

// Número máximo de celdas que se pueden utilizar.
#define MAX_X 10
#define MAX_Y 10

// Variables para dibujar la cuadrícula.
TFileIOResult nIoCuadricula;
TFileHandle hFileHandleCuad = 0;
short nFileSizeCuad = 20000;			// Tamaño del fichero.
string cuadricula = "grid.txt";     // Nombre del fichero.

// Variables globales de las dimensiones del mapa
int sizeX;			// Tamaño en el eje X.
int sizeY;			// Tamaño en el eje Y.
int sizeCell;		// Tamaño de la celda.
int pixPerX;		// Píxeles por eje X.
int pixPerY;		// Píxeles por eje Y.

// Coordenadas de la celda final.
int celdaXFin;
int celdaYFin;

// Matriz de conexiones.
bool connectionsMatrix[2*MAX_X+1][2*MAX_Y+1];

// Matrices con los caminos.
int pathX[MAX_X*MAX_Y];
int pathY[MAX_X*MAX_Y];

// Cuadrícula para encontrar el camino.
int grid[MAX_X*2][MAX_Y*2];

/*
 * Método que inicializa todas las conexiones a falso.
 */
void initConnections();

/*
* Método que dada una celda y su vecino da con la coordenada del punto
* en la matriz de conexiones.
*/
void cell2connCoord(int cellX, int cellY, int numNeigh, int & connX, int & connY);

/*
 * Marca una cierta celda como conectada a otra.
 */
void setConnection(int cellX, int cellY, int numNeigh);

/*
 * Método que borra la conexión entre dos celdas.
 */
void deleteConnection(int cellX, int cellY, int numNeigh);

/*
 * Método que devuelve true si y sólo si dos celdas dadas están conectadas.
 */
bool isConnected(int cellX, int cellY, int numNeigh);

/*
* Método que lee el mapa del fichero.
*/
bool readLineHeader(TFileHandle hFileHandle,TFileIOResult nIoResult, int & dimX, int & dimY, int &dimCell);

/*
* Método que lee la siguiente línea del fichero.
*/
bool readNextLine(TFileHandle hFileHandle,TFileIOResult & nIoResult, int & mapRow);

/*
* Método que carga el mapa desde un fichero.
*/
bool loadMap(string mapFileName);

/*
 * Método que dibuja el mapa por pantalla.
 */
void drawMap();

/*
 * Método que convierte las coordenadas del robot en odometría en coordenadas
 * de la celda.
 */
void pos2cell(float x_mm, float y_mm, int & x_cell, int & y_cell);

/*
 * Método que dibuja el robot en la pantalla.
 */
void drawRobot(float x_mm, float y_mm, float ang_rad);

/*
 * Método que va asignando costes recursivamente a la cuadrícula.
 */
void planPath(int coordenadaX, int coordenadaY, int coste);

/*
 * Método que asigna el valor -1 a obstáculos.
 */
void asignarValores();

/*
* Método que inicializa la cuadrícula con valor -2.
*/
void iniciarGrid();

/*
 * Método que inicializa el algoritmo NF1 para ir dando valores.
 */
void planPath(int x_ini, int y_ini, int x_end, int y_end);

/*
 * Método que encuentra el camino desde la posición inicial.
 */
void encontrarCamino(int x_ini, int y_ini);

/*
* Método que replanifica la trayectoria al detectar un obstáculo.
*/
void rePlanPath(int celdaX, int celdaY);

/*
* Método que redondea las coordenadas por si el robot se ha desviado.
*/
int redondearCoord(float coord);

/*
* Método que redondea el ángulo para dejarlo igual al eje más cercano.
*/
float redondearAng(float angulo);

/*
* Método que detecta si hay un obstáculo en la trayectoria.
*/
bool detectObstacle(float theta);

/*
* Método que planifica el camino desde la posición actual hasta una celda
* dada.
*/
bool go(int cellX, int cellY);

/*
* Método que recorre el camino hasta llegar al objetivo.
*/
void recorrerCamino();


#endif
