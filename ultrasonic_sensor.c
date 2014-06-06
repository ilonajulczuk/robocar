#include "ultrasonic_sensor.h"


uint16_t get_pulse_width() {
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

void toggle_yellow_diode() {
    DIODES_PORT |= (1 << YELLOW_DIODE);
}

void toggle_green_diode() {
    DIODES_PORT |= (1 << GREEN_DIODE);
}

void toggle_red_diode() {
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


int example() {
    
    int distance;
    initialize_diodes();
    initialize_us();

    while(1){
        distance = measure_distance();
        turn_off_diodes();
        if(distance == INVALID_DISTANCE) {
            toggle_red_diode();
        } else if (distance == INFINITE_DISTANCE) {
            toggle_green_diode(); 
        } else {
            if(distance > 10) {
                toggle_yellow_diode();
                _delay_ms(50);
                turn_off_diodes();
            }
            else {
                toggle_yellow_diode();
                _delay_ms(50);
            }
        }

        _delay_ms(50);
    }
}
