#pragma config(Sensor, S1,     sonar,          sensorSONAR)
#pragma config(Sensor, S2,     cam,            sensorI2CCustomFastSkipStates)
#pragma config(Sensor, S4,     lightSensor,    sensorLightActive)
#pragma config(Motor,  motorB,          r_motor,       tmotorNXT, PIDControl)
#pragma config(Motor,  motorC,          l_motor,       tmotorNXT, PIDControl)

// Librerías para el mapa.
#include "mapLib.c"

short valorColor = 40;		// Valor del color para elegir el mapa.
// Coordenadas para la planificación hasta la sala de la pelota.
int inicialBlancaX = 3;
int inicialBlancaY = 15;
int inicialNegraX = 11;
int inicialNegraY = 15;
int finSBlancaX = 3;
int finSBlancaY = 7;
int finSNegraX = 11;
int finSNegraY = 7;
int pelotaX = 7;
int pelotaY = 7;
float epsIzq = 0.1;
float epsDer = 0.05;
float epsGiro90 = 0.08;

/*
* Método que realiza la S en una cierta dirección dada.
*/
void realizarS(bool dir){

	float v = 150.0, w = 0.0;       // Velocidades lineal y angular.
  float radio = 400; 				     	// Radio de la trayectoria.
  float x, y, th;		// Variables para la odometría.

	if(dir){			// Se comprueba la dirección de la S.
		// Segundo tramo: MEDIA CIRCUNFERENCIA.
		w = v/radio;			// Se calcula la velocidad angular.
		setSpeed(v,w);   // Se fijan las velocidades.
		wait1Msec(abs(numPi/w)*1000.0);
		/*do {
			readOdometry(x,y,th);
			nxtDisplayTextLine(1,"%f",th);
		} while(abs(th) <= numPi - epsIzq);*/
		// Tercer tramo: MEDIA CIRCUNFERENCIA.
		setSpeed(v,-w);   // Se fijan las velocidades.
		wait1Msec(abs(numPi/w)*1000.0);
		/*do {
			readOdometry(x,y,th);
			nxtDisplayTextLine(1,"%f",th);
		} while(abs(th) >= epsDer);*/
		// Cuarto tramo: GIRO 90º.
		w = numPi/2.0;			// Se asigna la velocidad angular.
		setSpeed(0,w);	// Velocidad lineal a 0 para que el robot gire sobre si mismo.
		wait1Msec(abs(numPi/2.0/w)*1000.0);
		/*do {
			readOdometry(x,y,th);
			nxtDisplayTextLine(1,"%f",th);
		} while(abs(th) <= numPi/2 - epsGiro90);*/
	}

	else{			// Se comprueba la dirección de la S.
		w = v/radio;			// Se calcula la velocidad angular.
		readOdometry(x,y,th);
		setSpeed(v,-w);   // Se fijan las velocidades.
		wait1Msec(abs(numPi/w)*1000.0);
		/*do {
			readOdometry(x,y,th);
			nxtDisplayTextLine(1,"%f",th);
		} while(abs(th) >= epsDer);*/
		// Tercer tramo: MEDIA CIRCUNFERENCIA.
		setSpeed(v,w);   // Se fijan las velocidades.
		wait1Msec(abs(numPi/w)*1000.0);
		/*do {
			readOdometry(x,y,th);
			nxtDisplayTextLine(1,"%f",th);
		} while(abs(th) <= numPi - epsIzq);*/
		// Cuarto tramo: GIRO 90º.
		w = numPi/2.0;			// Se asigna la velocidad angular.
		setSpeed(0,-w);	// Velocidad lineal a 0 para que el robot gire sobre si mismo.
		wait1Msec(abs(numPi/2.0/w)*1000.0);
		/*do {
			readOdometry(x,y,th);
			nxtDisplayTextLine(1,"%f",th);
		} while(abs(th) >= numPi/2 + epsGiro90);*/
	}
}

/*
* Método que ejecuta el circuito si la salida es la blanca.
*/
void ejecutarBlanca(){

	string mapa = "mapaA.txt";		// Se asigna el mapa.
	loadMap(mapa);		// Se carga el mapa A.

	realizarS(true);		// Se hace la S a la derecha.

	// Se planifica el camino desde la celda final.
	planPath(finSBlancaX, finSBlancaY, pelotaX, pelotaY,
				inicialBlancaX, inicialBlancaY);

	encontrarCamino(finSBlancaX,finSBlancaY);		// Se encuentra el camino.

	string file = "grid.txt";
	TFileIOResult nIoCuadricula;
	TFileHandle hFileHandleCuad = 0;
	short nFileSizeCuad = 2000;			// Tamaño del fichero.

	OpenWrite(hFileHandleCuad, nIoCuadricula, file, nFileSizeCuad);
	string sString;

	for(int i = 2*sizeY; i >= 0; i--) {
		for(int j = 0; j <= 2*sizeX; j++) {
			stringFormat(sString, "%d ", grid[j][i]);
    	WriteText(hFileHandleCuad, nIoCuadricula, sString);
		}
		 WriteText(hFileHandleCuad, nIoCuadricula, "\n");
	}

	for(int j = 0; j <= 20; j++) {
		stringFormat(sString, "%d ", pathX[j]);
    WriteText(hFileHandleCuad, nIoCuadricula, sString);
	}
   WriteText(hFileHandleCuad, nIoCuadricula, "\n");

   for(int i = 0; i <= 20; i++) {
    	stringFormat(sString, "%d ", pathY[i]);
    	WriteText(hFileHandleCuad, nIoCuadricula, sString);
    }

	recorrerCamino();		// Se recorre el camino.

	//buscarAtrapar();		// Se busca la pelota y se atrapa.

	// Se detecta la puerta de salida.

	// Se planifica hasta la puerta.

	// Se recorre el camino hasta la salida.

}

/*
* Método que ejecuta el circuito si la salida es negra.
*/
void ejecutarNegra(){

	string mapa = "mapaB.txt";		// Se asigna el mapa.
	loadMap(mapa);		// Se carga el mapa B.

	realizarS(false);		// Se hace la S a la derecha.

	planPath(finSNegraX, finSNegraY, pelotaX, pelotaY,
				inicialNegraX, inicialNegraY);

	encontrarCamino(finSNegraX,finSNegraY);		// Se encuentra el camino.

	//drawMap();

	recorrerCamino();		// Se recorre el camino.

	//buscarAtrapar();		// Se busca la pelota y se atrapa.

	// Se detecta la puerta de salida.

	// Se planifica hasta la puerta.

	// Se recorre el camino hasta la salida.

}


/*
* Método principal que lanza la ejecución de todo el programa.
*/
task main(){

	/*initConnections();

	set_position(robot_odometry, INI_X, INI_Y, INI_TH);

	string mapa = "mapa2.txt";

	loadMap(mapa);

	drawMap();

	planPath(11,9,finSBlancaX,finSBlancaY);

  encontrarCamino(11,9);

  startTask(updateOdometry);
  recorrerCamino();
  stopTask(updateOdometry);

	string file = "grid.txt";
	TFileIOResult nIoCuadricula;
	TFileHandle hFileHandleCuad = 0;
	short nFileSizeCuad = 2000;			// Tamaño del fichero.

	OpenWrite(hFileHandleCuad, nIoCuadricula, file, nFileSizeCuad);
	string sString;

	for(int i = 2*sizeY; i >= 0; i--) {
		for(int j = 0; j <= 2*sizeX; j++) {
			stringFormat(sString, "%d ", grid[j][i]);
    	WriteText(hFileHandleCuad, nIoCuadricula, sString);
		}
		 WriteText(hFileHandleCuad, nIoCuadricula, "\n");
	}

	for(int j = 0; j <= 20; j++) {
		stringFormat(sString, "%d ", pathX[j]);
    WriteText(hFileHandleCuad, nIoCuadricula, sString);
	}
   WriteText(hFileHandleCuad, nIoCuadricula, "\n");

   for(int i = 0; i <= 20; i++) {
    	stringFormat(sString, "%d ", pathY[i]);
    	WriteText(hFileHandleCuad, nIoCuadricula, sString);
	}*/


	// Se inicializa la odometría y la matriz de conexiones.
	initConnections();		// Se inicializa la matriz de conexiones.

	startTask(updateOdometry);		// Se inicializa la tarea de odometría.
	// Se comprueba el color de la línea para cargar un mapa u otro.
	if(SensorValue(lightSensor) > valorColor){		// Salida blanca.

		set_position(robot_odometry, INI_X, INI_Y, numPi);
		nxtDisplayTextLine(1,"BLANCO");
		ejecutarBlanca();
	}
	else{			// Salida negra.
		set_position(robot_odometry, INI_X, INI_Y, 0.0);
		nxtDisplayTextLine(1,"NEGRO");
		ejecutarNegra();
	}

	stopTask(updateOdometry);			// Se para la tarea de odometría.

}

/*
*	Método para lanzar en paralelo la tarea de resetear odometría con pared
* de color o X en el suelo.
*/
task resetOdometria(){

	bool cuadradoDetec = false;
	bool xDetec = false;

	while(!cuadradoDetec){

		cuadradoDetec = true;		// Se indica que se ha detectado.
		// Se resetea odometría.

	}

	while(!xDetec){

		xDetec = true;
		// Se resetea odometría.

	}

}
