#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h> 
#include "ultrasonic_sensor.h"

#define MODE PD3
#define MOTOR_1_PHASE PD1
#define MOTOR_2_PHASE PD2
#define MAX_SPEED 255
#define MIN_SPEED 0

uint16_t duty_motor_1 = 165;
uint16_t duty_motor_2 = 165;
int change = 20;

int number_of_values = 7; 
uint16_t angles[] = {186, 250, 316, 340, 414, 340, 316, 250};
uint16_t indexes[] = {0, 1, 2, 3, 4, 3, 2, 1};
uint16_t distance_values[] = {0, 0, 0, 100, 0, 0, 0};

int i = 0;

void pwm_init_timer_zero()
{
    // initialize TCCR0 as per requirement, say as follows
    TCCR0A |= (1<<WGM00)|(1<<COM0A1);
    TCCR0B |= (1<<CS00);
}

void pwm_init_timer_two()
{
    // initialize TCCR2 as per requirement, say as follows
    TCCR2A |= (1<<WGM20)|(1<<COM2A1);
    TCCR2B |= (1<<CS20); 
}

void initialize_motors() {
    pwm_init_timer_zero();
    pwm_init_timer_two();
    DDRD |= (1 << MOTOR_1_PHASE) | (1 << MOTOR_2_PHASE) | (1 << MODE);
    PORTD |= (1 << MOTOR_1_PHASE) | (1 << MOTOR_2_PHASE) | (1 << MODE);
    DDRB |= 0xff;
}
     
void initialize_radar() {
    TCCR3A|=(1<<COM3A1)|(1<<COM3B1)|(1<<WGM31);
    TCCR3B|=(1<<WGM33)|(1<<WGM32)|(1<<CS31)|(1<<CS30);

    ICR3=4999;  // fPWM=50Hz (Period = 20ms Standard).

    DDRC|= (1<<PC6);  // PWM Pins as Out
}


void change_direction() {
    PORTD ^= (1 << MOTOR_1_PHASE);
    PORTD ^= (1 << MOTOR_2_PHASE);
}

void forwards() {
    PORTD |= (1 << MOTOR_1_PHASE) | (1 << MOTOR_2_PHASE);
}

void backwards() {
    PORTD &= ~((1 << MOTOR_1_PHASE) & (1 << MOTOR_2_PHASE));
}

uint16_t index_of_max(uint16_t numbers[], uint16_t N) {
    int max = -100;
    uint16_t max_index = 0;
    int i;
    for (i = 0; i < N; i++) {
        if (max < numbers[i]) {
            max = numbers[i];
            max_index = i;
        }
    }
    return max_index;
}

void radar_measure() {
    uint16_t distance;
    _delay_ms(50); 
    OCR3A = angles[i];
    _delay_ms(50);
    distance = measure_distance();
    distance_values[indexes[i]] = distance;
    i = (i + 1) % number_of_values; 
}

int main()
{
    uint16_t direction;
    uint16_t distance_forward;

    initialize_motors();
    initialize_diodes();
    initialize_us();
    initialize_radar();

    OCR0A = duty_motor_1;
    OCR2A = duty_motor_2;

    while(1){
        radar_measure();
        distance_forward = distance_values[3];
        turn_off_diodes();
        if(distance_forward == INVALID_DISTANCE) {
            toggle_red_diode();
        } else if (distance_forward == INFINITE_DISTANCE) {
            toggle_green_diode(); 
        } else {
            if(distance_forward > 10) {
                forwards();

                // chose the best direction
                direction = index_of_max(distance_values, number_of_values);
                OCR0A = duty_motor_1 + (direction - 2) * change;
                OCR2A = duty_motor_2;

                // signal that can ride freely
                toggle_yellow_diode();
                _delay_ms(50);
                turn_off_diodes();
            }
            else {
                toggle_yellow_diode();
                
                // stop
                OCR0A = 0;
                OCR2A = 0;
                
                // change direction to ride backwards
                _delay_ms(100);
                change_direction();
                _delay_ms(100);

                // choose direction with the most free space to ride in
                direction = index_of_max(distance_values, number_of_values);

                // back up
                OCR0A = duty_motor_1;
                OCR2A = duty_motor_2 + (direction -2 ) * 40;

                _delay_ms(900);

                OCR0A = 0;
                OCR2A = 0;
                
                // scan with radar to make new map 
                int j;
                for(j = 0; j < number_of_values; j++) {
                    radar_measure();
                }
            }
        }

        _delay_ms(50);
    }
}
