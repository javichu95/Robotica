////////////////////////////////////////////////////////////////
// Example to load "mapaA.txt"
////////////////////////////////////////////////////////////////

#include "mapLib.c"


task main(){

int x,y;
float th;
string map_file = "mapa2.txt";
initConnections();

if(	loadMap(map_file) ){
  nxtDisplayTextLine(6, "Mapa loaded ok");
}else{
  nxtDisplayTextLine(6, "Mapa NOT loaded");
}


drawMap();
th=-pi/4 + (20*PI)/180;
for (x=50; x<sizeX*sizeCell; x=x+200){
  th=th+PI/4;
  for (y=50; y<sizeY*sizeCell; y=y+200){
        drawRobot(x,y,th);
        wait1Msec(100);
  }
}

// sample commands to draw the robot at some sample locations
/*drawRobot(0,0,-90);
drawRobot(0,0,-88);
drawRobot(2,2,-170);


drawRobot(200,200,180);
drawRobot(300,300,90);
drawRobot(600,600,359);*/


wait1Msec(10000);


/* FIND THE WAY OUT from INI position to END position ... */

/*

void planPath( x_ini,  y_ini, x_end, y_end);

while (notfinished){

  // go(pathX[i],pathY[i]);

  // check if there are close obstacles


  // deal with them...
  // Avoid_obstacle(...) OR RePlanPath(...)

}

*/


}
