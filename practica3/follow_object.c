#pragma config(Sensor, S2,     cam,                 sensorI2CCustomFastSkipStates)

// CONFIG camera color position
#define BLUE 2
#define RED 1

// CONFIG GOAL PARAMETERS
#define GOAL_COLOR RED
#define AREA_COLOR 100


// SUSTITUIR POR LAS LIBRERIAS QUE USEIS para acceder a la cámara.
// Distintas versiones tienen nombres ligeramente distintos
#include "common.h"
#include "nxtcamlib.c"


/************************************************************************************/
// follow_object.c
// Track a blob of the selected color until the robot is "close enough"
/************************************************************************************/

task main ()
{
	bool continueTracking = true;
  	int _nblobs;
  	int_array bc, bl, bt, br, bb;
  	blob_array _blobs;
  	bool _condensed = true;

 	// Initialise the camera
	init_camera(cam);

	while (continueTracking) {

		// Get the blobs from the camera into the array
		 get_blops(cam, _nblops, bc, bl, bt, br, bb);
		 nxtDisplayTextLine(1, "%d", _nblobs);

		 if(_nblobs == 0){		// Si no encuentra la pelota, da vueltas.
		 	set_speed(0,1.4);		// Se fija la velocidad angular para dar vueltas.
		 } else{
		 	for (int i = 0; i < _nblobs; i++) {
     			if _blobs[i].color == GOAL_COLOR){		// Si el color coincide...
					// Se obtiene el centro y el error.


					// Ajustar lineal y angular para llegar.
				}
     		}	
		 }
	}

	// Límite en encoder.

	// Aplicar velocidad hasta cerrar la garra (== encoder).


}
