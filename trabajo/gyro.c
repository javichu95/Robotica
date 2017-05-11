///////////////////////////////////////////////////////////////////////////////////////////
//  Conjunto de funciones para realizar los giros del robot sobre si mismo con la ayuda  //
//  del gir�scopo.                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////

#include "hitechnic-gyro.h"
#include "controlvelocidad.c"

float eps = 0.15;		// Valor epsil�n para los giros.

/*
 * M�todo que convierte de grados a radianes.
 */
float grad2rad(float grados){

	return (grados*numPi)/180.0;
}


/*
 * M�todo que calibra el giroscopo.
 */
void calibrarGyro() {

	HTGYROstartCal(gyro);
}

/*
 * M�todo que realiza el giro seg�n los valores devueltos por el gir�scopo.
 */
void girarHasta(float angulo, float w) {

	float x, y ,theta;		// Variables para la odometr�a.
	readOdometry(x,y,theta);		// Se lee la odometr�a.

	angulo = abs(angulo);			// Se obtiene el valor absoluto del �ngulo.
	calibrarGyro();			// Se calibra el gir�scopo.
	setSpeed(0,w);		// Se fija la velocidad.

	time1[T1] = 0;		// Se pone el tiempo a 0.
	float girado = 0.0;			// Se pone el �ngulo girado a 0.
	while(girado <= angulo + eps) {		// Se repita hasta que cumpla el giro.
		girado = abs(grad2rad(HTGYROreadRot(gyro)*(time1[T1]/1000.0)));
	}

	setSpeed(0,0);		// Se para el robot.
	// Se calcula el �ngulo de rotaci�n.
	float rot = normalizarAngulo(((girado - eps)*w/abs(w)) + theta);
	readOdometry(x,y,theta);			// Se lee la odometr�a.
	resetOdometry(x,y,rot);			// Se resetea con el nuevo valor.
}
