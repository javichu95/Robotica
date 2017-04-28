#ifndef CONTROLVELOCIDAD_H
#define CONTROLVELOCIDAD_H

#pragma config(Sensor, S2,     cam,                 sensorI2CCustomFastSkipStates)
#pragma config(Motor,  motorB,          r_motor,       tmotorNXT, PIDControl)
#pragma config(Motor,  motorC,          l_motor,       tmotorNXT, PIDControl)

// Librer�as para la c�mara.
#include "G:\robotica\3rd Party Driver Library\include\common.h"
#include "G:\robotica\3rd Party Driver Library\include\nxtcamlib.c"

// Colores a detectar.
#define BLUE 1
#define RED 0

// Color a detectar y �rea del blop.
#define GOAL_COLOR RED
#define AREA_COLOR 2200

/*
 * M�todo que centra la pelota en la c�mara.
 */
float centroPelota(int_array bl, int_array br, int_array bt, int_array bb, int i);

/*
 * Mueve el robot para buscar la pelota.
 */
void buscar(bool derecha);

/*
 * Avanza hacia la pelota y si la tiene a la distancia adecuada la coge.
 */
bool atraparPelota(float area, float w);

/*
 * M�todo principal que centra al robot y coge la pelota.
 */
void buscarAtrapar();

/*
 * M�todo que abre la pinza del robot.
 */
void abrirPinza();

/*
 * M�todo que cierra la pinza del robot.
 */
void cerrarPinza();

 /*
  * M�todo que deja de ejercer presion con la pinza.
  */
void aflojarPinza();


#endif
