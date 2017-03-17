//*!!Sensor,    S1,                  cam, sensorI2CCustomStd,      ,             !!*//
//*!!                                                                            !!*//
//*!!Start automatically generated configuration code.                           !!*//
const tSensors cam                  = (tSensors) S1;   //sensorI2CCustomStd //*!!!!*//
//*!!CLICK to edit 'wizard' created sensor & motor configuration.                !!*//

#include "nxtcamlib.c"

/************************************************************************************/
// cam_display.c - debugging tool for nxtcam using Robot C. Needs nxtcamlib.c.
// Gordon Wyeth
// 30 October 2007
// Updated 4/12/07 to use new version of nxtcamlib.c that works around Robot-C compiler
// bug.
//
// Displays the blobs returned from the camera on the NXT display. Note the scaling
// functions below reflect the actual scaling required - not the scaling from the
// documented values of camera coordinates.
//
/************************************************************************************/

// int xscale(int x) - Scales x values from camera coordinates to screen coordinates.
int xscale(int x) {
	return ((x - 12) * 99) / 176;
}

// int yscale(int y) - Scales y values from camera coordinates to screen coordinates.
int yscale(int y) {
	return ((143 - y) * 63) / 143;
}

task main ()
{
	int n; // Number of blobs
	int i;
	int_array bc, bl, bt, br, bb; // Intermediate values for scaled corners
	int l, t, r, b; // Intermediate values for scaled corners

	// Initialises the camera
	init_camera(cam);

	while (true) {

		// Get the blobs into the arrays for display
		get_blobs(cam, n, bc, bl, bt, br, bb);

		// Clear the display
		eraseDisplay();
		for (i = 0; i < n; i++) {

			// Draw the scaled blobs
			l = xscale(bl[i]);
			t = yscale(bt[i]);
			r = xscale(br[i]);
			b = yscale(bb[i]);
			nxtFillRect(l, t, r, b);
		}
	}
}
