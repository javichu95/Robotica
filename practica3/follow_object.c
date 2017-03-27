#pragma config(Sensor, S2,     cam,                 sensorI2CCustomFastSkipStates)
#pragma config(Motor,  motorB,          r_motor,       tmotorNXT, PIDControl)
#pragma config(Motor,  motorC,          l_motor,       tmotorNXT, PIDControl)

// CONFIG camera color position
#define BLUE 2
#define RED 1

// CONFIG GOAL PARAMETERS
#define GOAL_COLOR RED
#define AREA_COLOR 100


// SUSTITUIR POR LAS LIBRERIAS QUE USEIS para acceder a la c√°mara.
// Distintas versiones tienen nombres ligeramente distintos
#include "G:\robotica\3rd Party Driver Library\include\common.h"
#include "nxtcamlib.c"
#include "mutexLib.c"
#include "positionLib.c"


/************************************************************************************/
// follow_object.c
// Track a blob of the selected color until the robot is "close enough"
/************************************************************************************/


// Par√°metros del robot.
float R = 26.0; // Radio de la rueda.
float L = 114.0; // Longitud del eje.
float numPi = 3.14159265;     // Valor de numPi.
// Par·metros del fichero.
string sFileName = "odometria.txt";     // Nombre del fichero.
TFileIOResult nIoResult;
TFileHandle hFileHandle = 0;
short nFileSize = 20000;			// TamaÒo del fichero.

TPosition robot_odometry;       // Posici√≥n para odometr√≠a.
TMutex semaphore_odometry = 0;  // Sem√°foro para odometr√≠a.

/*
* Funci√≥n que lee la posici√≥n del robot.
*/
void readOdometry(float &x, float &y, float &theta) {

	AcquireMutex(semaphore_odometry);        // Se bloquea en el sem√°foro.
  	// Se leen las variables.
	x = robot_odometry.x;
	y = robot_odometry.y;
	theta = robot_odometry.th;
	ReleaseMutex(semaphore_odometry);      // Se desbloquea el sem√°foro.

}

/*
* Funci√≥n que calcula la velocidad de cada rueda y se le asigna a los motores.
*/
int setSpeed(float v, float w){

	float w_l = 0.0, w_r = 0.0;  // Velocidad angular de cada rueda.
	float mR = 5.81155692, mL = 5.80103454, nR = 0.3985, nL = 0.5209; // Par√°metros para la transferencia a los motores.
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
* Funci√≥n que va actualizando la odometr√≠a para saber en todo momento donde
* est√° el robot.
*/
task updateOdometry(){

  int cycle = 50; // N√∫mero de ciclos para actualizar odometr√≠a.
  float dSl = 0.0, dSr = 0.0, dx = 0.0, dy = 0.0, dS = 0.0;    // Variables para la odometr√≠a.
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
    // Se leen los tac√≥metros.
    ruedaDer = nMotorEncoder[motorB];
    ruedaIzq = nMotorEncoder[motorC];
    nMotorEncoder[motorC] = 0.0;
    nMotorEncoder[motorB] = 0.0;
    releaseCPU();     // Se libera la CPU.

    // Se calculan los par√°metros.
    dSl = 2*numPi*R*(ruedaIzq/360.0);     // Aumento rueda izquierda.
    dSr = 2*numPi*R*(ruedaDer/360.0);     // Aumento rueda derecha.
    dS = (dSl + dSr)/2.0;						// Cuanto ha avanzado el robot en el ciclo.
    incTheta = (dSr - dSl)/L;				// Angulo girado por el robot en el ciclo.
    readOdometry(x,y,theta);			// Se lee la odometrÌa.
    dx = dS * cos(theta + incTheta/2.0);      // Aumento coordenada x.
    dy = dS * sin(theta + incTheta/2.0);      // Aumento coordenada y.

    theta = theta + incTheta;						// Aumento del ·ngulo.
    //theta = normalizarAngulo(theta);
    x = x + dx;
    y = y + dy;

    AcquireMutex(semaphore_odometry);     // Se bloquea con el sem√°foro.
    set_position(robot_odometry, x, y, theta);		 // Se fija la posiciÛn.
    ReleaseMutex(semaphore_odometry);     // Se librea con el sem√°foro.

    // Mostrar cada paso en la pantalla.
    /*nxtDisplayTextLine(2, "ODOMETRY NEW VALUE");
    nxtDisplayTextLine(3, "x,y: %2.2f %2.2f", robot_odometry.x, robot_odometry.y);
    nxtDisplayTextLine(4, "theta: %2.2f ", robot_odometry.th);*/

    // Par√°metros para escribir en el fichero.
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
	if(actual >= (objetivo - eps) && actual <= (objetivo + eps)) {
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
 * MÈtodo que centra la pelota en la c·mara.
 */
bool centrarPelota(int_array bl, int_array br, int_array bt, int_array bb) {

	// Variables para calcular el error respecto al centro.
	int x_centre, x_error;
	int y_centre, y_error;
	bool erased = false;

		if (!erased) {		
			nxtDisplayTextLine(0,"Tracking ...");
			erased = true;
		}

		// Fija el centro del blop respecto a la c·mara.
		x_centre = bl[0] + br[0];
		y_centre = bt[0] + bb[0];

		// Calcula el error del blop.
		x_error = 176 - x_centre;
		y_error = 143 - y_centre;

		if(abs(x_error)  < 0.15) {	// Si es menor es que ya est· centrada.
			return true;
		}

		// Da velocidades al motor para centrarlo.
		motor[l_motor] = (y_error - x_error) / 5;
		motor[r_motor] = (y_error + x_error) / 5;

		return false;
}

/*
 * MÈtodo principal que centra al cobor y coge la pelota.
 */
task main (){

	bool continueTracking = true;		// Booleano para indicar si se sigue el tracking.
	int _nblobs = 0;		// N˙mero de blops detectados.
  	int_array bc, bl, bt, br, bb;			// Variables para la detecciÛn de la c·mara.
  	bool encontrada = false;			// Booleano para indicar si se ha encontrado la pelota.
	//startTask(updateOdometry);		// Se inicializa la odometrÌa.
	init_camera(cam);					// Se inicializa la c·mara.			

	while (continueTracking) {			// Mientras se deba continuar el tracking...

		// Se obtienen los blops.
		get_blobs(cam, _nblobs, bc, bl, bt, br, bb);
		nxtDisplayTextLine(1, "%d", _nblobs);

		for (int i = 0; i < _nblobs; i++) {		// Se recorren los blops.
		    if (bc[i] == GOAL_COLOR){		// Si el color coincide...
				encontrada = true;
			}
		}		

		if(!encontrada){		// Si no encuentra la pelota, da vueltas.
			setSpeed(0,numPi/2);		// Se fija la velocidad angular para dar vueltas.
			wait1Msec(1000);			// Se espera un tiempo.
			setSpeed(0,0);			// Se para el robot.
		} else{			// Si la ha encontrado...
			if(centrarPelota(bl, br, bt, bb)) {		// Se centra al robot.
				continueTracking = false;
			}
			setSpeed(0,0);			// Se para al robot.
		}
	}

	// L√≠mite en encoder.

	// Aplicar velocidad hasta cerrar la garra (== encoder).


}
