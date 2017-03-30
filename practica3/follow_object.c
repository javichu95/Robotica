#pragma config(Sensor, S2,     cam,                 sensorI2CCustomFastSkipStates)
#pragma config(Motor,  motorB,          r_motor,       tmotorNXT, PIDControl)
#pragma config(Motor,  motorC,          l_motor,       tmotorNXT, PIDControl)

// CONFIG camera color position
#define BLUE 1
#define RED 0

// CONFIG GOAL PARAMETERS
#define GOAL_COLOR RED
#define AREA_COLOR 2200


// SUSTITUIR POR LAS LIBRERIAS QUE USEIS para acceder a la cÃ¡mara.
// Distintas versiones tienen nombres ligeramente distintos
#include "G:\robotica\3rd Party Driver Library\include\common.h"
#include "G:\robotica\3rd Party Driver Library\include\nxtcamlib.c"
#include "mutexLib.c"
#include "positionLib.c"

// Parámetros del robot.
float R = 26.0; // Radio de la rueda.
float L = 114.0; // Longitud del eje.
float numPi = 3.14159265;     // Valor de numPi.
float movPinza = 40.0;		//Velocidad de la pinza al cerrarse/abrirse.
float presionPinza = 10.0;	//presion que ejercera cuando este cerrada.
// Par?tros del fichero.
string sFileName = "odometria.txt";     // Nombre del fichero.
TFileIOResult nIoResult;
TFileHandle hFileHandle = 0;
short nFileSize = 20000;			// Tama??el fichero.

TPosition robot_odometry;       // Posición para odometría.
TMutex semaphore_odometry = 0;  // Semáforo para odometría.



/************************************************************************************/
// follow_object.c
// Track a blob of the selected color until the robot is "close enough"
/************************************************************************************/

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
    readOdometry(x,y,theta);			// Se lee la odometr?
    dx = dS * cos(theta + incTheta/2.0);      // Aumento coordenada x.
    dy = dS * sin(theta + incTheta/2.0);      // Aumento coordenada y.

    theta = theta + incTheta;						// Aumento del ?ulo.
    //theta = normalizarAngulo(theta);
    x = x + dx;
    y = y + dy;

    AcquireMutex(semaphore_odometry);     // Se bloquea con el semáforo.
    set_position(robot_odometry, x, y, theta);		 // Se fija la posici??
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
 * Abre la pinza del robot.
 */
 void abrirPinza() {
	hogCPU();
	motor[motorA] = -movPinza;	//Abre la pinza.
	releaseCPU();
	wait1Msec(250);
	hogCPU();
	motor[motorA] = 0;		//Tras un tiempo detiene el motor.
	releaseCPU();
 }

/*
 * Cierra la pinza del robot.
 */
 void cerrarPinza() {
	hogCPU();
	motor[motorA] = movPinza;		//Cierra la pinza.
	releaseCPU();
	wait1Msec(500);
	hogCPU();
	motor[motorA] = presionPinza;		//Una vez cerrada sigue ejerciendo algo de presion.
	releaseCPU();
 }
 
 /*
  * Deja de ejercer presion con la pinza.
  */
 void aflojarPinza() {
	hogCPU();
	motor[motorA] = 0.0;
	releaseCPU();
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
}

/*
 * Método que centra la pelota en la cámara.
 */
float centroPelota(int_array bl, int_array br, int_array bt, int_array bb, int i) {

	// Variables para calcular el error respecto al centro.
	int x_centre, x_error;

		// Fija el centro del blop respecto a la cámara.
		x_centre = bl[i] + br[i];

		// Calcula el error del blop.
		x_error = 176 - x_centre;

		return x_error;
}

/*
 * Mueve el robot para buscar la pelota.
 */
void buscar(bool derecha) {
	if(derecha) {
		setSpeed(0,-numPi);		// Se fija la velocidad angular para dar vueltas.
	} else {
		setSpeed(0,numPi);		// Se fija la velocidad angular para dar vueltas.
	}
}

/*
 * Avanza hacia la pelota y si la tiene a la distancia adecuada la coge.
 */
 bool atraparPelota(float area, float w) {
	nxtDisplayTextLine(5,"%.2f",area);
	if(area < AREA_COLOR) {		//Si el area no es grande avanza hacia la pelota.
		setSpeed(400,w);
		wait1MSec(1000);
		return false;		//Indica si ha cogido la pelota.
	} else {			//Si el area es suficientemente grande intenta cogerla.
		setSpeed(0, numPi/8);
		wait1MSec(1000);
		setSpeed(70,0);
		wait1MSec(2000);
		setSpeed(0, 0);	//Detiene al robot.
		cerrarPinza();	//Cierra la pinza.
		return true;	//indica si ha cogido la pelota.
	}
 }

/*
 * Método principal que centra al cobor y coge la pelota.
 */
task main (){

	bool continueTracking = true;		// Booleano para indicar si se sigue el tracking.
	int _nblobs = 0;		// Número de blops detectados.
	int_array bc, bl, bt, br, bb;			// Variables para la detección de la cámara.
	bool encontrada = false;			// Booleano para indicar si se ha encontrado la pelota.
	init_camera(cam);					// Se inicializa la cámara.
	float areaMayor = 0.0;
	int blopMayor = 0;
	int lastBloopIzq = 0;
	
	while (continueTracking) {			// Mientras se deba continuar el tracking...

		// Se obtienen los blops.
		get_blobs(cam, _nblobs, bc, bl, bt, br, bb);
		nxtDisplayTextLine(1, "%d", _nblobs);

		for (int i = 0; i < _nblobs; i++) {		// Se recorren los blops.

			if (bc[i] == GOAL_COLOR){			// Si el color coincide...
				nxtDisplayTextLine(0,"vistoooo");
				encontrada = true;
				float area = (br[i] - bl[i])*(bb[i]-bt[i]);
				if(area > areaMayor) {
					areaMayor = area;
					blopMayor = i;
					int lastBloopIzq = bl[i];
				}

			}

		}

		nxtDisplayTextLine(6,"%.2f",areaMayor);
		if(!encontrada){		// Si no encuentra la pelota, la busca.
			buscar(lastBloopIzq > 176);
		}else{			// Si la ha encontrado...
				float error = centroPelota(bl, br, bt, bb, blopMayor);	//Centra la pelota.
				nxtDisplayTextLine(7,"%.2f",error);
				float angular = error*0.002;
				if(atraparPelota(areaMayor,angular)) {	//Avanza para coger la pelota.
					//QUIZA DEBAMOS HACER UNA COMPROBACION O ALGO
					continueTracking = false;		//Si ha atrapado la pelota finaliza.
				}
				encontrada = false;
		}
	}
	aflojarPinza();
	wait1MSec(5000);
	abrirPinza();
}
