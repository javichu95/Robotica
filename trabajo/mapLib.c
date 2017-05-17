/////////////////////////////////////////////////////////////////////////////////////////
//  Conjunto de funciones que se encargan de planificar un camino hasta una celda      //
//  destino, ejecutar dicho camino y replanificar la ruta en caso de detectar un       //
//  obstáculo con el sonar.                                                            //
/////////////////////////////////////////////////////////////////////////////////////////

#include "cogerPelota.c"

// Número máximo de celdas que se pueden utilizar.
#define MAX_X 10
#define MAX_Y 10

int distancia = 34;		// Distancia al obstáculo.

// Variables globales de las dimensiones del mapa
int sizeX;			// Tamaño en el eje X.
int sizeY;			// Tamaño en el eje Y.
int sizeCell;		// Tamaño de la celda.
int pixPerX;		// Píxeles por eje X.
int pixPerY;		// Píxeles por eje Y.

// Coordenadas de la celda final e inicial.
int celdaXFin;
int celdaYFin;
int celdaOdoX;
int celdaOdoY;

// Frontera que indica las posiciones de la cuadricula a actualizar.
int fronteraX[MAX_X*MAX_Y];
int fronteraY[MAX_X*MAX_Y];

// Indica en que coordenada se la tira blanca.
int xTiraBlanca = 0;

// Booleano para indicar que es el camino inicial.
bool isPrimero = true;

// Matriz de conexiones.
bool connectionsMatrix[2*MAX_X+1][2*MAX_Y+1];

// Matrices con los caminos.
int pathX[MAX_X*MAX_Y];
int pathY[MAX_X*MAX_Y];

// Cuadrícula para encontrar el camino.
int grid[MAX_X*2][MAX_Y*2];

/*
* Inicializa todas las conexiones a falso.
*/
void initConnections(){

  for(int i=0; i<2*MAX_X+1; ++i){
    for (int j=0; j<2*MAX_Y+1; ++j){
      connectionsMatrix[i][j]=false;
    }
  }

}

/*
* Método que lee el mapa del fichero.
*/
bool readLineHeader(TFileHandle hFileHandle,TFileIOResult nIoResult, int & dimX, int & dimY, int &dimCell){

    //unsigned ans;
    //short ans;
    int ind = 1;
    //float aux = 0.1;
    bool eol = false;
    bool endfile = false;
    char onechar;
    char linechar[40];
    int num = 0;
    int indNum=0;
    int numbersRead[3];

    // read header
		while(!eol){
		  ReadByte(hFileHandle, nIoResult, onechar);
		  if ( nIoResult == 0 ){ // all ok
          if (onechar==13) // end of line
          {
	          linechar[ind-1]=0;
	          eol=true;
          }
          else{
            if (onechar=='\n'){ // line jump
              //skip
            }
            else{
              linechar[ind-1]=onechar;
              if (onechar==' '){
                numbersRead[indNum]=num;
                num=0;
                indNum++;
              }else{
                num = 10*num + (onechar - '0' );
              }
              ind++;
            }
          }
       }
       else{
            if (nIoResult==ioRsltEndOfFile){
              eol=true;
              endfile=true;

            }else{
              //nxtDisplayTextLine(1, "PROBLEM READING map");
            }
        }
     }
     // from char to string
     //StringFromChars(linestring,linechar);
     if (numbersRead[indNum]!=num && num!=0){
        numbersRead[indNum]=num;
     }

     dimX = numbersRead[0];
     dimY = numbersRead[1];
     dimCell = numbersRead[2];

     return endfile;

}

/*
* Método que lee la siguiente línea del fichero.
*/
bool readNextLine(TFileHandle hFileHandle,TFileIOResult & nIoResult, int & mapRow){

    //short ans;

    int ind = 0; // pointer to keep all text read in vector linechar
    char linechar[(MAX_X*2+1)*3]; // how long do we expect the lines...
    string linestring;
    char onechar;

    bool eol = false;
    bool endfile = false;
    int mapCol=0;

    // read header
		while(!eol){
		  ReadByte(hFileHandle, nIoResult, onechar);
		  if ( nIoResult == 0 ){ // all ok
          if (onechar==13) // end of line
          {
	          linechar[ind]=0;
	          eol=true;
          }
          else{
            if (onechar=='\n'){ // line jump
              //skip
            }
            else{
              linechar[ind]=onechar;
              if (onechar==' '){
                //numbersRead[indNum]=num;
                //num=0;
                //indNum++;
              }else{
                if (onechar=='1'){
                  //nxtDisplayTextLine(3, " %d %d", mapCol,mapRow);
                  connectionsMatrix[mapCol][mapRow]=true;
                }
                // else { false} // by default is false
                mapCol++;

              }
              ind++;
            }
          }
       }
       else{
            if (nIoResult==ioRsltEndOfFile){
              eol=true;
              endfile=true;

            }else{
              //nxtDisplayTextLine(1, "PROBLEM READING map");
            }
        }
     }

     // jump to next row
     mapRow--;
     if (mapRow<0){
        // STOP READING, map is full
        endfile=true;
     }

     // from char to string
     stringFromChars(linestring,linechar);
     /*if (numbersRead[indNum]!=num && num!=0){
        numbersRead[indNum]=num;
     }*/

     //nxtDisplayTextLine(3, "%s ", linestring);

     /*for(int j=2; j<=indNum; ++j){
        setConnection(numbersRead[0], numbersRead[1], numbersRead[j]);
        nxtDisplayTextLine(4, "%d connection open", numbersRead[j]);
        //wait10Msec(200);
     }*/
     return endfile;

	}

/*
* Método que carga el mapa desde un fichero.
*/
bool loadMap(string mapFileName){

     bool loadingOk=false;
     int mapRow; // last row from connection matrix

     string line="";
     bool eof = false;
     TFileIOResult nIoResult;
     TFileHandle hFileHandle;
     short nFileSize = 0; // it is filled when we open file

     CloseAllHandles(nIoResult);
     hFileHandle = 0;
	   //nxtDrawLine(_x+2, _y, _x-2, _y);

	   OpenRead(hFileHandle, nIoResult, mapFileName, nFileSize);
	   if( nIoResult ==0 ){
	       //nxtDisplayTextLine(1, "OPEN OK: %d", nFileSize);

	       //StringFromChars(sToString, FromChars)
         //Converts an array of bytes to a string value.  You MUST end your char array with a char value of zero!

	        // read first line
          eof = readLineHeader(hFileHandle,nIoResult, sizeX, sizeY, sizeCell);
          //nxtDisplayTextLine(2, "%s", line);
          mapRow=2*sizeY;
          // read rest of data
          while(!eof){
            eof = readNextLine(hFileHandle,nIoResult, mapRow);
            //eof = readNextCellConnections(hFileHandle,nIoResult);
            //nxtDisplayTextLine(2, "%s", line);
	        }
	        loadingOk=true;
	        Close(hFileHandle, nIoResult);
     }
     else{
           loadingOk=false;
           //nxtDisplayTextLine(1, "PROBLEM OPENING file");
     }

	   return loadingOk;

}

/*
 * Método que dibuja el mapa por pantalla.
 */
void drawMap(){

  int i,j,cx,cy;

  //eraseDisplay(); // L_B: (0,0); T_T: (99,63)
  //nxtDrawRect(_l, _t, _r, _b);
  pixPerX=100/sizeX;
  pixPerY=64/sizeY;

  nxtDrawRect(0,sizeY*pixPerY,sizeX*pixPerX,0);

  //nxtDrawLine(xPos, yPos, xPosTo, yPosTo);
  //i=cellX*sizeY+cellY;

  // check "vertical" walls
  for (i=2; i<2*sizeX; i=i+2){
    for (j=1; j< 2*sizeY; j=j+2){
      if (connectionsMatrix[i][j]==false){
          // paint "right" wall from cell (i/2-1, j2-1)
          cx=(i-1)/2;
          cy=(j-1)/2;
          nxtDrawLine((cx+1)*pixPerX, cy*pixPerY, (cx+1)*pixPerX, (cy+1)*pixPerY);
      }
    }
  }

  // check "horizontal" walls
  for (j=2; j<2*sizeY; j=j+2){
    for (i=1; i< 2*sizeX; i=i+2){
      if (connectionsMatrix[i][j]==false){
          // paint "top" wall from cell (i-1)/2, (j-1)/2)
          cx=(i-1)/2;
          cy=(j-1)/2;
          nxtDrawLine((cx)*pixPerX, (cy+1)*pixPerY, (cx+1)*pixPerX, (cy+1)*pixPerY);
      }
    }
  }

}

/*
* Método que redondea las coordenadas por si el robot se ha desviado
* y devuelve la celda relativa en la que está.
*/
int redondearCoord(float coord){

	float div = coord / sizeCell;		// Se obtiene la celda en la que está.
	float resto = div - (int)(div);	// Se saca el resto de la división.
	int signo = 1, resultado = 0;		// Variables para el signo y resultado.

	if(div < 0){		// Se comprueba si es negativo.
		signo = -1;		// Se saca el signo.
		div = -div;
	}

	if(resto < 0){	// Se comprueba si es negativo.
		resto = -resto;
	}

	if(resto >= 0.55){		// Si el resto es mayor que 0.75...
		resultado = ((((int)(div))+1)*2)*signo;		// Se asigna a la siguiente celda.
	} else{			// Si no es mayor que 0.75...
		resultado = ((int)(div));			// Se asigna la celda actual.
		resultado = resultado*signo*2;
	}

	return resultado;			// Se devuelve el resultado.

}

/*
 * Método que dibuja el robot en la pantalla.
 */
void drawRobot(float x_mm, float y_mm, float ang_rad, int inix, int iniy){

  int cellx,celly;
  int pixX,pixY;
  float ang_grad;
  int th;

  cellx = (redondearCoord(x_mm) + inix)/2;
  celly = (redondearCoord(y_mm) + iniy)/2;

  pixX=cellx*pixPerX+pixPerX/2;
  pixY=celly*pixPerY+pixPerY/2;
  nxtFillEllipse(pixX-1, pixY+1, pixX+1, pixY-1); //nxtFillEllipse(Left, Top, Right, Bottom);

  //normalizeAngle(ang_rad);
  ang_grad=radiansToDegrees(ang_rad);
  if (ang_grad<0){ ang_grad=ang_grad+360;}
  th=(ang_grad+22.5)/45;
  while(th>7){th=th-8;}

	// Pinta la orientación.
	if(th==0)		    { nxtDrawLine(pixX,pixY,pixX+2,pixY);		}
	else if(th==1)	{ nxtDrawLine(pixX,pixY,pixX+2,pixY+2);	}
	else if(th==2)	{ nxtDrawLine(pixX,pixY,pixX,pixY+2);	  }
	else if(th==3)	{ nxtDrawLine(pixX,pixY,pixX-2,pixY+2);	}
	else if(th==4)	{ nxtDrawLine(pixX,pixY,pixX-2,pixY);		}
	else if(th==5)	{ nxtDrawLine(pixX,pixY,pixX-2,pixY-2);	}
	else if(th==6)	{ nxtDrawLine(pixX,pixY,pixX,pixY-2);		}
	else if(th==7)	{ nxtDrawLine(pixX,pixY,pixX+2,pixY-2);	}

}


/*
 * Método que asigna el valor -1 a obstáculos.
 */
void asignarValores(){

	// Se recorre la matriz para poner los obstáculos.
  for(int i = 0; i <= 2*sizeX; i++){
		for(int j = 0; j <= 2*sizeY; j++){
			if(!connectionsMatrix[i][j]) {
				grid[i][j] = -1;
			}
		}
	}

}

/*
* Método que inicializa la cuadrícula con valor -2.
*/
void iniciarGrid(){

	// Recorre la cuadrícula inicializandola.
	for(int i = 0; i <= 2*sizeX; i++){
		for(int j = 0; j <= 2*sizeY; j++){
			grid[i][j] = -2;
		}
	}

}

/*
 * Método que va asignando costes iterativamente a la cuadrícula.
 */
void planPath(int x_end, int y_end) {

	// Se inicializan valores de la frontera.
	for(int i = 0; i < MAX_X*MAX_Y; i++) {
		fronteraX[i] = -1;
		fronteraY[i] = -1;
	}

	int leer = 0, escribir = 0;	 // Índices de lectura y escritura en la frontera.
	int numElementos = 0;		// Elementos que quedan por leer.
	int coste = 0;				// Coste a introducir.
	int x, y;					// Índices de la cuadricula.
	numElementos++;		// Se aumenta el número de elementos.

	// Se introducen las celdas objetivo.
	fronteraX[escribir] = x_end;
	fronteraY[escribir] = y_end;
	grid[x_end][y_end] = 0;		// Se asigna el coste.
	while(numElementos != 0) {		// Mientras queden elementos por leer.
		x = fronteraX[leer];		// Se lee la frontera.
		y = fronteraY[leer];

		leer++;					// Se actualiza el indice para la siguiente iteracion.
		numElementos--;			// Se resta un elemento por leer.

		coste = grid[x][y]+1;		// Se actualiza el coste.

		// Se añaden los vecinos que se deba añadir.
		if(grid[x+1][y]!=-1 && grid[x+2][y] < 0) {
			escribir++;
			numElementos++;
			fronteraX[escribir] = x+2;
			fronteraY[escribir] = y;
			grid[x+2][y] = coste;
		}

		if(grid[x-1][y]!=-1 && grid[x-2][y] < 0) {
			escribir++;
			numElementos++;
			fronteraX[escribir] = x-2;
			fronteraY[escribir] = y;
			grid[x-2][y] = coste;
		}

		if(grid[x][y+1]!=-1 && grid[x][y+2] < 0) {
			escribir++;
			numElementos++;
			fronteraX[escribir] = x;
			fronteraY[escribir] = y+2;
			grid[x][y+2] = coste;
		}

		if(grid[x][y-1]!=-1 && grid[x][y-2] < 0) {
			escribir++;
			numElementos++;
			fronteraX[escribir] = x;
			fronteraY[escribir] = y-2;
			grid[x][y-2] = coste;
		}
	}
	drawMap();
}

/*
 * Método que inicializa el algoritmo NF1 para ir dando valores.
 */
void planPath(int x_ini, int y_ini, int x_end, int y_end, int x_odo, int y_odo){

	iniciarGrid();			// Inicializa al cuadrícula.
	asignarValores();		// Coloca los obstáculos.

	// Se guarda la celda final e inicial.
	if(isPrimero){
		celdaXFin = x_end;
		celdaYFin = y_end;
		celdaOdoX = x_odo;
		celdaOdoY = y_odo;
		isPrimero = false;			// Se indica que ya no será la primera.
	}

	grid[x_end][y_end] = 0;			// Se asigna el valor 0 al objetivo.

	planPath(x_end, y_end);		// Se planifica el camino.

}

/*
 * Método que encuentra el camino desde la posición inicial.
 */
void encontrarCamino(int x_ini, int y_ini){

	// Coordenadas de inicio.
	int x = x_ini;
	int y = y_ini;

	// Coordenadas del menor valor encontrado.
	int menorX = x_ini, menorY = y_ini;

	// Se introducen las dos primeras coordenadas.
	pathX[0] = x_ini;
	pathY[0] = y_ini;

	int ind = 1;			// Índice para ir guardando el camino.
	while(grid[x][y] != 0) {
		// Comprobamos si no hay obstáculo en medio y que es menor.
		if(grid[x-1][y] != -1 && grid[x-2][y] < grid[menorX][menorY]) {
			menorX = x-2;		// Asignamos el nuevo menor.
		}
		else if(grid[x+1][y] != -1 && grid[x+2][y] < grid[menorX][menorY]) {
			menorX = x+2;		// Asignamos el nuevo menor.
		}
		else if(grid[x][y-1] != -1 && grid[x][y-2] < grid[menorX][menorY]) {
			menorY = y-2;		// Asignamos el nuevo menor.
		}
		else if(grid[x][y+1] != -1 && grid[x][y+2] < grid[menorX][menorY]) {
			menorY = y+2;		// Asignamos el nuevo menor.
		}

		// Se actualizan los menores.
		x = menorX;
		y = menorY;

		// Se meten las coordenadas del camino.
		pathX[ind] = x;
		pathY[ind] = y;

		ind++;			// Se actualiza el índice.
	}

}

/*
* Método que replanifica la trayectoria al detectar un obstáculo.
*/
void rePlanPath(int celdaX, int celdaY){

	planPath(celdaX, celdaY, celdaXFin, celdaYFin, celdaOdoX, celdaOdoY);			// Se replanifica la ruta.
	encontrarCamino(celdaX, celdaY);		// Volver a encontrar el camino.

}

/*
* Método que redondea el ángulo para dejarlo igual al eje más cercano.
*/
float redondearAng(float angulo){

	float eje = 0;

	if (angulo > -numPi/4 && angulo <= numPi/4) {
		eje = 0;		// Es el eje 0.
	} else if (angulo > numPi/4 && angulo <= 3*numPi/4) {
		eje = numPi/2;		// Es el eje PI/2.
	} else if(angulo > 3*numPi/4 || angulo <= -3*numPi/4) {
		eje = numPi;				// Es el eje PI.
	} else{
		eje = -numPi/2;			// Es el eje -PI/2.
	}

	return eje;		// Devuelve el valor del eje.

}

/*
* Método que detecta si hay un obstáculo en la trayectoria.
*/
bool detectObstacle(float theta){

	float x,y;			// Variables para la odometría.
	readOdometry(x,y,theta);		// Se lee la odometría.

	// Se obtiene la celda en la que está el obstáculo.
	int celdaX = redondearCoord(x) + celdaOdoX;
	int celdaY = redondearCoord(y) + celdaOdoY;

	// Se saca el vecino entre el que está el obstáculo
	int  xconn = celdaX, yconn = celdaY;

	float angRed = redondearAng(theta);		// Se redondea el ángulo.
	if (angRed == 0.0) {
		xconn++;
	} else if (angRed == numPi/2) {
		yconn++;
	} else if(angRed == numPi) {
		xconn--;
	} else {
		yconn--;
	}

	if(connectionsMatrix[xconn][yconn] && SensorValue[sonar] <= distancia){	// Se ha detectado el obstáculo.

		// Se marca la conexión como cerrada.
		connectionsMatrix[xconn][yconn] = false;

		rePlanPath(celdaX, celdaY);		// Se replanifica la ruta.

		return true;			// Se devuelve que habías obstáculo.

	}

	return false;		// Se devuelve que no había obstáculo desconocido.

}

/*
* Método que hace girar al robot hasta el ángulo dado.
*/
void girar(float actual, float angGiro, float w) {

	float x, y, theta;		// Variables para la odometría.
	readOdometry(x, y, theta);

	if(angGiro == numPi/2) {			// Si el giro es de PI/2.
		if(actual == -numPi/2) {		// Se mira el ángulo en el que está.
			girarHasta(angGiro,w);			// Se asigna la velocidad.
		} else if(actual == numPi) {		// Se mira el ángulo en el que está.
				girarHasta(angGiro,w);			// Se asigna la velocidad.
		} else {			// Se mira el ángulo en el que está.
			girarHasta(angGiro,w);			// Se asigna la velocidad.
		}
	}

	else {			// Si el giro es PI...
		if(actual == -numPi/2) {			// Si el eje es -PI/2...
			girarHasta(angGiro,w);			// Se asigna la velocidad.
		} else if(actual == numPi) {		// Si el eje es PI...
			if(theta < 0) {		// Se comprueba la dirección de giro.
				girarHasta(angGiro,w);			// Se asigna la velocidad.
			} else {			// Se comprueba la dirección de giro.
				girarHasta(angGiro,-w);		// Se asigna la velocidad.
			}
		} else if(actual == numPi / 2) { // Si el eje es PI/2...
				girarHasta(angGiro,-w);			// Se asigna la velocidad.
		} else {			// Si el eje es 0...
			if(theta < 0) {		// Se comprueba la dirección de giro.
				girarHasta(angGiro,-w);			// Se asigna la velocidad.
			} else {		// Se comprueba la dirección de giro.
				girarHasta(angGiro,w);			// Se asigna la velocidad.
			}
		}
	}

}

/*
 * Detecta la tira blanca para resetear la odometria.
 */
void detectarTira() {

		float x, y, th;		// Variables para la odometría.
		float recorridoX;		// Variable para lo recorrido.

		readOdometry(x,y,th);		// Se lee la odometría.
		recorridoX = x;			// Se asigna el valor de lo recorrido.

		setSpeed(100,0);			//Se avanza hasta detectar la tira.
		while(SensorValue(lightSensor) < valorColor &&
				(x <= recorridoX + sizeCell && x >= recorridoX - sizeCell)){
			readOdometry(x,y,th);
		}

		setSpeed(0,0);		// Se para la velocidad.
		bool detectada = true;
		if(x > recorridoX + sizeCell || x < recorridoX - sizeCell) {
				detectada = false;
		}

		if(detectada) {
				//Se actualiza la odometria con los datos obtenidos.
			readOdometry(x,y,th);
			float posActual = 0.0;
			if(celdaOdoX == 3){
				posActual = 600;
			} else{
				posActual = -600;
			}
			resetOdometry(posActual, y, th);

			//Se avanza media baldosa para que el robot este centrado.
			readOdometry(x, y, th);
			recorridoX = x;
			setSpeed(150,0);
			while(x <= recorridoX + sizeCell/2 && x >= recorridoX - sizeCell/2) {
				readOdometry(x, y ,th);
			}
			setSpeed(0,0);
		}
}


/*
* Método que planifica el camino desde la posición actual hasta una celda
* dada.
*/
bool go(int cellX, int cellY){

	float x, y, theta;		// Variables para la odometría.
	float angulo;			 // Ángulo diferencia y ángulo de giro.
	bool hayObstaculo = false;		// Booleano que indica si hay obstáculo.

	readOdometry(x,y,theta);			// Se lee la odometría.

	float w = numPi/2;			// Velocidad angular.
	float v = 200;					// Velocidad lineal.

	// Se saca la celda en la que estamos.
	int coordX = redondearCoord(x) + celdaOdoX;
	int coordY = redondearCoord(y) + celdaOdoY;

	theta = redondearAng(theta);			// Se redondea el ángulo al eje más cercano.

	if(cellX - coordX == 0){			// Pendiente de la recta infinito.
		if(coordY > cellY){			// Se comprueba si la celda destino es mayor o menor.
			angulo = theta - numPi/2;
		} else{
			angulo = theta + numPi/2;
		}

	} else{			// Pendiente de la recta 0.
		if(coordX > cellX){			// Se comprueba si la celda destino es mayor o menor.
			angulo = theta - 0;
		} else{
			angulo = theta - numPi;
		}
	}

	angulo = normalizarAngulo(angulo);		// Se normaliza el ángulo.

	if(angulo == numPi){		// Si es PI, se va recto.
		if(coordX != cellX && cellX == xTiraBlanca) {
				detectarTira();
		} else {
				setSpeed(v,0);			// Se asigna la velocidad lineal.
				readOdometry(x,y,theta);		// Se lee la odometría.

				// Variable para lo recorrido en cada índice.
				float recorridoX = x;
				float recorridoY = y;

				// Se comprueba si se ha llegado al objetivo.
				while(x <= recorridoX + sizeCell && y <= recorridoY + sizeCell
						&& x >= recorridoX - sizeCell && y >= recorridoY - sizeCell){
					readOdometry(x,y,theta);
				}
			}

				setSpeed(0,0);	// Se paran los motores.

				hayObstaculo = detectObstacle(theta);			// Se comprueba si hay obstáculo.

				setSpeed(0,0);
	}
	else{				// Se giran PI/2 o PI en la dirección adecuada.
		float angGiro = numPi/2;
		if(angulo == 0){			// Se comprueba si se gira PI grados o PI/2.
			angGiro = numPi;
		}
		if(angulo < 0){			// Se comprueba la dirección de giro.
			w = -w;
		}

		girar(theta, angGiro, w);			// Se indica que gire.
		readOdometry(x,y,theta);			// Se lee la odometría.
		theta = redondearAng(theta);			// Se redondea el ángulo al eje más cercano.

		hayObstaculo = detectObstacle(theta);			// Se comprueba si hay obstáculo.

		setSpeed(0,0);

		if(!hayObstaculo){			// Si no hay un obstáculo...
			hayObstaculo = go(cellX, cellY);		// Se realiza el movimiento lineal hasta la celda objetivo.
			setSpeed(0,0);
		}
	}

	return hayObstaculo;			// Se devuelve si hay obstáculo.

}

/*
* Método que recorre el camino hasta llegar al objetivo.
*/
void recorrerCamino(bool pintar){

	int ind = 1;			// Indice para recorrer el camino.
	bool seguir = true;			// Booleano que indica si hay que seguir.
	bool hayObs = false;		// Booleano para saber si hay un obstáculo.
	float x, y, theta;			//Variables para odometria.

	while(seguir){			// Mientras no se llegue al objetivo.

		hayObs = go(pathX[ind],pathY[ind]);		// Indica que comience a avanzar.

		if(hayObs){			// Si hay obstáculo, se reinicia el índice.
			ind = 0;
		}

		ind++;			// Se actualiza el índice.

		// Se comprueba si se ha llegado al destino.
		if(grid[pathX[ind-1]][pathY[ind-1]] == 0){
			seguir = false;
		}
		readOdometry(x, y, theta);
		if(pintar) {
			drawRobot(x, y, theta, celdaOdoX,celdaOdoY);
		}
	}

}
