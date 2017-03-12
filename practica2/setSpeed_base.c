/*
* Hace que el robot se mueva a una cierta velocidad (lineal o angular) en una trayectoria
* en 8. Actualiza internamente la odometría así que puede saber donde está en cada momento.
*/

#include "mutexLib.c"
#include "positionLib.c"

// Parámetros del robot.
float R = 26.0; // Radio de la rueda.
float L = 115.0; // Longitud del eje.

TPosition robot_odometry;       // Posición para odometría.
TMutex semaphore_odometry = 0;  // Semáforo para odometría.


// Función que calcula la velocidad de cada rueda y se le asigna a los motores.
int setSpeed(float v, float w)
{

// start the motors so that the robot gets v mm/s linear speed and w RADIAN/s angular speed

  float w_l =1, w_r=1;  // Velocidad angular de cada rueda.
  float mR = 5.8115, mL = 5.8010, nR = 0.3985, nL = 0.5209; // Parámetros para la transferencia a los motores.
  float motorPowerRight = 15, motorPowerLeft = 15;  // Potencia de cada motor.

  // Calculamos la velocidad angular de cada rueda.
  w_r = v/R + w*L/(2.0*R);
  w_l = v/R - w*L/(2.0*R);

  // Se calcula la potencia del motor.
  motorPowerLeft = mL*w_l + nL;
  motorPowerRight = mR*w_r + nR;

	// Se asigna la potencia al motor.
	motor[motorB] = motorPowerRight;
	motor[motorC] = motorPowerLeft;
	//motor[motorA] = 10;

	//wait1Msec(4000);     // Se asigna un tiempo de espera.

  return 0;
}

// Función que va actualizando la odometría para saber en todo momento donde
// está el robot.
/*task updateOdometry(){
  int cycle = ??? ; // Número de ciclos para actualizar odometría.
  float dSl,dSr,dx,dy, dT;    // Variables para la odometría.

  while (true){       // Bucle infinito que va actualizando.

    timeAux=nPgmTime;
    hogCPU();     // Se bloquea la CPU.
    // Se leen los tacómetros.
    float ruedaDer = nMotorEncoder[motorB];
    float ruedaIzq = nMotorEncoder[motorC];
    releaseCPU();     // Se libera la CPU.

    AcquireMutex();     // Se bloquea con el semáforo.
    // Se calculan los parámetros.
    dSl = ;     // Aumento rueda izquierda.
    dSr = ;     // Aumento rueda derecha.
    dx = dS * cos(incTheta + incTheta/2);      // Aumento coordenada x.
    dy = dS * sin(incTheta + incTheta/2);;      // Aumento coordenada y.
    float theta = (robot_odometry -> th) + incTheta;
    float x = (robot_odometry -> x) + dx;
    float y = (robot_odometry -> y) + dy;

    set_position(robot_odometry, x, y, theta);
    // RESET tachometer right after to start including the "moved" degrees turned in next iteration
    nMotorEncoder[motorC] = 0;
    nMotorEncoder[motorB] = 0;
    ReleaseMutex();     // Se librea con el semáforo.

    // Mostrar cada paso en la pantalla.
    nxtDisplayTextLine(2, "ODOMETRY NEW VALUE");
    nxtDisplayTextLine(3, "x,y: %2.2f %2.2f", robot_odometry.x, robot_odometry.y);
    nxtDisplayTextLine(4, "theta: %2.2f ", robot_odometry.th);

    // Parámetros para escribir en el fichero.
    string sFileName = "odometria.txt";     // Nombre del fichero.
    string sString;     // Variable para guardar el string.
    TFileIOResult nIoResult;
    TFileHandle hFileHandle;
    int nFileSize = 200; //1 byte each char...
    
    // Se escribe en el fichero.
    StringFormat(sString, "%2.2f %2.2f %2.2f \n", x, y, theta);
    WriteText(hFileHandle, nIoResult, sString);

    // Esperar hasta completar el ciclo.
    wait1Msec(cycle-(nPgmTime-timeAux));

  }

}*/


// Programa principal que realiza la trayectoria asignando velocidades a las ruedas.
task main()
{
  float pi = 3.14159;
  float v = 100, w = 0; // Velocidades lineal y angular.
  float radio; 					// Radio de la trayectoria.
  int circunf;					 // L (semi-dist. between robot wheels)

  // Configuración


  // Resetear valores de odometría y encoders de motores.
  // Encoders a 0.
	nMotorEncoder[motorB] = 0;
	nMotorEncoder[motorC] = 0;
  // Valores de odometría a 0.

  // Se inicializa la tarea de actualizar odometría.

  // Se establecen la velocidad lineal y angular.
	//setSpeed(100,w);
	//wait1Msec(4000);

	//Trayectoria 8 a partir de aqui.

  // Se gira el robot 90º.
	w = -pi/2;
	setSpeed(0,-pi/2);	//Velocidad lineal a 0 para que el robot gire sobre si mismo.
	wait1Msec(((-pi/2)/w)*1000);	//Se espera a que el robot gire -90 grados.*/

  // Se genera la primera parte de la trayectoria.

	radio = 400.0;
	v = 150.0;
	w = v/radio;			//Se calcula la velocidad angular.
	setSpeed(v,w);
	wait1Msec((pi/w)*1000);		//Se espera el tiempo que tardara en girar 180 grados.

  // Se genera la segunda parte de la trayectoria.
	w = -w;		//Se cambia la dircción del giro.
	setSpeed(v,w);
	wait1Msec((-2*pi/w)*1000);//Se espera el tiempo que tardara el robot en girar 360 grados.


  // Se genera la tercera parte de la trayectoria.

	w = -w; //Se vuelve a cambiar la direccion de giro.
	setSpeed(v,w);
	wait1Msec((pi/w)*1000);	//Se espera el tiempo que tardara el robot en girar 180 grados.

  // Se genera la cuarta parte de la trayectoria.
	w = pi/2;
	setSpeed(0,w);	//Velocidad lineal a 0 para que el robot gire sobre si mismo.
	wait1Msec(((pi/2)/w)*1000);	//Se espera a que el robot gire 90 grados.


}
