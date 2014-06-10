#include <avr/io.h>
#include <util/delay.h>
#include "ultrasonic_sensor.h"

#define MODE PD3
#define MOTOR_1_PHASE PD1
#define MOTOR_2_PHASE PD2
#define MAX_SPEED 255
#define MIN_SPEED 0

volatile uint16_t duty_motor_1, duty_motor_2;


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


int main()
{
    motors_init();
    initialize_diodes();
    initialize_us();

    duty_motor_1 = 65;
    duty_motor_2 = 65;

    OCR0A = duty_motor_1;
    OCR2A = duty_motor_2;

    int distance;
    uint8_t i;

    int MAX_BACK_UP = 10;
    //Configure TIMER1
    TCCR1A|=(1<<COM1A1)|(1<<COM1B1)|(1<<WGM11);        //NON Inverted PWM
    TCCR1B|=(1<<WGM13)|(1<<WGM12)|(1<<CS11)|(1<<CS10); //PRESCALER=64 MODE 14(FAST PWM)

    ICR1=4999;  //fPWM=50Hz (Period = 20ms Standard).

    DDRB|= (1<<PB5);   //PWM Pins as Out


    while(1){
        OCR1A=216;  //90 degree
        _delay_ms(50);

        
        OCR1A=435;  //180 degree
        _delay_ms(50);

        OCR1A=216;  //90 degree
        _delay_ms(50);

        OCR1A=325;  //135 degree
        _delay_ms(50);

        OCR1A=167;   //0 degree
        _delay_ms(50);

        OCR0A = duty_motor_1;
        OCR2A = duty_motor_2;
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

                OCR0A = 0;
                OCR2A = 0;

                _delay_ms(100);
                change_direction();
                OCR0A = duty_motor_1;
                OCR2A = duty_motor_2;
                for(i=0; i < MAX_BACK_UP; i++) {
                    _delay_ms(50);
                }
                change_direction();
            }
        }

        _delay_ms(50);
    }
}
