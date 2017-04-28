#ifndef CONTROLVELOCIDAD_H
#define CONTROLVELOCIDAD_H

#include "mutexLib.c"
#include "positionLib.c"

// Par�metros del robot.
float R = 26.0; // Radio de la rueda.
float L = 114.0; // Longitud del eje.

// Par�metros para el movimiento de la pinza.
float movPinza = 40.0;		// Velocidad de la pinza al cerrarse/abrirse.
float presionPinza = 20.0;	// Presi�n que ejercera cuando est� cerrada.

// Par�metros del fichero.
string sFileName = "odometria.txt";     // Nombre del fichero.
TFileIOResult nIoResult;
TFileHandle hFileHandle = 0;
short nFileSize = 20000;			// Tama�o del fichero.

// Par�metros para la odometr�a.
TPosition robot_odometry;       // Posici�n para odometr�a.
TMutex semaphore_odometry = 0;  // Sem�foro para odometr�a.

/*
* Funci�n que lee la posici�n del robot.
*/
void readOdometry(float &x, float &y, float &theta);

/*
* Funci�n que calcula la velocidad de cada rueda y se le asigna a los motores.
*/
int setSpeed(float v, float w);

/*
* Funci�n que va actualizando la odometr�a para saber en todo momento donde
* est� el robot.
*/
task updateOdometry();

/*
 * M�todo que comprueba que un valor se encuentre entre un rango de valores.
 */
bool compareValor(float actual, float objetivo, bool menor);

#endif
