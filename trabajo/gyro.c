#include "hitechnic-gyro.h"
#include "controlvelocidad.c"

float eps = 0.15;

/*
 * Convierte de grados a radianes.
 */
float grad2rad(float grados) {
	return (grados*numPi)/180.0;
}


/*
 * Calibra el giroscopo.
 */
void calibrarGyro() {
	HTGYROstartCal(gyro);
}

/*
 * Termina cuando el giroscopo indica que ha girado lo indicado.
 */
void girarHasta(float angulo, float w) {
	float x, y ,theta;
	readOdometry(x,y,theta);
	angulo = abs(angulo);
	calibrarGyro();
	setSpeed(0,w);
	time1[T1] = 0;
	nxtDisplayTextLine(3,"%f",angulo);
	float girado = abs(grad2rad(HTGYROreadRot(gyro)*(time1[T1]/1000.0)));
	while(girado <= angulo+eps) {
		girado = abs(grad2rad(HTGYROreadRot(gyro)*(time1[T1]/1000.0)));
	}
	setSpeed(0,0);
	float rot = normalizarAngulo((girado*w/abs(w)) - eps + theta);
	readOdometry(x,y,theta);
	resetOdometry(x,y,rot);
	/*
	angulo = abs(angulo);
	calibrarGyro();
	float girado = 0.0;
	setSpeed(0,w);
	time1[T1] = 0;
	nxtDisplayTextLine(3,"%f",angulo);
	while(girado < angulo) {
		while(time1[T1] < 10) {
			wait1Msec(1);
		}
		time1[T1] = 0;
		float vel = HTGYROreadRot(gyro);
		girado += abs(vel*0.01);
		nxtDisplayTextLine(1,"%f",girado);
		nxtDisplayTextLine(2,"%f",vel);
	}
	setSpeed(0,0);
	*/
}
