#pragma config(Sensor, S1,     sonar,          sensorSONAR)
#pragma config(Sensor, S2,     cam,            sensorI2CCustomFastSkipStates)
#pragma config(Sensor, S3,     lightSensor,    sensorLightActive)
#pragma config(Motor,  motorB,          r_motor,       tmotorNXT, PIDControl)
#pragma config(Motor,  motorC,          l_motor,       tmotorNXT, PIDControl)

// Librer�as para el mapa.
#include "mapLib.c"

int valorColor = 45;		// Valor del color para elegir el mapa.
// Coordenadas para la planificaci�n hasta la sala de la pelota.
int finSBlancaX = 4;
int finSBlancaY = 7;
int finSNegraX = 11;
int finSNegraY = 7;
int pelotaX = 7;
int pelotaY = 7;

/*
* M�todo que realiza la S en una cierta direcci�n dada.
*/
void realizarS(bool dir){

	float v = 150.0, w = 0.0;       // Velocidades lineal y angular.
  float radio = 400; 				     	// Radio de la trayectoria.
  float x, y, th;		// Variables para la odometr�a.

	if(dir){			// Se comprueba la direcci�n de la S.
		// Primer tramo: GIRO 90�.
		w = -numPi/2.0;			// Se asigna la velocidad angular.
		setSpeed(0,w);	// Velocidad lineal a 0 para que el robot gire sobre si mismo.
		do {
			readOdometry(x,y,th);
		} while(abs(th) <= 0.1);

		// Segundo tramo: MEDIA CIRCUNFERENCIA.
		w = v/radio;			// Se calcula la velocidad angular.
		setSpeed(v,w);   // Se fijan las velocidades.
		do {
			readOdometry(x,y,th);
		} while(abs(th)-numPi <= 0.1);

		// Tercer tramo: MEDIA CIRCUNFERENCIA.
		setSpeed(v,-w);   // Se fijan las velocidades.
		do {
			readOdometry(x,y,th);
		} while(abs(th) <= 0.1);

		// Cuarto tramo: GIRO 90�.
		w = numPi/2.0;			// Se asigna la velocidad angular.
		setSpeed(0,w);	// Velocidad lineal a 0 para que el robot gire sobre si mismo.
		do {
			readOdometry(x,y,th);
		} while(abs(th-numPi/2) <= 0.1);
	}

	else{			// Se comprueba la direcci�n de la S.
		// Primer tramo: GIRO 90�.
		w = numPi/2.0;			// Se asigna la velocidad angular.
		setSpeed(0,w);	// Velocidad lineal a 0 para que el robot gire sobre si mismo.
		do {
			readOdometry(x,y,th);
		} while(abs(th)-numPi <= 0.1);

		// Segundo tramo: MEDIA CIRCUNFERENCIA.
		w = v/radio;			// Se calcula la velocidad angular.
		setSpeed(v,w);   // Se fijan las velocidades.
		do {
			readOdometry(x,y,th);
		} while(abs(th) <= 0.1);

		// Tercer tramo: MEDIA CIRCUNFERENCIA.
		setSpeed(v,-w);   // Se fijan las velocidades.
		do {
			readOdometry(x,y,th);
		} while(abs(th)-numPi <= 0.1);

		// Cuarto tramo: GIRO 90�.
		w = -numPi/2.0;			// Se asigna la velocidad angular.
		setSpeed(0,w);	// Velocidad lineal a 0 para que el robot gire sobre si mismo.
		do {
			readOdometry(x,y,th);
		} while(abs(th-numPi/2) <= 0.1);
	}
}

/*
* M�todo que ejecuta el circuito si la salida es la blanca.
*/
void ejecutarBlanca(){

	string mapa = "mapaA.txt";		// Se asigna el mapa.
	loadMap(mapa);		// Se carga el mapa A.

	realizarS(true);		// Se hace la S a la derecha.

	planPath(finSBlancaX,finSBlancaY,pelotaX,pelotaY);		// Se planifica el camino desde la celda final.

	encontrarCamino(finSBlancaX,finSBlancaY);		// Se encuentra el camino.

	recorrerCamino();		// Se recorre el camino.

	buscarAtrapar();		// Se busca la pelota y se atrapa.

	// Se detecta la puerta de salida.

	// Se planifica hasta la puerta.

	// Se recorre el camino hasta la salida.

}

/*
* M�todo que ejecuta el circuito si la salida es negra.
*/
void ejecutarNegra(){

	string mapa = "mapaB.txt";		// Se asigna el mapa.
	loadMap(mapa);		// Se carga el mapa B.

	realizarS(false);		// Se hace la S a la derecha.

	planPath(finSNegraX,finSNegraY, pelotaX, pelotaY);		// Se planifica el camino desde la celda final.

	encontrarCamino(finSNegraX,finSNegraY);		// Se encuentra el camino.

	recorrerCamino();		// Se recorre el camino.

	buscarAtrapar();		// Se busca la pelota y se atrapa.

	// Se detecta la puerta de salida.

	// Se planifica hasta la puerta.

	// Se recorre el camino hasta la salida.

}


/*
* M�todo principal que lanza la ejecuci�n de todo el programa.
*/
task main(){

	// Se inicializa la odometr�a y la matriz de conexiones.
	set_position(robot_odometry, INI_X, INI_Y, INI_TH);
	initConnections();		// Se inicializa la matriz de conexiones.

	startTask(updateOdometry);		// Se inicializa la tarea de odometr�a.

	// Se comprueba el color de la l�nea para cargar un mapa u otro.
	if(SensorValue(ligthSensor) > valorColor){		// Salida blanca.
		ejecutarBlanca();
	}
	else{			// Salida negra.
		ejecutarNegra();
	}

	stopTask(updateOdometry);			// Se para la tarea de odometr�a.
}

/*
*	M�todo para lanzar en paralelo la tarea de resetear odometr�a con pared
* de color o X en el suelo.
*/
task resetOdometria(){

	bool cuadradoDetec = false;
	bool xDetec = false;

	while(!cuadradoDetec){

		cuadradoDetec = true;		// Se indica que se ha detectado.
		// Se resetea odometr�a.

	}

	while(!xDetec){

		xDetec = true;
		// Se resetea odometr�a.

	}

}
