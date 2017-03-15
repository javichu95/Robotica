/*
* Hace que el robot se mueva a una cierta velocidad (lineal o angular) en una trayectoria
* en 8. Actualiza internamente la odometría así que puede saber donde está en cada momento.
*/

#include "mutexLib.c"
#include "positionLib.c"

// Parámetros del robot.
float R = 26.0; // Radio de la rueda.
float L = 113.0; // Longitud del eje.
float numPi = 3.14159265;     // Valor de numPi.
// Par�metros del fichero.
string sFileName = "odometria.txt";     // Nombre del fichero.
TFileIOResult nIoResult;
TFileHandle hFileHandle = 0;
short nFileSize = 20000;

TPosition robot_odometry;       // Posición para odometría.
TMutex semaphore_odometry = 0;  // Semáforo para odometría.


/*
* Función que calcula la velocidad de cada rueda y se le asigna a los motores.
*/
int setSpeed(float v, float w){

  float w_l = 0.0, w_r = 0.0;  // Velocidad angular de cada rueda.
  float mR = 5.81155692, mL = 5.80103454, nR = 0.3985, nL = 0.5209; // Parámetros para la transferencia a los motores.
  float motorPowerRight = 0.0, motorPowerLeft = 0.0;  // Potencia de cada motor.

  // Calculamos la velocidad angular de cada rueda.
  w_r = v/R + w*L/(2.0*R);
  w_l = v/R - w*L/(2.0*R);

  // Se calcula la potencia del motor.
  motorPowerLeft = mL*w_l + nL;
  motorPowerRight = mR*w_r + nR;

	// Se asigna la potencia al motor.
	motor[motorB] = motorPowerRight;
	motor[motorC] = motorPowerLeft;

  return 0;

}

/*
* Función que obtiene las velocidades linear y angular del robot.
*/
void readSpeed(float &v, float &w) {


}

/*
* Función que va actualizando la odometría para saber en todo momento donde
* está el robot.
*/
task updateOdometry(){

  int cycle = 100; // Número de ciclos para actualizar odometría.
  float dSl = 0.0,dSr = 0.0, dx = 0.0,dy = 0.0;    // Variables para la odometría.
	float timeAux = 0.0;
	Delete(sFileName, nIoResult);		// Se borra el fichero si ya existe.

	// Se abre el fichero.
	OpenWrite(hFileHandle, nIoResult, sFileName, nFileSize);

	while (true){       // Bucle infinito que va actualizando.

    timeAux=nPgmTime;
    hogCPU();     // Se bloquea la CPU.
    // Se leen los tacómetros.
    float ruedaDer = nMotorEncoder[motorB];
    float ruedaIzq = nMotorEncoder[motorC];
    nMotorEncoder[motorC] = 0.0;
    nMotorEncoder[motorB] = 0.0;
    releaseCPU();     // Se libera la CPU.

    AcquireMutex(semaphore_odometry);     // Se bloquea con el semáforo.
    // Se calculan los parámetros.
    dSl = 2*numPi*R*(ruedaIzq/360.0);     // Aumento rueda izquierda.
    dSr = 2*numPi*R*(ruedaDer/360.0);     // Aumento rueda derecha.
    float dS = (dSl + dSr)/2.0;
    float incTheta = (dSr - dSl)/L;
    dx = dS * cos((robot_odometry.th) + incTheta/2.0);      // Aumento coordenada x.
    dy = dS * sin((robot_odometry.th) + incTheta/2.0);      // Aumento coordenada y.
    float theta = (robot_odometry.th) + incTheta;
    float x = (robot_odometry.x) + dx;
    float y = (robot_odometry.y) + dy;

    set_position(robot_odometry, x, y, theta);
    ReleaseMutex(semaphore_odometry);     // Se librea con el semáforo.

    // Mostrar cada paso en la pantalla.
    nxtDisplayTextLine(2, "ODOMETRY NEW VALUE");
    nxtDisplayTextLine(3, "x,y: %2.2f %2.2f", robot_odometry.x, robot_odometry.y);
    nxtDisplayTextLine(4, "theta: %2.2f ", robot_odometry.th);

    // Parámetros para escribir en el fichero.
    string sString;     // Variable para guardar el string.

    // Se escribe en el fichero.
    stringFormat(sString, "NUEVO DATO\n");
    WriteText(hFileHandle, nIoResult, sString);
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
* Función que lee la posición del robot.
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
 * Comprueba que un valor se encuentre entre un rango de valores.
 */
bool compareValor(float actual, float objetivo,bool menor) {
	float eps = 0.15;
	if(actual >= (objetivo)	&& actual <= (objetivo + eps)) {
		return true;
	}
	return false;
	/*
	if(menor) {
		if(actual <= objetivo) {
				return true;
		}
	} else {
		if(actual >= objetivo) {
			return true;
		}
	}
	return false;
	*/
}

/*
 * Realiza la trayectoria del 8 por tiempo (sin odometria).
 */
void trayectoria8Tiempo() {
	float v = 0.0, w = 0.0;       // Velocidades lineal y angular.
  float radio; 				     	// Radio de la trayectoria.
	// Se gira el robot 90º.
	w = -numPi/2.0;
	setSpeed(v,w);	// Velocidad lineal a 0 para que el robot gire sobre si mismo.
	wait1Msec((-numPi/2.0/w)*1000.0);	// Se espera a que el robot gire -90 grados.

  // Se genera la primera parte de la trayectoria.
	radio = 400.0;
	v = 150.0;
	w = v/radio;			// Se calcula la velocidad angular.
	setSpeed(v,w);   // Se fijan las velocidades.
	wait1Msec((numPi/w)*1000.0);		// Se espera el tiempo que tardará en girar 180 grados.

  // Se genera la segunda parte de la trayectoria.
	setSpeed(v,-w);   // Se fijan las velocidades.
	wait1Msec((2.0*numPi/w)*1000.0); // Se espera el tiempo que tardará el robot en girar 360 grados.

  // Se genera la tercera parte de la trayectoria.
	setSpeed(v,w);     // Se fijan las velocidades.
	wait1Msec((numPi/w)*1000.0);	// Se espera el tiempo que tardara el robot en girar 180 grados.

  // Se genera la cuarta parte de la trayectoria.
	w = numPi/2.0;      // Se establecen los valores para velocidad angular y lineal.
	v = 0.0;
	setSpeed(v,w);	// Se fijan las velocidades.
	wait1Msec((numPi/2.0/w)*1000.0);	// Se espera a que el robot gire 90 grados.
}

/*
 * Se realiza la trayectoria del 8 utilizando la odometria y comprobando la posici�n mediante el angulo.
 */
void trayectoria8Angulo() {
	float v = 0.0, w = 0.0;       // Velocidades lineal y angular.
  float radio; 				     	// Radio de la trayectoria.
	float x = 0.0, y = 0.0, th = 0.0;
	// Se gira el robot 90º.
	w = -numPi/2.0;
	setSpeed(v,w);	// Velocidad lineal a 0 para que el robot gire sobre si mismo.
	do {
		readOdometry(x,y,th);
	} while(!compareValor(th,0.0,true));
	setSpeed(0,0);	// Se fijan las velocidades.
	wait1Msec(3000);	// Se espera a que el robot gire 90 grados.

  // Se genera la primera parte de la trayectoria.
	radio = 400.0;
	v = 150.0;
	w = v/radio;			// Se calcula la velocidad angular.
	setSpeed(v,w);   // Se fijan las velocidades.
	do {
		readOdometry(x,y,th);
	} while(!compareValor(th,numPi,false));
	setSpeed(0,0);	// Se fijan las velocidades.
	wait1Msec(3000);	// Se espera a que el robot gire 90 grados.

  // Se genera la segunda parte de la trayectoria.
	setSpeed(v,-w);   // Se fijan las velocidades.
	do {
		readOdometry(x,y,th);
	} while(!compareValor(th,-numPi,true));
	setSpeed(0,0);	// Se fijan las velocidades.
	wait1Msec(3000);	// Se espera a que el robot gire 90 grados.

  // Se genera la tercera parte de la trayectoria.
	setSpeed(v,w);     // Se fijan las velocidades.
	do {
		readOdometry(x,y,th);
	} while(!compareValor(th,0.0,false));
	setSpeed(0,0);	// Se fijan las velocidades.
	wait1Msec(3000);	// Se espera a que el robot gire 90 grados.

  // Se genera la cuarta parte de la trayectoria.
	w = numPi/2.0;      // Se establecen los valores para velocidad angular y lineal.
	v = 0.0;
	setSpeed(v,w);	// Se fijan las velocidades.
	do {
		readOdometry(x,y,th);
	} while(!compareValor(th,numPi/2,false));
}

/*
* Programa principal que realiza la trayectoria asignando velocidades a las ruedas.
*/
task main()
{

  // Encoders a 0.
	nMotorEncoder[motorB] = 0.0;
	nMotorEncoder[motorC] = 0.0;
  // Valores de odometría a 0.
	set_position(robot_odometry,INI_X,INI_Y,INI_TH);

  // Se inicializa la tarea de actualizar odometría.
	startTask(updateOdometry);
  // Se establecen la velocidad lineal y angular.
	//setSpeed(100,0);
	//wait1Msec(4000);

	// TRAYECTORIA EN 8.
	//trayectoria8Tiempo();
  trayectoria8Angulo();

	stopTask(updateOdometry);		// Se para la tarea.

	Close(hFileHandle, nIoResult);		// Se cierra el fichero.
}
