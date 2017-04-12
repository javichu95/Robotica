#include "mapLib.c"

string sFileName = "grid.txt";     // Nombre del fichero.
TFileIOResult nIoResult;
TFileHandle hFileHandle = 0;
short nFileSize = 20000;			// Tama??el fichero.

task main()
{
	int x,y;
	float th;
	string map_file = "mapa2.txt";
	initConnections();

	if(	loadMap(map_file) ){
	  nxtDisplayTextLine(6, "Mapa loaded ok");
	}else{
	  nxtDisplayTextLine(6, "Mapa NOT loaded");
	}


	iniciarGrid();
	asignarValores();

	planPath(0,0,0,0);

	OpenWrite(hFileHandle, nIoResult, sFileName, nFileSize);

	string sString;
	for(int i = 2*sizeY; i >= 0; i--) {
		for(int j = 0; j <= 2*sizeX; j++) {
			stringFormat(sString, "%d ", grid[j][i]);
    	WriteText(hFileHandle, nIoResult, sString);
		}
    WriteText(hFileHandle, nIoResult, "\n");
	}

}
