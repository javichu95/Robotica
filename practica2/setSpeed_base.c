/*
* Hace que el robot se mueva a una cierta velocidad (lineal o angular) en una trayectoria
* en 8. Actualiza internamente la odometría así que puede saber donde está en cada momento.
*/

#include "mutexLib.c"
#include "positionLib.c"

// Parámetros del robot.
float R = 26.0; // Radio de la rueda.
float L = 114.0; // Longitud del eje.
float numPi = 3.14159265;     // Valor de numPi.
// Par�metros del fichero.
string sFileName = "odometria.txt";     // Nombre del fichero.
TFileIOResult nIoResult;
TFileHandle hFileHandle = 0;
short nFileSize = 20000;			// Tama�o del fichero.

TPosition robot_odometry;       // Posición para odometría.
TMutex semaphore_odometry = 0;  // Semáforo para odometría.

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
  hogCPU();
	motor[motorB] = motorPowerRight;
	motor[motorC] = motorPowerLeft;
	nxtDisplayTextLine(2, "POTENCIAS");
    nxtDisplayTextLine(3, "derecho: %2.2f", motor[motorB]);
    nxtDisplayTextLine(4, "izquierdo: %2.2f", motor[motorC]);
	releaseCPU();


  return 0;

}

/*
 * Normaliza un angulo entre -pi y pi.
 */
float normalizarAngulo(float angulo) {

	while(angulo <= -numPi){
		angulo = angulo + 2*numPi;
	}
	while(angulo > numPi){
		angulo = angulo - 2*numPi;
	}
	return angulo;
}

/*
* Función que va actualizando la odometría para saber en todo momento donde
* está el robot.
*/
task updateOdometry(){

  int cycle = 50; // Número de ciclos para actualizar odometría.
  float dSl = 0.0, dSr = 0.0, dx = 0.0, dy = 0.0, dS = 0.0;    // Variables para la odometría.
	float timeAux = 0.0;
	Delete(sFileName, nIoResult);		// Se borra el fichero si ya existe.
	float x = 0.0, y = 0.0, theta = 0.0;	// Posicion del robot.
	float ruedaDer = 0.0, ruedaIzq = 0.0; // Cuanto han girado las ruedas en cada ciclo.
	float incTheta = 0.0; // Angulo que ha girado el robot en cada ciclo.
	// Se abre el fichero.
	OpenWrite(hFileHandle, nIoResult, sFileName, nFileSize);

	while (true){       // Bucle infinito que va actualizando.

    timeAux=nPgmTime;
    hogCPU();     // Se bloquea la CPU.
    // Se leen los tacómetros.
    ruedaDer = nMotorEncoder[motorB];
    ruedaIzq = nMotorEncoder[motorC];
    nMotorEncoder[motorC] = 0.0;
    nMotorEncoder[motorB] = 0.0;
    releaseCPU();     // Se libera la CPU.

    // Se calculan los parámetros.
    dSl = 2*numPi*R*(ruedaIzq/360.0);     // Aumento rueda izquierda.
    dSr = 2*numPi*R*(ruedaDer/360.0);     // Aumento rueda derecha.
    dS = (dSl + dSr)/2.0;						// Cuanto ha avanzado el robot en el ciclo.
    incTheta = (dSr - dSl)/L;				// Angulo girado por el robot en el ciclo.
    readOdometry(x,y,theta);			// Se lee la odometr�a.
    dx = dS * cos(theta + incTheta/2.0);      // Aumento coordenada x.
    dy = dS * sin(theta + incTheta/2.0);      // Aumento coordenada y.

    theta = theta + incTheta;						// Aumento del �ngulo.
    //theta = normalizarAngulo(theta);
    x = x + dx;
    y = y + dy;

    AcquireMutex(semaphore_odometry);     // Se bloquea con el semáforo.
    set_position(robot_odometry, x, y, theta);		 // Se fija la posici�n.
    ReleaseMutex(semaphore_odometry);     // Se librea con el semáforo.

    // Mostrar cada paso en la pantalla.
    /*nxtDisplayTextLine(2, "ODOMETRY NEW VALUE");
    nxtDisplayTextLine(3, "x,y: %2.2f %2.2f", robot_odometry.x, robot_odometry.y);
    nxtDisplayTextLine(4, "theta: %2.2f ", robot_odometry.th);*/

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



/*
 * Comprueba que un valor se encuentre entre un rango de valores.
 */
bool compareValor(float actual, float objetivo, bool menor) {

	float eps = 0.15;
	if(actual >= (objetivo - eps)	&& actual <= (objetivo + eps)) {
		return true;
	}
	return false;

	/*if(menor) {
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
 * Realiza la trayectoria del 8 por tiempo (sin odometr�a).
 */
void trayectoria8Tiempo() {

	float v = 0.0, w = 0.0;       // Velocidades lineal y angular.
  float radio; 				     	// Radio de la trayectoria.

	// Se gira el robot 90º.
	w = -numPi/2.0;			// Se asigna la velocidad angular.
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
 * Se realiza la trayectoria del 8 utilizando la odometria y comprobando
 * la posici�n mediante el �ngulo.
 */
void trayectoria8Angulo() {

	float v = 0.0, w = 0.0;       // Velocidades lineal y angular.
  float radio; 				     	// Radio de la trayectoria.
	float x = 0.0, y = 0.0, th = 0.0;	// Variables para la odometr�a.

	// Se gira el robot 90º.
	w = -numPi/2.0;
	setSpeed(v,w);	// Velocidad lineal a 0 para que el robot gire sobre si mismo.
	do {
		readOdometry(x,y,th);
	} while(!compareValor(th,0.0,true));
	/*setSpeed(0,0);	// Se fijan las velocidades para pararlo.
	wait1Msec(3000);	// Se espera a que el robot gire 90 grados.*/

  // Se genera la primera parte de la trayectoria.
	radio = 400.0;
	v = 150.0;
	w = v/radio;			// Se calcula la velocidad angular.
	setSpeed(v,w);   // Se fijan las velocidades.
	do {
		readOdometry(x,y,th);
	} while(!compareValor(th,numPi,false));
	/*setSpeed(0,0);	// Se fijan las velocidades para pararlo.
	wait1Msec(3000);	// Se espera a que el robot gire 90 grados.*/

  // Se genera la segunda parte de la trayectoria.
	setSpeed(v,-w);   // Se fijan las velocidades.
	do {
		readOdometry(x,y,th);
	} while(!compareValor(th,-numPi,true));
	/*setSpeed(0,0);	// Se fijan las velocidades para pararlo.
	wait1Msec(3000);	// Se espera a que el robot gire 90 grados.*/

  // Se genera la tercera parte de la trayectoria.
	setSpeed(v,w);     // Se fijan las velocidades.
	do {
		readOdometry(x,y,th);
	} while(!compareValor(th,0.0,false));
	/*setSpeed(0,0);	// Se fijan las velocidades para pararlo.
	wait1Msec(3000);	// Se espera a que el robot gire 90 grados.*/

  // Se genera la cuarta parte de la trayectoria.
	w = numPi/2.0;      // Se establecen los valores para velocidad angular y lineal.
	v = 0.0;
	setSpeed(v,w);	// Se fijan las velocidades para pararlo.
	do {
		readOdometry(x,y,th);
	} while(!compareValor(th,numPi/2,false));

	setSpeed(0,0);			// Se para el robot.

}

/*
* Programa principal que realiza la trayectoria asignando velocidades a las ruedas.
*/
task main(){
	float x = 0.0, y = 0.0, th = 0.0;
  // Encoders a 0.
	nMotorEncoder[motorB] = 0.0;
	nMotorEncoder[motorC] = 0.0;
  // Valores de odometría a 0.
	set_position(robot_odometry,INI_X,INI_Y,INI_TH);

  // Se inicializa la tarea de actualizar odometría.
	startTask(updateOdometry);
  // Se establecen la velocidad lineal y angular.
	/*setSpeed(0,numPi/2);
	do {
		readOdometry(x,y,th);
	} while(!compareValor(th,5*numPi/2,false));	*/

	// TRAYECTORIA EN 8.
	//trayectoria8Tiempo();
  trayectoria8Angulo();

	stopTask(updateOdometry);		// Se para la tarea.

	Close(hFileHandle, nIoResult);		// Se cierra el fichero.
}
