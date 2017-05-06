///////////////////////////////////////////////////////////////////////////////////////////
//  Conjunto de funciones para dar velocidad a los motores y mantener una actualización  //
//  periodica de la odometría.                                                           //
///////////////////////////////////////////////////////////////////////////////////////////

#include "mutexLib.c"
#include "positionLib.c"

// Parámetros del robot.
float R = 26.0; // Radio de la rueda.
float L = 114.0; // Longitud del eje.

// Parámetros del fichero de odometría.
string sFileName = "odometria.txt";     // Nombre del fichero.
TFileIOResult nIoResult;
TFileHandle hFileHandle = 0;
short nFileSize = 20000;			// Tamaño del fichero.

// Parámetros para la odometría.
TPosition robot_odometry;       // Posición para odometría.
TMutex semaphore_odometry = 0;  // Semáforo para odometría.

/*
* Función que lee la posición del robot de la odometría.
*/
void readOdometry(float &x, float &y, float &theta) {

	AcquireMutex(semaphore_odometry);        // Se bloquea en el semáforo.

  // Se leen las variables.
	x = robot_odometry.x;
	y = robot_odometry.y;
	theta = robot_odometry.th;

	ReleaseMutex(semaphore_odometry);      // Se desbloquea el semáforo.

}

/*
* Función que calcula la velocidad de cada rueda y se le asigna a los motores.
*/
int setSpeed(float v, float w){

  float w_l = 0.0, w_r = 0.0;  // Velocidad angular de cada rueda.
  // Parámetros para la transferencia a los motores.
  float mR = 5.81155692, mL = 5.80103454, nR = 0.3985, nL = 0.5209;
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
	/*nxtDisplayTextLine(2, "POTENCIAS");
  nxtDisplayTextLine(3, "derecho: %2.2f", motor[motorB]);
  nxtDisplayTextLine(4, "izquierdo: %2.2f", motor[motorC]);*/

	releaseCPU();			// Se libera la CPU.

  return 0;

}

/*
 * Modifica los valores de la odometria.
 */
void resetOdometry(float x, float y, float theta) {
		stopTask(updateOdometry);
		AcquireMutex(semaphore_odometry);
		set_position(robot_odometry, x, y, theta);
		ReleaseMutex(semaphore_odometry);
		startTask(updateOdometry);
}

/*
* Función que va actualizando la odometría para saber en todo momento donde
* está el robot.
*/
task updateOdometry(){

	Delete(sFileName, nIoResult);		// Se borra el fichero de odometría si ya existe.

  int cycle = 50; // Número de ciclos para actualizar odometría.
  // Variables para la odometría.
  float dSl = 0.0, dSr = 0.0, dx = 0.0, dy = 0.0, dS = 0.0;
	float timeAux = 0.0;		// Variable para el tiempo.
	float x = 0.0, y = 0.0, theta = 0.0;	// Posicion del robot.
	float ruedaDer = 0.0, ruedaIzq = 0.0; // Cuanto han girado las ruedas en cada ciclo.
	float incTheta = 0.0; // Angulo que ha girado el robot en cada ciclo.

	// Se abre el fichero.
	OpenWrite(hFileHandle, nIoResult, sFileName, nFileSize);

	while (true){       // Bucle infinito que va actualizando.

    timeAux=nPgmTime;		// Se lee el tiempo actual.
    hogCPU();     // Se bloquea la CPU.

    // Se leen los tacómetros.
    ruedaDer = nMotorEncoder[motorB];
    ruedaIzq = nMotorEncoder[motorC];

    // Se ponen a 0 los tacómetros.
    nMotorEncoder[motorC] = 0.0;
    nMotorEncoder[motorB] = 0.0;

    releaseCPU();     // Se libera la CPU.

    // Se calculan los parámetros.
    dSl = 2*numPi*R*(ruedaIzq/360.0);     // Aumento rueda izquierda.
    dSr = 2*numPi*R*(ruedaDer/360.0);     // Aumento rueda derecha.
    dS = (dSl + dSr)/2.0;						// Cuanto ha avanzado el robot en el ciclo.
    incTheta = (dSr - dSl)/L;				// Angulo girado por el robot en el ciclo.

    readOdometry(x,y,theta);			// Se lee la odometría.
    dx = dS * cos(theta + incTheta/2.0);      // Aumento coordenada x.
    dy = dS * sin(theta + incTheta/2.0);      // Aumento coordenada y.

    theta = theta + incTheta;						// Aumento del ángulo.
    theta = normalizarAngulo(theta);		// Se normaliza el ángulo.

    // Aumento en el eje x e y.
    x = x + dx;
    y = y + dy;

    AcquireMutex(semaphore_odometry);     // Se bloquea con el semáforo.
    set_position(robot_odometry, x, y, theta);		 // Se fija la posición.
    ReleaseMutex(semaphore_odometry);     // Se librea con el semáforo.

    // Parámetros para escribir en el fichero.
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
