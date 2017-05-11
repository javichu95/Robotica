///////////////////////////////////////////////////////////////////////////////////////////
//  Conjunto de funciones para realizar los giros del robot sobre si mismo con la ayuda  //
//  del giróscopo.                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////

#include "hitechnic-gyro.h"
#include "controlvelocidad.c"

float eps = 0.15;		// Valor epsilón para los giros.

/*
 * Método que convierte de grados a radianes.
 */
float grad2rad(float grados){

	return (grados*numPi)/180.0;
}


/*
 * Método que calibra el giroscopo.
 */
void calibrarGyro() {

	HTGYROstartCal(gyro);
}

/*
 * Método que realiza el giro según los valores devueltos por el giróscopo.
 */
void girarHasta(float angulo, float w) {

	float x, y ,theta;		// Variables para la odometría.
	readOdometry(x,y,theta);		// Se lee la odometría.

	angulo = abs(angulo);			// Se obtiene el valor absoluto del ángulo.
	calibrarGyro();			// Se calibra el giróscopo.
	setSpeed(0,w);		// Se fija la velocidad.

	time1[T1] = 0;		// Se pone el tiempo a 0.
	float girado = 0.0;			// Se pone el ángulo girado a 0.
	while(girado <= angulo + eps) {		// Se repita hasta que cumpla el giro.
		girado = abs(grad2rad(HTGYROreadRot(gyro)*(time1[T1]/1000.0)));
	}

	setSpeed(0,0);		// Se para el robot.
	// Se calcula el ángulo de rotación.
	float rot = normalizarAngulo(((girado - eps)*w/abs(w)) + theta);
	readOdometry(x,y,theta);			// Se lee la odometría.
	resetOdometry(x,y,rot);			// Se resetea con el nuevo valor.
}
