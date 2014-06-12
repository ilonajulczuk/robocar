#Assembled robot

![zenon-bot](http://i.imgur.com/35FpQ5d.jpg)

It's a little wibbly-wobbly cutie.

#PARTS

 - atmega32u4 board
 - sparkfun redbot chassis
 - drv8835 dual motor driver carrier
 - HC-SR04 ultrasonic sensor
 - servo-motor
 - lots of duct tape and zip tie

![Parts](http://i.imgur.com/P2JPmtV.jpg)

![duct tape](http://cdn1.bigcommerce.com/server600/1r2fayi/product_images/uploaded_images/duct-tape.jpg)

![zip tie](http://ecx.images-amazon.com/images/I/61F72PN1AZL.gif)

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

Ultrasonic motor is placed on a servo motor which rotates it. This setup creates
a radar which enables robot to scan it's environment and choose the best path to go.

##DIODES

I use blinking diodes to debug how sensors are working using color codes.

- green - no obstacle detected in front of the robot (rare)
- red - distance sensor not working correctly
- white
    - blinking - can ride without problems
    - constant light - obstacle detected really near the robot, robot needs to reverse immediately

##Servo motor

I used small servo of unknow brand. It was lying around.

It has three inputs:

- VCC
- GND
- control signal

I control it using PWM on 50 Hz. Different duty cycles enable me to set servo
to different angles.

##Schematic

Here is a design picture created using [fritzing](http://fritzing.org).

![Imgur](http://i.imgur.com/HJUOlHg.png)

Design is a bit messy, but it's only a prototype on breadboard. I haven't done
any pcb yet.


##Software

Software is written in C for atmega32u4.


###Initialization

Programs starts with initializations.

Here are initialized components:

- motors
- diodes
- ultrasonic sensor
- radar (servo rotating ultrasonic sensor)

###Moving forward

In main loop everything starts from radar measuring a distance and 
storing it to appropriate cell in table of measured distance for different angles.

Then robot checks how much space is available for it if running forwards.
If sensor works correctly and there is no obstacle nearer than 10 cm, robot
chooses the best direction to turn based on amount of available space.
The farther from obstacle, the better.

###Avoiding obstacles

If sensor detects an obstacle in front of it, it reverses, turning in the same time.

The choice of angle is based on `distance_values` for angles.

After that it stops and scans an environment with radar to update `distance_values.


