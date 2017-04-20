#include "mapLib.c"

string cuadricula = "grid.txt";     // Nombre del fichero.
TFileIOResult nIoCuadricula;
TFileHandle hFileHandleCuad = 0;
short nFileSizeCuad = 20000;			// Tama??el fichero.

task main()
{
	int x,y;
	float th;
	string map_file = "mapa2.txt";
	initConnections();

	loadMap(map_file);

	iniciarGrid();
	asignarValores();

	planPath(1,1,5,3);

	encontrarCamino(1,1);

	OpenWrite(hFileHandleCuad, nIoCuadricula, cuadricula, nFileSizeCuad);

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
	startTask(updateOdometry);
	recorrerCamino();
	stopTask(updateOdometry);
}
