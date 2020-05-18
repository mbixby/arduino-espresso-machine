#ifndef PINS_H
#define PINS_H

#include <Arduino.h>

#define TANK_PIN 2   // +5V
#define BOILER_PIN 3 // +5V
// All blinking - error
#define LED1 10
#define LED2 9
#define LED3 6
#define LED4 5
// Press to start dispensing for a predefined volume. (BTN1-3)
// Press and hold to program volume.
#define BTN1 11
#define BTN2 12
#define BTN3 8
// Manual operation. Press to start / stop dispensing.
#define BTN4 7
// Tank + boiler sensor pin
#define CHASIS_PIN A0
// Flowmeter input
#define FLW_PIN A1
// Inactive = boiler fill solenoid on, active = E61 solenoid on
#define REL_SLN A3
// Active = boiler on
#define REL_BOILER A4
// Active = pump on
#define REL_PUMP A2

// Relays are active on LOW
#define REL_OFF HIGH
#define REL_ON LOW

// Machine state
#define IDLE 0
#define FILLING_BOILER 1
#define DISPENSING 2
#define NEEDS_TANK_REFILL 3

#define OK true
#define NEEDS_FILL false

#define MIN_BOILER_FILL_TIME 4000
#define WATER_PROBE_INTERVAL 1000

#endif