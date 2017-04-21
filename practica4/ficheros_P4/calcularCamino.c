#pragma config(Sensor, S1, 	sonar,          sensorSONAR)

#include "mapLib.c"

task main()
{
	//while(true) {
	/*int distance_in_cm = 31;   // Create variable 'distance_in_cm' and initialize it to 20(cm).

   while(SensorValue[sonar] > distance_in_cm)   // While the Sonar Sensor readings are less than the specified, 'distance_in_cm':
   {
      setSpeed(150,0);
   }
   setSpeed(0,numPi/2);
   wait1Msec(1000);
   motor[motorB] = 0;    // Motor B is stopped at a 0 power level
   motor[motorC] = 0;    // Motor C is stopped at a 0 power level
		nxtDisplayTextLine(1, "%d", SensorValue[sonar]);
	}*/

	int x,y;
	float th;
	string map_file = "mapa2.txt";

	// Se inicializa la odometría.
	set_position(robot_odometry, INI_X, INI_Y, INI_TH);
	initConnections();

	loadMap(map_file);

	planPath(1,1,5,5);

	encontrarCamino(1,1);

	startTask(updateOdometry);
	recorrerCamino();
	stopTask(updateOdometry);
}
