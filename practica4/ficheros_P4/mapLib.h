#ifndef MAPLIB_H
#define MAPLIB_H

#include "controlVelocidad.c"

// N�mero m�ximo de celdas que se pueden utilizar.
#define MAX_X 10
#define MAX_Y 10

// Variables para dibujar la cuadr�cula.
TFileIOResult nIoCuadricula;
TFileHandle hFileHandleCuad = 0;
short nFileSizeCuad = 20000;			// Tama�o del fichero.
string cuadricula = "grid.txt";     // Nombre del fichero.

// Variables globales de las dimensiones del mapa
int sizeX;			// Tama�o en el eje X.
int sizeY;			// Tama�o en el eje Y.
int sizeCell;		// Tama�o de la celda.
int pixPerX;		// P�xeles por eje X.
int pixPerY;		// P�xeles por eje Y.

// Coordenadas de la celda final.
int celdaXFin;
int celdaYFin;

// Matriz de conexiones.
bool connectionsMatrix[2*MAX_X+1][2*MAX_Y+1];

// Matrices con los caminos.
int pathX[MAX_X*MAX_Y];
int pathY[MAX_X*MAX_Y];

// Cuadr�cula para encontrar el camino.
int grid[MAX_X*2][MAX_Y*2];

/*
 * M�todo que inicializa todas las conexiones a falso.
 */
void initConnections();

/*
* M�todo que dada una celda y su vecino da con la coordenada del punto
* en la matriz de conexiones.
*/
void cell2connCoord(int cellX, int cellY, int numNeigh, int & connX, int & connY);

/*
 * Marca una cierta celda como conectada a otra.
 */
void setConnection(int cellX, int cellY, int numNeigh);

/*
 * M�todo que borra la conexi�n entre dos celdas.
 */
void deleteConnection(int cellX, int cellY, int numNeigh);

/*
 * M�todo que devuelve true si y s�lo si dos celdas dadas est�n conectadas.
 */
bool isConnected(int cellX, int cellY, int numNeigh);

/*
* M�todo que lee el mapa del fichero.
*/
bool readLineHeader(TFileHandle hFileHandle,TFileIOResult nIoResult, int & dimX, int & dimY, int &dimCell);

/*
* M�todo que lee la siguiente l�nea del fichero.
*/
bool readNextLine(TFileHandle hFileHandle,TFileIOResult & nIoResult, int & mapRow);

/*
* M�todo que carga el mapa desde un fichero.
*/
bool loadMap(string mapFileName);

/*
 * M�todo que dibuja el mapa por pantalla.
 */
void drawMap();

/*
 * M�todo que convierte las coordenadas del robot en odometr�a en coordenadas
 * de la celda.
 */
void pos2cell(float x_mm, float y_mm, int & x_cell, int & y_cell);

/*
 * M�todo que dibuja el robot en la pantalla.
 */
void drawRobot(float x_mm, float y_mm, float ang_rad);

/*
 * M�todo que va asignando costes recursivamente a la cuadr�cula.
 */
void planPath(int coordenadaX, int coordenadaY, int coste);

/*
 * M�todo que asigna el valor -1 a obst�culos.
 */
void asignarValores();

/*
* M�todo que inicializa la cuadr�cula con valor -2.
*/
void iniciarGrid();

/*
 * M�todo que inicializa el algoritmo NF1 para ir dando valores.
 */
void planPath(int x_ini, int y_ini, int x_end, int y_end);

/*
 * M�todo que encuentra el camino desde la posici�n inicial.
 */
void encontrarCamino(int x_ini, int y_ini);

/*
* M�todo que replanifica la trayectoria al detectar un obst�culo.
*/
void rePlanPath(int celdaX, int celdaY);

/*
* M�todo que redondea las coordenadas por si el robot se ha desviado.
*/
int redondearCoord(float coord);

/*
* M�todo que redondea el �ngulo para dejarlo igual al eje m�s cercano.
*/
float redondearAng(float angulo);

/*
* M�todo que detecta si hay un obst�culo en la trayectoria.
*/
bool detectObstacle(float theta);

/*
* M�todo que planifica el camino desde la posici�n actual hasta una celda
* dada.
*/
bool go(int cellX, int cellY);

/*
* M�todo que recorre el camino hasta llegar al objetivo.
*/
void recorrerCamino();


#endif
