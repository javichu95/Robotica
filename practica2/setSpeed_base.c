/*
* Hace que el robot se mueva a una cierta velocidad (lineal o angular) en una trayectoria
* en 8. Actualiza internamente la odometría así que puede saber donde está en cada momento.
*/

#include "mutexLib.c"
#include "positionLib.c"

// Parámetros del robot.
float R = 26.0; // Radio de la rueda.
float L = 87.0; // Longitud del eje.

TPosition robot_odometry;       // Posición para odometría.
TMutex semaphore_odometry = 0;  // Semáforo para odometría.


// Función que calcula la velocidad de cada rueda y se le asigna a los motores.
int setSpeed(float v, float w)
{

// start the motors so that the robot gets v mm/s linear speed and w RADIAN/s angular speed

  float w_l =1, w_r=1;  // Velocidad angular de cada rueda.
  float mR = 1,mL = 1,nR = 0,nL = 0; // Parámetros para la transferencia a los motores.
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

	wait1Msec(1000);     // Se asigna un tiempo de espera.

  return 0;
}

// Función que va actualizando la odometría para saber en todo momento donde
// está el robot.
/*task updateOdometry(){
  int cycle = ??? ; // Número de ciclos para actualizar odometría.
  float dSl,dSr,dx,dy, dT;    // Variables para la odometría.

  while (true){       // Bucle infinito que va actualizando.

    timeAux=nPgmTime;
    // Se leen los tacómetros y se estima cuantos mm se ha movido cada rueda.
    float theta = robot_odometry -> th + w*incTiempo;
    float x = v*incTiempo * cos(w*incTiempo + theta/2);
    float y = v*incTiempo * sin(w*incTiempo + theta/2);

    set_position(robot_odometry, x, y, theta);
    // RESET tachometer right after to start including the "moved" degrees turned in next iteration
    nMotorEncoder[motorC] = 0;
    nMotorEncoder[motorB] = 0;

    // Mostrar cada paso en la pantalla y escribirlo en un fichero.
		nxtDisplayTextLine(2, "ODOMETRY NEW VALUE");
    nxtDisplayTextLine(3, "x,y: %2.2f %2.2f", robot_odometry.x,robot_odometry.y);
    nxtDisplayTextLine(4, "theta: %2.2f ", robot_odometry.th);
	  // Escribir en fichero.





	 // Esperar hasta completar el ciclo.
	 // ...

  }

}*/


// Programa principal que realiza la trayectoria asignando velocidades a las ruedas.
task main()
{
  float pi = 3.14159;
  float v = 0, w = 3.14159; // Velocidades lineal y angular.
  float radio; // Radio de la trayectoria.
  int circunf; // L (semi-dist. between robot wheels)

  // Configuración


  // Resetear valores de odometría y encoders de motores.
  // Encoders a 0.
	nMotorEncoder[motorB] = 0;
	nMotorEncoder[motorC] = 0;
  // Valores de odometría a 0.

  // Se inicializa la tarea de actualizar odometría.

  // Se establecen la velocidad lineal y angular.
	setSpeed(v,w);

	//Trayectoria 8 a partir de aqui.
	//Trayectoria 8 a partir de aqui.
	
  // Se gira el robot 90º.
	w = -pi/2;
	setSpeed(0,-pi/2);	//Velocidad lineal a 0 para que el robot gire sobre si mismo.
	wait1Msec((-pi/2)/w);	//Se espera a que el robot gire -90 grados.
	
  // Se genera la primera parte de la trayectoria.
  
	radio = 40.0;
	v = 20.0;
	w = v/radio;			//Se calcula la velocidad angular.
	setSpeed(v,w);
	wait1Msec(pi/w);		//Se espera el tiempo que tardara en girar 180 grados.

  // Se genera la segunda parte de la trayectoria.
	w = -w;		//Se cambia la dircción del giro.
	setSpeed(v,w);
	wait1Msec((2*pi)/w);	//Se espera el tiempo que tardara el robot en girar 360 grados.


  // Se genera la tercera parte de la trayectoria.
	
	w = -w; //Se vuelve a cambiar la direccion de giro.
	setSpeed(v,w);
	wait1Msec(pi/w);	//Se espera el tiempo que tardara el robot en girar 180 grados.

  // Se genera la cuarta parte de la trayectoria.
	w = pi/2;
	setSpeed(0,w);	//Velocidad lineal a 0 para que el robot gire sobre si mismo.
	wait1Msec((pi/2)/w);	//Se espera a que el robot gire 90 grados.
}
