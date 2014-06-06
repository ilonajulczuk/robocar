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

uint16_t get_pulse_width()
{
    uint32_t i;
    uint32_t result;

    // Wait for the rising edge
    for(i = 0; i < TIMEOUT; i++)
    {
        if(!(US_PIN & (1<<US_POS))) continue; else break;
    }

    if(i == TIMEOUT)
        return 0xffff; //Indicates time out

    // High Edge Found

    // Setup Timer1
    TCCR1A = 0X00;
    TCCR1B = (1<<CS11); // Prescaler = Fcpu/8
    TCNT1 = 0x00;       // Init counter

    // Now wait for the falling edge
    for(i = 0; i < TIMEOUT; i++)
    {
        if(US_PIN & (1<<US_POS))
        {
            if(TCNT1 > 60000) break; else continue;
        }
        else
            break;
    }

    if(i == TIMEOUT)
        return 0xffff; // Indicates time out

    // Falling edge found

    result = TCNT1;

    // Stop Timer
    TCCR1B = 0x00;

    if(result > 60000)
        return 0xfffe;  // No obstacle
    else
        return (result >> 1);
}


int measure_distance() {
    uint16_t reading;
    int distance;

    //Give the US pin a 15us High Pulse
    US_PORT |= (1<<US_TRIG);  // High

    _delay_us(10);

    US_PORT &= (~(1<<US_TRIG));  // Low


    //Measure the width of pulse
    reading = get_pulse_width();

    //Handle Errors
    if(reading == US_ERROR) {
        return INVALID_DISTANCE;
    }
    else  if(reading == US_NO_OBSTACLE) {
        return INFINITE_DISTANCE;
    }
    else {
        distance = (reading / 58.0); //Convert to cm
        return distance;
    }
}

void turn_yellow_diode_on() {
    DIODES_PORT |= (1 << YELLOW_DIODE);
}

void turn_green_diode_on() {
    DIODES_PORT |= (1 << GREEN_DIODE);
}

void turn_red_diode_on() {
    DIODES_PORT |= (1 << RED_DIODE);
}

void turn_off_diodes() {
    DIODES_PORT &= ~((1 << YELLOW_DIODE) | (1 << GREEN_DIODE) | (1 << RED_DIODE));
}

void initialize_diodes() {
    DIODES_DDR = (1 << YELLOW_DIODE) | (1 << GREEN_DIODE) | (1 << RED_DIODE);
}

void initialize_us() {
    // Set Ultra Sonic Port as out
    US_DDR |= (1<<US_TRIG);

    //Now make the pin input
    US_DDR &= (~(1<<US_POS));
}

void wait() {
    int i;
    for(i = 0; i < 10; i++) {
        _delay_loop_2(0);
    }
}

int main() {
    
    int distance;
    initialize_diodes();
    initialize_us();

    while(1){
        distance = measure_distance();
        turn_off_diodes();
        if(distance == INVALID_DISTANCE) {
            turn_red_diode_on();
        } else if (distance == INFINITE_DISTANCE) {
            turn_green_diode_on(); 
        } else {
            if(distance > 10) {
                turn_yellow_diode_on();
                _delay_ms(50);
                turn_off_diodes();
            }
            else {
                turn_yellow_diode_on();
                _delay_ms(50);
            }
        }

        _delay_ms(50);
    }
}
