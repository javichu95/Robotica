#pragma config(Sensor, S2,     cam,                 sensorI2CCustomFastSkipStates)
#pragma config(Motor,  motorB,          r_motor,       tmotorNXT, PIDControl)
#pragma config(Motor,  motorC,          l_motor,       tmotorNXT, PIDControl)

// CONFIG camera color position
#define BLUE 1
#define RED 0

// CONFIG GOAL PARAMETERS
#define GOAL_COLOR RED
#define AREA_COLOR 100


// SUSTITUIR POR LAS LIBRERIAS QUE USEIS para acceder a la c√°mara.
// Distintas versiones tienen nombres ligeramente distintos
#include "G:\robotica\3rd Party Driver Library\include\common.h"
#include "G:\robotica\3rd Party Driver Library\include\nxtcamlib.c"
#include "controlVelocidad.h"


/************************************************************************************/
// follow_object.c
// Track a blob of the selected color until the robot is "close enough"
/************************************************************************************/

/*
 * MÈtodo que centra la pelota en la c·mara.
 */
bool centrarPelota(int_array bl, int_array br, int_array bt, int_array bb, int i) {

	// Variables para calcular el error respecto al centro.
	int x_centre, x_error;
	int y_centre, y_error;
	bool erased = false;

		if (!erased) {
			//nxtDisplayTextLine(0,"Tracking ...");
			erased = true;
		}

		// Fija el centro del blop respecto a la c·mara.
		x_centre = bl[i] + br[i];
		y_centre = bt[i] + bb[i];

		// Calcula el error del blop.
		x_error = 176 - x_centre;
		y_error = 143 - y_centre;

	//nxtDisplayTextLine(0,"%.3f",x_error);
		if(abs(x_error)  < 3) {	// Si es menor es que ya est· centrada.
			nxtDisplayTextLine(0,"centrado");
			return true;
		}

		// Da velocidades al motor para centrarlo.
		motor[l_motor] = (y_error - x_error) / 5;
		motor[r_motor] = (y_error + x_error) / 5;

		return false;
}

/*
 * Mueve el robot para buscar la pelota.
 */
void buscar() {
	setSpeed(0,numPi/4);		// Se fija la velocidad angular para dar vueltas.
}

/*
 * Avanza hacia la pelota y si la tiene a la distancia adecuada la coge.
 */
 bool atraparPelota(float area) {
	nxtDisplayTextLine(5,"%.2f",area);
	if(area < AREA_COLOR) {		//Si el area no es grande avanza hacia la pelota.
		setSpeed(70,0);
		return false;		//Indica si ha cogido la pelota.
	} else {			//Si el area es suficientemente grande intenta cogerla.
		setSpeed(0, 0);	//Detiene al robot.
		cerrarPinza();	//Cierra la pinza.
		return true;	//indica si ha cogido la pelota.
	}
 }

/*
 * MÈtodo principal que centra al cobor y coge la pelota.
 */
task main (){

	bool continueTracking = true;		// Booleano para indicar si se sigue el tracking.
	int _nblobs = 0;		// N˙mero de blops detectados.
  	int_array bc, bl, bt, br, bb;			// Variables para la detecciÛn de la c·mara.
  	bool encontrada = false;			// Booleano para indicar si se ha encontrado la pelota.
	init_camera(cam);					// Se inicializa la c·mara.
	float areaMayor = 0.0;
	int blopMayor = 0;
	//bool haCentrado = false;
	while (continueTracking) {			// Mientras se deba continuar el tracking...

		// Se obtienen los blops.
		get_blobs(cam, _nblobs, bc, bl, bt, br, bb);
		nxtDisplayTextLine(1, "%d", _nblobs);

		for (int i = 0; i < _nblobs; i++) {		// Se recorren los blops.

		    if (bc[i] == GOAL_COLOR){			// Si el color coincide...
					nxtDisplayTextLine(0,"vistoooo");
		    	encontrada = true;
				area = (br[i] - bl[i])*(bt[i]-bb[i]);
				if(area > areaMayor) {
					areaMayor = area;
					blopMayor = i;
				}

			}
		}

		if(!encontrada){		// Si no encuentra la pelota, la busca.
			buscar();
		} else{			// Si la ha encontrado...
			bool centrado = centrarPelota(bl, br, bt, bb, blopMayor);	//Centra la pelota.
			if(centrado) {		// Se centra al robot.
				if(atraparPelota(areaMayor)) {	//Avanza para coger la pelota.
					//QUIZA DEBAMOS HACER UNA COMPROBACION O ALGO
					continueTracking = false;		//Si ha atrapado la pelota finaliza.
				}
			} 
			/* VERSION ALTERNATIVA PARA PROBAR (ESTA NO CREO QUE VAYA BIEN)
			//En cada iteracion centra y avanza alternativamente.
			if(!haCentrado) {
				//Si en la anterior ha avanzado, centra.
				centrarPelota(bl, br, bt, bb, blopMayor);
				haCentrado = true;		//Indica que ha centrado.
			} else {
				//Si en la anterior ha centrado, avanza.
				if(atraparPelota(areaMayor)) {
					continueTracking = false;	//Si ha atrapado la pelota acaba.
				}
				haCentrado = false;		//Indica que ha avanzado.
			}
			*/
		}
	}
}
