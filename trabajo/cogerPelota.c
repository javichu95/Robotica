/////////////////////////////////////////////////////////////////////////////////////////
//  Conjunto de funciones que se encargan de la b�squeda de la pelota, aproximaci�n    //
//  hasta ella y cogerla finalmente.                                                   //
/////////////////////////////////////////////////////////////////////////////////////////

// Librer�as para la c�mara.
#include "common.h"
#include "nxtcamlib.c"

// Librer�as para el control de motores.
#include "gyro.c"

// Colores a detectar.
#define RED 0

// Color a detectar y �rea del blop.
#define GOAL_COLOR RED
#define AREA_COLOR 1600

// Movimientos de las pinzas para cerrar y abrir.
float movPinza = 40.0;			// Valor del movimiento de la pinza.
float presionPinza = 10.0;	// Valor de la presi�n de la pinza.

/*
* M�todo que abre la pinza del robot.
*/
void abrirPinza() {

	hogCPU();			// Se bloquea la CPU.
	motor[motorA] = -movPinza;	// Se abre la pinza.
	releaseCPU();			// Se libera la CPU.

	wait1Msec(500);		// Se espera un tiempo.

	hogCPU();			// Se bloquea la CPU.
	motor[motorA] = 0;		// Se detiene el motor.
	releaseCPU();		// Se libera la CPU.

}

/*
 * M�todo que cierra la pinza del robot.
 */
void cerrarPinza() {

	hogCPU();			// Se bloquea la CPU.
	motor[motorA] = movPinza;		//Se cierra la pinza.
	releaseCPU();		// Se libera la CPU.

	wait1Msec(500);			// Se espera un tiempo.

	hogCPU();			// Se bloquea la CPU.
	motor[motorA] = presionPinza;		//mUna vez cerrada sigue ejerciendo algo de presi�n.
	releaseCPU();		// Se libera la CPU.

}

/*
* M�todo que deja de ejercer presi�n con la pinza.
*/
void aflojarPinza() {

	hogCPU();	// Se bloquea la CPU.
	motor[motorA] = 0.0;			// Se deja de ejercer presi�n.
	releaseCPU();		// Se libera la CPU.

}

/*
* M�todo que comprueba si ha cogido la pelota para la carrera.
*/
bool cogida() {

	int _nblobs = 0;								// N�mero de blops detectados.
	int_array bc, bl, bt, br, bb;		// Variables para la detecci�n de la c�mara.

	get_blobs(cam, _nblobs, bc, bl, bt, br, bb);	// Se obtienen los blops.

	for (int i = 0; i < _nblobs; i++) {		// Se recorren los blops.
		if (bc[i] == GOAL_COLOR){			// Si el color coincide...
			// Se calcula su �rea.
			float area = (br[i] - bl[i])*(bb[i]-bt[i]);
			if(area > AREA_COLOR-200 && bt[i] < 144) {		// Se compara para saber si el �rea corresponde.
				return true;
			}
		}
	}
	return false;

}

/*
* M�todo que comprueba si ha cogido la pelota para la evaluaci�n.
*/
bool cogidaEval() {

	wait1Msec(500);
	setSpeed(0,numPi/2);

	time1[T1] = 0;

	while(time1[T1] <= 4000){
		int _nblobs = 0;								// N�mero de blops detectados.
		int_array bc, bl, bt, br, bb;		// Variables para la detecci�n de la c�mara.

		get_blobs(cam, _nblobs, bc, bl, bt, br, bb);	// Se obtienen los blops.

		for (int i = 0; i < _nblobs; i++) {		// Se recorren los blops.
			if (bc[i] == GOAL_COLOR){			// Si el color coincide...
				// Se calcula su �rea.
				float area = (br[i] - bl[i])*(bb[i]-bt[i]);
				if(area > 5) {		// Se compara para saber si el �rea corresponde.
					setSpeed(0,0);
					abrirPinza();
					return false;
				}
			}
		}
	}
	setSpeed(0,0);
	return true;

}

/*
 * M�todo que centra la pelota en la c�mara.
 */
float centroPelota(int_array bl, int_array br, int_array bt, int_array bb, int i){

	// Variables para calcular el error respecto al centro.
	int x_centre, x_error;

	// Fija el centro del blop respecto a la c�mara.
	x_centre = bl[i] + br[i];

	// Calcula el error del blop.
	x_error = 176 - x_centre;

	return x_error;		// Se devuelve el error.
}

/*
 * Mueve el robot para buscar la pelota.
 */
void buscar(bool derecha){

	if(derecha) {
		setSpeed(0,-numPi/2);		// Se fija la velocidad angular para dar vueltas.
	} else {
		setSpeed(0,numPi/2);		// Se fija la velocidad angular para dar vueltas.
	}
}

/*
 * Avanza hacia la pelota y si la tiene a la distancia adecuada la coge.
 */
bool atraparPelota(float area, float w){

	if(area < AREA_COLOR) {		// Si el area no es grande avanza hacia la pelota.

		setSpeed(120,w);		// Fija la velocidad.
		wait1Msec(1000);			// Espera un tiempo.
		return false;		// Indica que no ha cogido la pelota.
	}
	else {			// Si el area es suficientemente grande intenta cogerla.

		setSpeed(0, numPi/8);		// Se fija una velocidad angular para girar al robot.
		wait1Msec(750);			// Se espera un tiempo.
		setSpeed(85,0);			// Se avanza linealmente.
		wait1Msec(2500);		// Se espera un tiempo.
		setSpeed(0, 0);	// Se detiene al robot.3.
		cerrarPinza();	// Se cierra la pinza.

		return true;
	}

}

/*
 * M�todo principal que centra al robot y coge la pelota.
 */
void buscarAtrapar(){

	abrirPinza();				//Se abre la pinza.

	bool continueTracking = true;		// Booleano para indicar si se sigue el tracking.
	int _nblobs = 0;								// N�mero de blops detectados.
	int_array bc, bl, bt, br, bb;		// Variables para la detecci�n de la c�mara.
	bool encontrada = false;				// Booleano para indicar si se ha encontrado la pelota.
	init_camera(cam);								// Se inicializa la c�mara.

	// Variables para los blops.
	float areaMayor = 0.0;
	int blopMayor = 0;
	int lastBloopIzq = 0;

	while (continueTracking) {			// Mientras se deba continuar el tracking...

		get_blobs(cam, _nblobs, bc, bl, bt, br, bb);	// Se obtienen los blops.
		areaMayor = 0.0;
		for (int i = 0; i < _nblobs; i++) {		// Se recorren los blops.

			if (bc[i] == GOAL_COLOR){			// Si el color coincide...
				// Se calcula su �rea.
				float area = (br[i] - bl[i])*(bb[i]-bt[i]);
				if(area > 10) {
					encontrada = true;		// Se indica que se ha encontrado.
					if(area > areaMayor) {		// Se compara con el mayor �rea vista.
						areaMayor = area;		// Si es la mayor se guarda.
						blopMayor = i;
						lastBloopIzq = bl[i];
					}
				}
			}

		}

		if(!encontrada){		// Si no encuentra la pelota, la busca.
				buscar(lastBloopIzq > 176);
		} else{			// Si la ha encontrado...
				float error = centroPelota(bl, br, bt, bb, blopMayor);	// Centra la pelota.
				float angular = error*0.002;			// Se asigna la velocidad angular.
				if(atraparPelota(areaMayor,angular)) {	// Avanza para coger la pelota.
					continueTracking = false;		//Si ha atrapado la pelota finaliza.
				}
				encontrada = false;		// Si no la ha atrapado, sigue buscando.
		}
	}

	aflojarPinza();		// Afloja la pinza.
	setSpeed(0,0);
}
