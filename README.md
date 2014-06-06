#PARTS

 - atmega32u4 board
 - sparkfun redbot chassis
 - drv8835 dual motor driver carrier
 - lots of sensors

![Parts](http://i.imgur.com/P2JPmtV.jpg)

##Motor control

![schematic](http://botland.com.pl/img/p/851-6079.jpg)

PWM1 at PB4 and PWM2 at PB7.

To control motors I use DRV8835 driver.

This driver needs voltage supply, for logic and for motors.

I feed both VCC from my board VCC. Currently I don't use any
external power source except from USB connected to board.

Motor driver has two channels, that enable it to control two motors.

Motor driver has two possible modes of operation. I use the simpler one.

To control motor you need to specify phase (should it rotate clockwise or
anticlockwise?) and speed. Phase is specifide by logic value on digital
pin of motor driver and speed is controlled by PWM pin.


##Ultrasonic sensor

Robocar uses HC-SR04 sensor.

I wrote a simple library to interface with it.

![mounted sensor](http://i.imgur.com/35iVqjd.jpg) 

This sensor is really easy to use. It has VCC, ground, trigger which waits
for inputs and echo pin which outputs signal of length proportional to
distance of an obstacle.

Sound wave travels to obstacle and back to sensor. If we divide time it takes
by two and speed of sound we get distance.

I used this [tutorial](http://extremeelectronics.co.in/avr-tutorials/interfacing-ultrasonic-rangefinder-with-avr-mcus-%E2%80%93-avr-tutorial/)
which uses similar sensor (but with only one pin for communication with a sensor).

##DIODES

I use blinking diodes to debug how sensors are working using color codes.

##TODO

I have two more additional distance sensors from sharp. I will use them to scan both sides
of the robot.
