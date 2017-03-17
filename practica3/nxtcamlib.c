//*!!CLICK to edit 'wizard' created sensor & motor configuration.                !!*//

/************************************************************************************/
// nxtcamlib.c - 'library' file for simplifying use of nxtcam.
// Gordon Wyeth
// 30 October 2007
// Updated 4/12/07 to work around intermittent Robot-C compiler bug. Use of structs
// has been removed and replaced with independent arrays. Original version should work
// reliably with next version of Robot-C according to developers.
//
//
// nxtcamlib.c provides an easy to use interface to the nxtcam. The two key functions
// are:
//
// void init_camera (tSensors camera) - Initialises the camera so it is ready to find blobs.
// Assumes that the color map has been set up using nxtcamview or similar.
//
// void get_blobs(tSensors camera, int &nblobs, int_array &color, int_array &left,
// int_array &top, int_array &right, int_array &bottom) - Fills the variables nblobs and
// bounding box coordinate arrays with current values from the camera. nblobs is loaded
// with the number of visible blobs. The arrays carry the color, leftmost pixel, topmost
// pixel, rightmost pixel, and bottommost pixel in each blob. Only the first nblobs
// in each array are valid. Old data will persist in the rest.
//
// The functions camera_cmd() and camera_flush() might also be useful, but were more
// intended to make writing the above functions easier.
//
/************************************************************************************/

#define MAX_BLOBS 8

// I2C Constants associated with camera
const int I2C_addr  = 0x02;
const int cmd_reg = 0x41;
const int count_reg = 0x42;
const int data_reg = 0x43;

// Data structure for storing blob information
typedef int int_array[MAX_BLOBS];

// void camera_flush() - flushes any pending reply bytes from camera.
void camera_flush(tSensors camera)
{
	int n;
	byte dump[8];

	while (nI2CStatus[camera] == STAT_COMM_PENDING){}
	n = nI2CBytesReady[camera];
	while (n > 0) {
		while (nI2CStatus[camera] == STAT_COMM_PENDING){}
		readI2CReply(camera, &dump[0], n);
		while (nI2CStatus[camera] == STAT_COMM_PENDING){}
		n = nI2CBytesReady[camera];
	}
}

// void camera_cmd(byte cmd) - sends cmd to the camera over I2C.
void camera_cmd(tSensors camera, byte cmd) {

	const byte msg[] =	{3, I2C_addr,	cmd_reg, cmd};

	while (nI2CStatus[camera] == STAT_COMM_PENDING){}
	sendI2CMsg(camera, &msg[0], 0);
	camera_flush(camera);
}

// void init_camera() - Initialises camera ready to find blobs.
void init_camera(tSensors camera)
{
	SensorType[camera] = sensorI2CCustomFast;
	camera_cmd(camera, 'A'); // Sort by size
	camera_cmd(camera,'E'); // Start finding
}

// void get_blobs() - loads the current blobs into the global data structure over I2C
void get_blobs(tSensors camera, int &nblobs, int_array &color, int_array &left, int_array &top, int_array &right, int_array &bottom)
{
	byte msg[3];
	byte reply[5];
	int i;

	camera_flush(camera);

	// Request number of blobs from the count register
	msg[0] = 2;
	msg[1] = I2C_addr;
	msg[2] = count_reg;
	while (nI2CStatus[camera] == STAT_COMM_PENDING){}
	sendI2CMsg(camera, &msg[0], 1);

	// Get the reply and put into nblobs global
	while (nI2CStatus[camera] == STAT_COMM_PENDING){}
	while (nI2CBytesReady[camera] != 1){}
	readI2CReply(camera, &reply[0], 1);
	nblobs = reply[0];

	camera_flush(camera);

	// Get nblobs of blob data from the camera
	for (i = 0; i < nblobs; i++) {

		// Request blob data
		msg[0] = 2;
		msg[1] = I2C_addr;
		msg[2] = data_reg + i * 5;
		while (nI2CStatus[camera] == STAT_COMM_PENDING){}
		sendI2CMsg(camera, &msg[0], 5);

		// Get blob data reply
		while (nI2CStatus[camera] == STAT_COMM_PENDING){}
		while (nI2CBytesReady[camera] != 5){}
		readI2CReply(camera, &reply[0], 5);

		// Put data into global variables.
		// Casting to unsigned int throws a warning BUT casting a byte to int is quite
		// different to casting to unsigned int, as the former performs sign extension
		// (copies first bit of byte to higher bits to make it negative) and latter
		// does not. Not sure what Robot C is doing now, but leave it for when they
		// fix it. :)
		color[i] 	= ((unsigned int)reply[0]) & 0x00FF;
		left[i] 	= ((unsigned int)reply[1]) & 0x00FF;
		top[i] 		= ((unsigned int)reply[2]) & 0x00FF;
		right[i] 	= ((unsigned int)reply[3]) & 0x00FF;
		bottom[i] = ((unsigned int)reply[4]) & 0x00FF;

		camera_flush(camera);

		}
}
