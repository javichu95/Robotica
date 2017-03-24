//*!!Sensor,    S1,                  cam, sensorI2CCustomFast,      ,            !!*//
//*!!                                                                            !!*//
//*!!Start automatically generated configuration code.                           !!*//
const tSensors cam                  = (tSensors) S1;   //sensorI2CCustomFast //*!!!!*//
//*!!CLICK to edit 'wizard' created sensor & motor configuration.                !!*//

#include "G:\robotica\3rd Party Driver Library\include\nxtcamlib.c"

/************************************************************************************/
// cam_display.c - debugging tool for nxtcam using Robot C. Needs nxtcamlib.c.
// Gordon Wyeth
// 30 October 2007
// Updated 4/12/07 to use new version of nxtcamlib.c that works around Robot-C compiler
// bug.
//
// Displays the blobs returned from the camera on the NXT display as text. Left and right
// buttons can be used to choose which blob you would like to look at.
//
/************************************************************************************/

// Global
int cb; // Current blob index to display

// task button_handler() - increments blob index when right button is pressed, decrements
// when left button is pressed. Keeps values between 0 and 7.

task button_handler()
{
	while (true) {
		while (nNxtButtonPressed == -1){}
		if (nNxtButtonPressed == 2) {
			if (cb == 0) {
				cb = 7;
			} else {
				cb--;
			}
		} else if (nNxtButtonPressed == 1) {
			if (cb == 7) {
				cb = 0;
			} else {
				cb++;
			}
		}
		while (nNxtButtonPressed != -1){}
	}
}

task main ()
{
	int nblobs;
	int_array bc;
	int_array bl;
	int_array bt;
	int_array br;
	int_array bb;

	// Initialise the camera
	init_camera(cam);

	// Start with blob 0
	cb = 0;

	// Setup button handler
	nNxtButtonTask = -2;
	StartTask(button_handler);

	while (true) {

		// Get the current blob data from the camera
		get_blobs(cam, nblobs, bc, bl, bt, br, bb);
		// Print the data on the screen
		nxtDisplayTextLine(1, "Blob %d of %d", cb + 1, nblobs);
		nxtDisplayTextLine(2, "Color: %d", bc[cb]);
		nxtDisplayTextLine(3, "Left: %d", bl[cb]);
		nxtDisplayTextLine(4, "Top: %d", bt[cb]);
		nxtDisplayTextLine(5, "Right: %d", br[cb]);
		nxtDisplayTextLine(6, "Bottom: %d", bb[cb]);
	}
}
