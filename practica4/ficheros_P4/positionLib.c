// Variables globales para la odometría.
#define INI_X 0
#define INI_Y 0
#define INI_TH PI/2

float numPi = 3.14159265;     // Valor de numPi.

/*
 * Estructura con la posición.
 */
typedef struct {
     float x;   //x coordinate
     float y;   //y coordinate
     float th;  //theta - orientation RADIANES
}TPosition;

/*
 * Método que fija la posición.
 */
void set_position(TPosition &pos, float x, float y, float th)
{
  pos.x=x;
  pos.y=y;
  pos.th=th;

}

/*
 * Normaliza un angulo entre -pi y pi.
 */
float normalizarAngulo(float angulo) {

	while(angulo <= -numPi){
		angulo = angulo + 2*numPi;
	}
	while(angulo > numPi){
		angulo = angulo - 2*numPi;
	}

	return angulo;

}
