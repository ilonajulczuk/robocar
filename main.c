#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h> 
#include "ultrasonic_sensor.h"

#define MODE PD3
#define MOTOR_1_PHASE PD1
#define MOTOR_2_PHASE PD2
#define MAX_SPEED 255
#define MIN_SPEED 0

volatile uint16_t duty_motor_1, duty_motor_2;
volatile uint16_t tot_overflow;


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

void motors_init() {
    pwm_init_timer_zero();
    pwm_init_timer_two();
    DDRD |= (1 << MOTOR_1_PHASE) | (1 << MOTOR_2_PHASE) | (1 << MODE);
    PORTD |= (1 << MOTOR_1_PHASE) | (1 << MOTOR_2_PHASE) | (1 << MODE);
    DDRB |= 0xff;
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

void change_spped_of_first_motor(uint8_t change) {
    if(duty_motor_1 + change >= MAX_SPEED) {
        duty_motor_1 = MAX_SPEED;
    } else if(duty_motor_1 + change <= MIN_SPEED) {
        duty_motor_1 = MIN_SPEED;
    } else {
        duty_motor_1 += change;
    }
}

void change_spped_of_second_motor(uint8_t change) {
    if(duty_motor_2 + change >= MAX_SPEED) {
        duty_motor_2 = MAX_SPEED;
    } else if(duty_motor_2 + change <= MIN_SPEED) {
        duty_motor_2 = MIN_SPEED;
    } else {
        duty_motor_2 += change;
    }
}

//Simple Wait Function
void Wait()
{
   uint8_t i;
   for(i=0;i<10;i++)
   {
      _delay_loop_2(0);
      _delay_loop_2(0);
      _delay_loop_2(0);
   }

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


int main()
{
    motors_init();
    initialize_diodes();
    initialize_us();

    duty_motor_1 = 105;
    duty_motor_2 = 105;

    OCR0A = duty_motor_1;
    OCR2A = duty_motor_2;

    uint16_t distance;
    uint16_t direction;
    

    //Configure TIMER1
    TCCR3A|=(1<<COM3A1)|(1<<COM3B1)|(1<<WGM31);        //NON Inverted PWM
    TCCR3B|=(1<<WGM33)|(1<<WGM32)|(1<<CS31)|(1<<CS30); //PRESCALER=64 MODE 14(FAST PWM)

    ICR3=4999;  //fPWM=50Hz (Period = 20ms Standard).

    DDRC|= (1<<PC6);   //PWM Pins as Out
    
    uint16_t angles[] = {186, 220, 286, 414, 286, 220};
    uint16_t indexes[] = {0, 1, 2, 3, 2, 1};
    uint16_t distance_values[] = {0, 0, 100, 0, 0};
    
    int i = 0;
    while(1){
        _delay_ms(50); 
        OCR3A = angles[i];  //90 degree
        _delay_ms(100);
        distance = measure_distance();
        distance_values[indexes[i]] = distance;
        
        i = (i + 1) % 5; 

        turn_off_diodes();
        if(distance == INVALID_DISTANCE) {
            toggle_red_diode();
        } else if (distance == INFINITE_DISTANCE) {
            toggle_green_diode(); 
        } else {
            if(distance_values[2] > 10) {
                forwards();
                direction = index_of_max(distance_values, 5);
                toggle_yellow_diode();
                _delay_ms(50);
                turn_off_diodes();
                OCR0A = duty_motor_1 - (direction - 2) * 40;
                OCR2A = duty_motor_2;
            }
            else {
                toggle_yellow_diode();

                OCR0A = 0;
                OCR2A = 0;

                _delay_ms(100);
                change_direction();
                _delay_ms(100);
                direction = index_of_max(distance_values, 5);
                OCR0A = duty_motor_1;
                OCR2A = duty_motor_2 - (direction -2 ) * 40;
                
                _delay_ms(500);
            }
        }

        _delay_ms(50);
    }
}
