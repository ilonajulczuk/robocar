#include <avr/io.h>
#include <util/delay.h>

#define MOTOR_1 PB5
#define MOTOR_2 PB6
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
  
    // make sure to make OC0 pin (pin PB3 for atmega32) as output pin
    DDRB |= 0xff;
}

void pwm_init_timer_two()
{
    // initialize TCCR2 as per requirement, say as follows
    TCCR2A |= (1<<WGM20)|(1<<COM2A1);
    
    TCCR2B |= (1<<CS20); 
    // make sure to make OC0 pin (pin PB3 for atmega32) as output pin
    DDRB |= (1<<MOTOR_2);

}

void motors_init() {
    pwm_init_timer_zero();
    pwm_init_timer_two();
    DDRD |= (1 << MOTOR_1_PHASE) | (1 << MOTOR_2_PHASE) | (1 << MODE);
    PORTD |= (1 << MOTOR_1_PHASE) | (1 << MOTOR_2_PHASE) | (1 << MODE);
}
     
void change_direction() {
    PORTD ^= (1 << MODE);
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

int main()
{
    motors_init();
    duty_motor_1 = 215;       // duty cycle = 45% of 255 = 114.75 = 115
    duty_motor_2 = 215;
  
    // initialize timer in PWM mode
    
    uint8_t i;
    // run forever

    
    while(1)
    {
        change_direction();
        for(i=190; i > 50; i -= 20) {
            OCR0A = i;
            OCR2A = i;
            _delay_ms(200);
        }
    }
}
