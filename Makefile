ARDUINO_DIR	= /usr/share/arduino/
AVR_TOOLS_PATH	= /usr/bin
AVRDUDE_CONF	= /etc/avrdude.conf
AVRDUDE_ARD_BAUDRATE	= 115200
AVRDUDE_ARD_PROGRAMMER	= arduino

TARGET		= Arduino-64
ARDUINO_LIBS	= 

MCU		= atmega168
F_CPU		= 8000000
ARDUINO_PORT	= /dev/arduino

include Arduino.mk
