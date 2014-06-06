#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <avr/io.h>
#include <util/delay.h>

#define US_PORT PORTD
#define US_PIN  PIND
#define US_DDR  DDRD

#define US_POS PD0      //PORTD0
#define US_TRIG PD7

/********************************************************************

  This function measurers the width of high pulse in micro second.

 ********************************************************************/

#define US_ERROR 0xffff
#define US_NO_OBSTACLE 0xfffe

#define INVALID_DISTANCE -1
#define INFINITE_DISTANCE 0xffff


/*******************************************************************
  DIODES SETUP

 *******************************************************************/

#define DIODES_PORT PORTD
#define DIODES_DDR DDRD
#define RED_DIODE PD6
#define GREEN_DIODE PD5
#define YELLOW_DIODE PD4

#define TIMEOUT 600000



uint16_t get_pulse_width();
int measure_distance();
void toggle_red_diode();
void toggle_green_diode();
void toggle_yellow_diode();
void turn_off_diodes();

void initialize_diodes();
void initialize_us();

#endif
