#include "controlVelocidad.h"

/*
* Funci�n que lee la posici�n del robot.
*/
void readOdometry(float &x, float &y, float &theta) {

	AcquireMutex(semaphore_odometry);        // Se bloquea en el sem�foro.
  // Se leen las variables.
	x = robot_odometry.x;
	y = robot_odometry.y;
	theta = robot_odometry.th;
	ReleaseMutex(semaphore_odometry);      // Se desbloquea el sem�foro.

}

/*
* Funci�n que calcula la velocidad de cada rueda y se le asigna a los motores.
*/
int setSpeed(float v, float w){

  float w_l = 0.0, w_r = 0.0;  // Velocidad angular de cada rueda.
  float mR = 5.81155692, mL = 5.80103454, nR = 0.3985, nL = 0.5209; // Par�metros para la transferencia a los motores.
  float motorPowerRight = 0.0, motorPowerLeft = 0.0;  // Potencia de cada motor.

  // Calculamos la velocidad angular de cada rueda.
  w_r = v/R + w*L/(2.0*R);
  w_l = v/R - w*L/(2.0*R);

  // Se calcula la potencia del motor.
  motorPowerLeft = mL*w_l + nL;
  motorPowerRight = mR*w_r + nR;

	// Se asigna la potencia al motor.
	hogCPU();			// Se bloquea la CPU.
	motor[motorB] = motorPowerRight;
	motor[motorC] = motorPowerLeft;
	// Se muestran las velocidades por pantalla.
	//nxtDisplayTextLine(2, "POTENCIAS");
  //nxtDisplayTextLine(3, "derecho: %2.2f", motor[motorB]);
  //nxtDisplayTextLine(4, "izquierdo: %2.2f", motor[motorC]);
	releaseCPU();			// Se libera la CPU.

  return 0;

}

/*
* Funci�n que va actualizando la odometr�a para saber en todo momento donde
* est� el robot.
*/
task updateOdometry(){

  int cycle = 100; // N�mero de ciclos para actualizar odometr�a.
  float dSl = 0.0, dSr = 0.0, dx = 0.0, dy = 0.0, dS = 0.0;    // Variables para la odometr�a.
	float timeAux = 0.0;		// Variable para el tiempo.
	Delete(sFileName, nIoResult);		// Se borra el fichero si ya existe.
	float x = 0.0, y = 0.0, theta = 0.0;	// Posicion del robot.
	float ruedaDer = 0.0, ruedaIzq = 0.0; // Cuanto han girado las ruedas en cada ciclo.
	float incTheta = 0.0; // Angulo que ha girado el robot en cada ciclo.

	// Se abre el fichero.
	OpenWrite(hFileHandle, nIoResult, sFileName, nFileSize);

	while (true){       // Bucle infinito que va actualizando.

    timeAux=nPgmTime;		// Se lee el tiempo actual.
    hogCPU();     // Se bloquea la CPU.
    // Se leen los tac�metros.
    ruedaDer = nMotorEncoder[motorB];
    ruedaIzq = nMotorEncoder[motorC];
    // Se ponen a 0 los tac�metros.
    nMotorEncoder[motorC] = 0.0;
    nMotorEncoder[motorB] = 0.0;
    releaseCPU();     // Se libera la CPU.

    // Se calculan los par�metros.
    dSl = 2*numPi*R*(ruedaIzq/360.0);     // Aumento rueda izquierda.
    dSr = 2*numPi*R*(ruedaDer/360.0);     // Aumento rueda derecha.
    dS = (dSl + dSr)/2.0;						// Cuanto ha avanzado el robot en el ciclo.
    incTheta = (dSr - dSl)/L;				// Angulo girado por el robot en el ciclo.
    readOdometry(x,y,theta);			// Se lee la odometr�a.
    dx = dS * cos(theta + incTheta/2.0);      // Aumento coordenada x.
    dy = dS * sin(theta + incTheta/2.0);      // Aumento coordenada y.

    theta = theta + incTheta;						// Aumento del �ngulo.
    theta = normalizarAngulo(theta);		// Se normaliza el �ngulo.
    // Aumento en el eje x e y.
    x = x + dx;
    y = y + dy;

    AcquireMutex(semaphore_odometry);     // Se bloquea con el sem�foro.
    set_position(robot_odometry, x, y, theta);		 // Se fija la posici�n.
    ReleaseMutex(semaphore_odometry);     // Se librea con el sem�foro.

    // Par�metros para escribir en el fichero.
    string sString;     // Variable para guardar el string.

    // Se escribe en el fichero.
    stringFormat(sString, "%2.2f ", x);
    WriteText(hFileHandle, nIoResult, sString);
    stringFormat(sString, "%2.2f ",y);
    WriteText(hFileHandle, nIoResult, sString);
    stringFormat(sString, "%2.2f\n",theta);
    WriteText(hFileHandle, nIoResult, sString);

    // Esperar hasta completar el ciclo.
    wait1Msec(cycle-(nPgmTime-timeAux));

  }

}

/*
 * M�todo que comprueba que un valor se encuentre entre un rango de valores.
 */
bool compareValor(float actual, float objetivo, bool menor) {

	float eps = 0.15;			// Valor epsilon.

	// Se comprueba si est� en el rango.
	if(actual >= (objetivo - eps)	&& actual <= (objetivo + eps)) {
		return true;
	}

	return false;
}
