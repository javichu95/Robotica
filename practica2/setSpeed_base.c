//#pragma config(Sensor,  .... )
//                  PLANTILLA P2
//  Make the robot move at a certain speed (linear and angular), so it can drive along an 8
//  Update the robot internal odometry estimation, so it aproximately "knows" where it is all the time
//

#include "mutexLib.c"
#include "positionLib.c"

// ROBOT PARAMETERS
float R = 26.0; // mm
float L = 87.0; // mm

TPosition robot_odometry;       // WE SHOULD ACCESS THIS VARIABLE with a "semaphore".
TMutex semaphore_odometry = 0;  // Important to initialize to zero!!! Not acquired.


// FUNCTION!!
int setSpeed(float v, float w)
{

// start the motors so that the robot gets v mm/s linear speed and w RADIAN/s angular speed

  float w_l =1, w_r=1;
  float mR = 1,mL = 1,nR = 0,nL = 0; // parameters of power/speed transfer
  float motorPowerRight = 15, motorPowerLeft = 15;

  // Calculamos la velocidad angular de cada rueda.
  w_r = v/R + w*L/(2.0*R);
  w_l = v/R - w*L/(2.0*R);

  // Se calcula la potencia del motor.
  motorPowerLeft = mL*w_l + nL;
  motorPowerRight = mR*w_r + nR;

	// Se asigna la potencia al motor.
	motor[motorB] = motorPowerRight;
	motor[motorC] = motorPowerLeft;
	motor[motorA] = 10;

	wait1Msec(1000);

  return 0;
}

// TASK TO BE LAUNCHED SIMULTANEOUSLY to "main"!!
/*task updateOdometry(){
  int cycle = ??? ; // we want to update odometry every ?? s
  float dSl,dSr,dx,dy, dT;

  while (true){

    timeAux=nPgmTime;
	// read tachometers, and estimate how many mm. each wheel has moved since last update
    // RESET tachometer right after to start including the "moved" degrees turned in next iteration



   	// show each step on screen and write in a file
		nxtDisplayTextLine(2, "ODOMETRY NEW VALUE");
    	nxtDisplayTextLine(3, "x,y: %2.2f %2.2f", robot_odometry.x,robot_odometry.y);
    	nxtDisplayTextLine(4, "theta: %2.2f ", robot_odometry.th);
	  	// file ...





	 // Wait until cycle is completed?
	 // ...

  }

}*/


//the program below uses feedback from encoders to determine how much the robot turns.
task main()
{

  float v = 0, w = 3.14159; //speeds
  int radio; //Trajectory R
  int circunf; // L (semi-dist. between robot wheels)

  // config.


  // reset odometry values and motor encoders.

  // Encoders a 0.
	nMotorEncoder[motorB] = 0;
	nMotorEncoder[motorC] = 0;

  //StartTask(updateOdometry);

  // Se establecen la velocidad lineal y angular.
	setSpeed(v,w);

  // turn 90 degrees on the robot


  // generate 1st part of trayectory



  // generate 2nd part of trayectory



  // generate 3rd part of trayectory



  // generate 4th part of trayectory


}
