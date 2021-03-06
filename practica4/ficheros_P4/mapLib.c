
#include "mapLib.h"

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
* M�todo que dada una celda y su vecino da con la coordenada del punto
* en la matriz de conexiones.
*/
void cell2connCoord(int cellX, int cellY, int numNeigh, int & connX, int & connY){

  connX=2*cellX+1;
  connY=2*cellY+1;
  switch(numNeigh){
    case 0: connY++; break;
    case 1: connY++; connX++; break;
    case 2: connX++;break;
    case 3: connY--; connX++; break;
    case 4: connY--; break;
    case 5: connY--; connX--; break;
    case 6: connX--; break;
    case 7: connY++; connX--; break;
  }

}

/*
 * Marca una cierta celda como conectada a otra.
 */
void setConnection(int cellX, int cellY, int numNeigh){

  int connX, connY; // Coordenadas en la matriz de conexiones.

  // Pasa de coordenadas de la celda a coordenadas de la matriz de conexiones.
  cell2connCoord(cellX, cellY, numNeigh, connX, connY);

  // Marca la celda como conectada.
  connectionsMatrix[connX][connY]=true;

}

/*
 * M�todo que borra la conexi�n entre dos celdas.
 */
void deleteConnection(int cellX, int cellY, int numNeigh){

  int connX, connY; // Coordenadas en la matriz de conexiones.

  // Pasa de coordenadas de la celda a coordenadas de la matriz de conexiones.
  cell2connCoord(cellX, cellY, numNeigh, connX, connY);

  // Marca la celda como no conectada.
  connectionsMatrix[connX][connY]=false;

}

/*
 * M�todo que devuelve true si y s�lo si dos celdas dadas est�n conectadas.
 */
bool isConnected(int cellX, int cellY, int numNeigh){

   int connX, connY; // Coordenadas en la matriz de conexiones.

 	// Pasa de coordenadas de la celda a coordenadas de la matriz de conexiones.
  cell2connCoord(cellX, cellY, numNeigh, connX, connY);

  // Devuelve si la celda est� conectada con la otra o no.
  return(connectionsMatrix[connX][connY]);

}

/*
* M�todo que lee el mapa del fichero.
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
              nxtDisplayTextLine(1, "PROBLEM READING map");
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

     /*nxtDisplayTextLine(3, "%d %d ", dimX, dimY);
     nxtDisplayTextLine(4, "%d ", dimCell);
     wait10Msec(300);*/

     return endfile;

}

/*
* M�todo que lee la siguiente l�nea del fichero.
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
                  nxtDisplayTextLine(3, " %d %d", mapCol,mapRow);
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
              nxtDisplayTextLine(1, "PROBLEM READING map");
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
     StringFromChars(linestring,linechar);
     /*if (numbersRead[indNum]!=num && num!=0){
        numbersRead[indNum]=num;
     }*/

     nxtDisplayTextLine(3, "%s ", linestring);

     /*for(int j=2; j<=indNum; ++j){
        setConnection(numbersRead[0], numbersRead[1], numbersRead[j]);
        nxtDisplayTextLine(4, "%d connection open", numbersRead[j]);
        //wait10Msec(200);
     }*/
     return endfile;

	}

/*
* M�todo que carga el mapa desde un fichero.
*/
bool loadMap(string mapFileName){

     bool loadingOk=false;
     int dimConectionX,dimConectionY;
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
           nxtDisplayTextLine(1, "PROBLEM OPENING file");
     }

	   return loadingOk;

}

/*
 * M�todo que dibuja el mapa por pantalla.
 */
void drawMap(){

  int i,j,cx,cy;

  eraseDisplay(); // L_B: (0,0); T_T: (99,63)
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
 * M�todo que convierte las coordenadas del robot en odometr�a en coordenadas
 * de la celda.
 */
void pos2cell(float x_mm, float y_mm, int & x_cell, int & y_cell){

  x_cell =  (int) x_mm/sizeCell;

  y_cell = (int) y_mm/sizeCell;

}

/*
 * M�todo que dibuja el robot en la pantalla.
 */
void drawRobot(float x_mm, float y_mm, float ang_rad){

  int cellx,celly;
  int pixX,pixY;
  float ang_grad;
  int th;

  pos2cell(x_mm, y_mm, cellx,celly);

  pixX=cellx*pixPerX+pixPerX/2;
  pixY=celly*pixPerY+pixPerY/2;
  nxtFillEllipse(pixX-1, pixY+1, pixX+1, pixY-1); //nxtFillEllipse(Left, Top, Right, Bottom);

  //normalizeAngle(ang_rad);
  ang_grad=radiansToDegrees(ang_rad);
  if (ang_grad<0){ ang_grad=ang_grad+360;}
  th=(ang_grad+22.5)/45;
  while(th>7){th=th-8;}

	// Pinta la orientaci�n.
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
 * M�todo que va asignando costes recursivamente a la cuadr�cula.
 */
void planPath(int coordenadaX, int coordenadaY, int coste){

  // Comprueba si es una celda v�lida y si el coste que ya hay es mayor.
  if(grid[coordenadaX+1][coordenadaY] != -1 && coordenadaX+2 <= 2*sizeX) {
		if(grid[coordenadaX+2][coordenadaY]	< -1 || grid[coordenadaX+2][coordenadaY] > coste) {
      // Asigna el coste y realiza la llamada recursiva.
			grid[coordenadaX+2][coordenadaY] = coste;
			planPath(coordenadaX+2,coordenadaY, coste+1);
		}
	}

  // Comprueba si es una celda v�lida y si el coste que ya hay es mayor.
	if(grid[coordenadaX-1][coordenadaY] != -1 && coordenadaX-2 >= 0) {
		if(grid[coordenadaX-2][coordenadaY]	< -1 || grid[coordenadaX-2][coordenadaY] > coste) {
      // Asigna el coste y realiza la llamada recursiva.
			grid[coordenadaX-2][coordenadaY] = coste;
			planPath(coordenadaX-2,coordenadaY, coste+1);
		}
	}

  // Comprueba si es una celda v�lida y si el coste que ya hay es mayor.
	if(grid[coordenadaX][coordenadaY+1] != -1 && coordenadaY+2 <= 2*sizeY) {
		if(grid[coordenadaX][coordenadaY+2]	< -1 || grid[coordenadaX][coordenadaY+2] > coste) {
      // Asigna el coste y realiza la llamada recursiva.
			grid[coordenadaX][coordenadaY+2] = coste;
			planPath(coordenadaX,coordenadaY+2, coste+1);
		}
	}

  // Comprueba si es una celda v�lida y si el coste que ya hay es mayor.
	if(grid[coordenadaX][coordenadaY-1] != -1 && coordenadaY-2 >= 0) {
		if(grid[coordenadaX][coordenadaY-2]	< -1 || grid[coordenadaX][coordenadaY-2] > coste) {
      // Asigna el coste y realiza la llamada recursiva.
			grid[coordenadaX][coordenadaY-2] = coste;
			planPath(coordenadaX,coordenadaY-2, coste+1);
		}
	}

}

/*
 * M�todo que asigna el valor -1 a obst�culos.
 */
void asignarValores(){

	// Se recorre la matriz para poner los obst�culos.
  for(int i = 0; i <= 2*sizeX; i++){
		for(int j = 0; j <= 2*sizeY; j++){
			if(!connectionsMatrix[i][j]) {
				grid[i][j] = -1;
			}
		}
	}

}

/*
* M�todo que inicializa la cuadr�cula con valor -2.
*/
void iniciarGrid(){

	// Recorre la cuadr�cula inicializandola.
	for(int i = 0; i <= 2*sizeX; i++){
		for(int j = 0; j <= 2*sizeY; j++){
			grid[i][j] = -2;
		}
	}

}

/*
 * M�todo que inicializa el algoritmo NF1 para ir dando valores.
 */
void planPath(int x_ini, int y_ini, int x_end, int y_end){

	iniciarGrid();			// Inicializa al cuadr�cula.
	asignarValores();		// Coloca los obst�culos.

	// Se guarda la celda final e inicial.
	if(isPrimero){
		celdaXFin = x_end;
		celdaYFin = y_end;
		celdaXIni = x_ini;
		celdaYIni = y_ini;
		isPrimero = false;			// Se indica que ya no ser� la primera.
	}

	grid[x_end][y_end] = 0;			// Se asigna el valor 0 al objetivo.

	planPath(x_end, y_end, 1);		// Se planifica el camino.

}

/*
 * M�todo que encuentra el camino desde la posici�n inicial.
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

	int ind = 1;			// �ndice para ir guardando el camino.
	while(grid[x][y] != 0) {
		// Comprobamos si no hay obst�culo en medio y que es menor.
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
		grid[x][y] = 99-ind;		// Indicamos el orden del camino.
		// Se actualizan los menores.
		x = menorX;
		y = menorY;
		// Se meten las coordenadas del camino.
		pathX[ind] = x;
		pathY[ind] = y;
		ind++;			// Se actualiza el �ndice.
	}

}

/*
* M�todo que replanifica la trayectoria al detectar un obst�culo.
*/
void rePlanPath(int celdaX, int celdaY){

	planPath(celdaX, celdaY, celdaXFin, celdaYFin);			// Se replanifica la ruta.
	encontrarCamino(celdaX, celdaY);		// Volver a encontrar el camino.

	// Se escribe la nueva ruta en el fichero.
/*	OpenWrite(hFileHandleCuad, nIoCuadricula, cuadricula, nFileSizeCuad);

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

}

/*
* M�todo que redondea las coordenadas por si el robot se ha desviado.
*/
int redondearCoord(float coord){

	float div = coord / sizeCell;		// Se obtiene la celda en la que est�.
	float resto = div - (int)(div);	// Se saca el resto de la divisi�n.
	int signo = 1, resultado = 0;		// Variables para el signo y resultado.

	if(div < 0){		// Se comprueba si es negativo.
		signo = -1;		// Se saca el signo.
		div = -div;
	}

	if(resto < 0){	// Se comprueba si es negativo.
		resto = -resto;
	}

	if(resto >= 0.75){		// Si el resto es mayor que 0.75...
		resultado = ((((int)(div))+1)*2)*signo;		// Se asigna a la siguiente celda.
	} else{			// Si no es mayor que 0.75...
		resultado = ((int)(div));			// Se asigna la celda actual.
		resultado = resultado*signo*2;
	}

	return resultado;			// Se devuelve el resultado.

}

/*
* M�todo que redondea el �ngulo para dejarlo igual al eje m�s cercano.
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

	return eje;

}

/*
* M�todo que detecta si hay un obst�culo en la trayectoria.
*/
bool detectObstacle(float theta){

	int distancia = 31;		// Distancia al obst�culo.

	if(SensorValue[sonar] <= distancia){	// Se ha detectado el obst�culo.

		float x,y, theta;			// Variables para la odometr�a.
		readOdometry(x,y,theta);		// Se lee la odometr�a.

		// Se obtiene la celda en la que est� el obst�culo.
		int celdaX = redondearCoord(x) + celdaXIni;
		int celdaY = redondearCoord(y) + celdaYIni;

		// Se saca el vecino entre el que est� el obst�culo
		int  xconn = celdaX, yconn = celdaY;

		float angRed = redondearAng(theta);		// Se redondea el �ngulo.
		if (angRed == 0.0) {
			xconn++;
		} else if (angRed == numPi/2) {
			yconn++;
		} else if(angRed == numPi) {
			xconn--;
		} else {
			yconn--;
		}

		// Se marca la conexi�n como cerrada.
		connectionsMatrix[xconn][yconn] = false;
		rePlanPath(celdaX, celdaY);		// Se replanifica la ruta.

		return true;

	}

	return false;
}

void girar(float actual, float angGiro, float w) {
	float x, y, theta;

	if(angGiro == numPi/2) {			// Si el giro es de pi/2.
		if(actual == -numPi/2) {
			setSpeed(0,w);			// Se asigna la velocidad.
			readOdometry(x,y,theta);
			while(theta < 0) {
					readOdometry(x,y,theta);
			}
		} else if(actual == numPi) {
				setSpeed(0,w);			// Se asigna la velocidad.
				readOdometry(x,y,theta);
				while(abs(theta) > numPi/2) {
					readOdometry(x,y,theta);
			}
		} else {
			setSpeed(0,w);			// Se asigna la velocidad.
			readOdometry(x,y,theta);
			while(theta > actual-numPi/2 && theta < actual + numPi/2) {
				readOdometry(x,y,theta);
			}
		}
	} else {			// Si el giro es pi...
		if(actual == -numPi/2) {			// Si el eje es -pi/2...
			setSpeed(0,w);			// Se asigna la velocidad.
			readOdometry(x,y,theta);
			while(theta < numPi / 2) {
				readOdometry(x,y,theta);
			}
		} else if(actual == numPi) {		// Si el eje es pi...
			if(theta < 0) {
				setSpeed(0,w);			// Se asigna la velocidad.
				readOdometry(x,y,theta);
				while(theta < 0) {
					readOdometry(x,y,theta);
				}
			} else {
				setSpeed(0,-w);			// Se asigna la velocidad.
				readOdometry(x,y,theta);
				while(theta > 0) {
					readOdometry(x,y,theta);
				}
			}
		} else if(actual == numPi / 2) { // Si el eje es pi/2...
				setSpeed(0,-w);			// Se asigna la velocidad.
				readOdometry(x,y,theta);
				while(theta > -numPi/2) {
					readOdometry(x,y,theta);
				}
		} else {			// Si el eje es 0...
			if(theta < 0) {
				setSpeed(0,-w);			// Se asigna la velocidad.
				readOdometry(x,y,theta);
				while(theta < 0) {
					readOdometry(x,y,theta);
				}
			} else {
				setSpeed(0,w);			// Se asigna la velocidad.
				readOdometry(x,y,theta);
				while(theta > 0) {
					readOdometry(x,y,theta);
				}
			}
		}
	}
	setSpeed(0,0);		// Se para la velocidad.

}

/*
* M�todo que planifica el camino desde la posici�n actual hasta una celda
* dada.
*/
bool go(int cellX, int cellY){

	float x, y, theta;		// Variables para la odometr�a.
	float angulo;			 // �ngulo diferencia y �ngulo de giro.
	readOdometry(x,y,theta);			// Se lee la odometr�a.
	float w = numPi/2;			// Velocidad angular.
	float v = 150;					// Velocidad lineal.
	bool hayObstaculo = false;		// Booleano que indica si hay obst�culo.

	// Se saca la celda en la que estamos.
	int coordX = redondearCoord(x) + celdaXIni;
	int coordY = redondearCoord(y) + celdaYIni;

	theta = redondearAng(theta);			// Se redondea el �ngulo al eje m�s cercano.

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

	angulo = normalizarAngulo(angulo);

	if(angulo == numPi){		// Si es PI, se va recto.
		setSpeed(v,0);			// Se asigna la velocidad lineal.
		readOdometry(x,y,theta);		// Se lee la odometr�a.

		// Variable para lo recorrido en cada �ndice.
		float recorridoX = abs(x);
		float recorridoY = abs(y);

		/*float restoX = recorridoX / sizeCell;
		float restoY = recorridoY / sizeCell;

		restoX = restoX - (int)(restoX);
		restoY = restoY - (int)(restoY);

		recorridoX = recorridoX-(restoX*sizeCell);
		recorridoY = recorridoY-(restoY*sizeCell);*/

		// Se comprueba si se ha llegado al objetivo.
		while(abs(x) <= recorridoX + sizeCell && abs(y) <= recorridoY + sizeCell
				&& abs(x) >= recorridoX - sizeCell && abs(y) >= recorridoY - sizeCell){
			readOdometry(x,y,theta);
		}
		setSpeed(0,0);

		hayObstaculo = detectObstacle(theta);			// Se comprueba si hay obst�culo.

	}
	else{				// Se giran PI/2 o PI en la direcci�n adecuada.
		float angGiro = numPi/2;
		if(angulo == 0){			// Se comprueba si se gira PI grados o PI/2.
			angGiro = numPi;
		}
		if(angulo < 0){
			w = -w;
		}

		/*if(angGiro == numPi/2) {
			setSpeed(0,w);
			wait1Msec(1000);
		} else {
			setSpeed(0,w);
			wait1Msec(2000);
		}*/
		girar(theta, angGiro, w);			// Se indica que gire.
		readOdometry(x,y,theta);			// Se lee la odometr�a.
		theta = redondearAng(theta);			// Se redondea el �ngulo al eje m�s cercano.

		hayObstaculo = detectObstacle(theta);			// Se comprueba si hay obst�culo.
		if(!hayObstaculo){			// Se detecta si hay un obst�culo.
			hayObstaculo = go(cellX, cellY);		// Se realiza el movimiento lineal.
		}

	}
	drawMap();
	return hayObstaculo;			// Se devuelve si hay obst�culo.

}

/*
* M�todo que recorre el camino hasta llegar al objetivo.
*/
void recorrerCamino(){

	int ind = 1;			// Indice para recorrer el camino.
	bool seguir = true;			// Booleano que indica si hay que seguir.
	bool hayObs = false;		// Booleano para saber si hay un obst�culo.
	while(seguir){			// Mientras no se llegue al objetivo.

		hayObs = go(pathX[ind],pathY[ind]);		// Indica que comience a avanzar.

		if(hayObs){			// Si hay obst�culo, se reinicia el �ndice.
			ind = 0;
		}

		ind++;			// Se actualiza el �ndice.

		if(grid[pathX[ind-1]][pathY[ind-1]] == 0){
			seguir = false;
		}

	}

}
