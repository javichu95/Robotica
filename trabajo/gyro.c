#include "hitechnic-gyro.h"

tSensors port = S3;
tHTGYRO gyro;

/*
 * Convierte de grados a radianes.
 */
float grad2rad(float grados) {
	return grados*numPi/180;
}

/*
 * Termina cuando el giroscopo indica que ha girado lo indicado.
 */
void girarHasta(float angulo) {
	angulo = abs(angulo);
	time1[T1] = 0;
	readSensor(&gyro);
	while(abs(grad2rad(gyro.rotation*time1[T1])) >= angulo) {
		readSensor(&gyro);
	}
}

/*
 * Calibra el giroscopo.
 */
void calibrarGyro() {
	sensorCalibrate(&gyro);
}

/*
 * Inicializa el sensor.
 */
void inicializarGyro() {
	initSensor(&gyro,port);
	calibrarGyro();
}
