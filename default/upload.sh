#!/bin/bash
/usr/share/arduino/hardware/tools/avrdude -C/usr/share/arduino/hardware/tools/avrdude.conf -v -v -v -v -patmega32u4 -cavr109 -P/dev/ttyACM0 -b57600 -D -Uflash:w:robocar.hex:i
