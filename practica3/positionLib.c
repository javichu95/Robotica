// GLOBAL VARIABLE FOR ODOMETRY
#define INI_X 0
#define INI_Y 0
#define INI_TH PI/2

typedef struct {
     float x;   //x coordinate
     float y;   //y coordinate
     float th;  //theta - orientation
}TPosition;


void set_position(TPosition &pos, float x, float y, float th)
{
  pos.x=x;
  pos.y=y;
  pos.th=th;

}
