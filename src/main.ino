/*
 * See README.md
 *
 * USE AT YOUR OWN RISK.
 * This project is posted for educational use only. It involves electricity,
 * high heat, and water/steam.
 * I will not be held liable damages or injuries resulting from the use of this code
 * or from copying this project.
 */
#include <RBD_Timer.h>
#include <RBD_Button.h>
#include <RBD_Light.h>

#define TANK_PIN 2 // +5V
#define BOILER_PIN 3 // +5V
#define LED1 10
#define LED2 9
#define LED3 6
#define LED4 5
#define BTN1 11
#define BTN2 12
#define BTN3 8
#define BTN4 7
// tank + boiler sensor pin
#define CHASIS_PIN A0
// flowmeter input
#define FLW_PIN A1
// inactive = boiler fill solenoid on, active = E61 solenoid on
#define REL_SLN A4
// active = pump on
#define REL_PUMP A3
// active = pump on
#define REL_BOILER A2

// Relays are active on LOW
#define REL_OFF HIGH
#define REL_ON LOW

// Machine state
#define IDLE 0
#define FILLING_BOILER 1
#define DISPENSING 2

#define MIN_BOILER_FILL_TIME 4000
#define WATER_PROBE_INTERVAL 1000

RBD::Button button1(BTN1);
RBD::Button button2(BTN2);
RBD::Button button3(BTN3);
RBD::Button button4(BTN4);

// On-off is reversed (LEDs are connected
// to +5V pin and PWM output pin)
RBD::Light light1(LED1);
RBD::Light light2(LED2);
RBD::Light light3(LED3);
RBD::Light light4(LED4);

int lastMachineState = -1;
int lastBoilerFillSenseTime = 0;
int lastMachineStateChangeTime = 0;
boolean doesUserWantDispense = false;
boolean lastBoilerFillSenseValue = false;

// Water level in the boiler and the tank is sensed each 1s to prevent
// corrosion of the metal probes.
//
// BOILER_PIN / TANK_PIN supplies input voltage to the sensor.
// TODO Debounce (boiling water in the tank)
boolean senseBoilerFill() {
  int elapsed = millis() - lastBoilerFillSenseTime;
  if (lastBoilerFillSenseTime && elapsed < WATER_PROBE_INTERVAL){
    return lastBoilerFillSenseValue;
  }
  lastBoilerFillSenseTime = millis();
  digitalWrite(BOILER_PIN, HIGH);
  int boilerLevelInputValue = analogRead(CHASIS_PIN);
  digitalWrite(BOILER_PIN, LOW);
  lastBoilerFillSenseValue = boilerLevelInputValue < 100;
  return lastBoilerFillSenseValue;
}

void setInitialMachineState() {
  digitalWrite(REL_BOILER, REL_ON);

  // Turn off LEDs
  light1.on();
  light2.on();
  light3.on();
  light4.on();
}

void setMachineState(int newState) {
  if (lastMachineState == newState){
    return;
  }

  // Time elapsed since the last state change
  int elapsed = millis() - lastMachineStateChangeTime;

  // Ensure at least PUMP_DELAY between switching pump on/off
  if (lastMachineState == FILLING_BOILER && newState == IDLE &&
    elapsed < MIN_BOILER_FILL_TIME){
    Serial.println("Throttling boiler fill");
    return;
  }

  setInitialMachineState();

  if (newState == IDLE){
    Serial.println("Changing state IDLE");
    // Everything off
    digitalWrite(REL_SLN, REL_OFF);
    digitalWrite(REL_PUMP, REL_OFF);
  } else if (newState == FILLING_BOILER){
    Serial.println("Changing state FILLING_BOILER");
    // Boiler fill solenoid + pump
    digitalWrite(REL_SLN, REL_OFF);
    digitalWrite(REL_PUMP, REL_ON);
  } else if (newState == DISPENSING){
    Serial.println("Changing state DISPENSING");
    light1.off();
    // E61 solenoid + pump
    digitalWrite(REL_SLN, REL_ON);
    digitalWrite(REL_PUMP, REL_ON);
  }
  lastMachineStateChangeTime = millis();
  lastMachineState = newState;
}

void setup() {
  digitalWrite(REL_SLN, REL_OFF);
  digitalWrite(REL_PUMP, REL_OFF);
  digitalWrite(REL_BOILER, REL_OFF);
  pinMode(REL_PUMP, OUTPUT);
  pinMode(REL_SLN, OUTPUT);
  pinMode(REL_BOILER, OUTPUT);
  
  pinMode(TANK_PIN, OUTPUT);
  pinMode(BOILER_PIN, OUTPUT);
  pinMode(CHASIS_PIN, INPUT);
  pinMode(FLW_PIN, INPUT);

  setMachineState(IDLE);

  Serial.begin(9600);
}

void loop() {
  light1.update();
  light2.update();
  light3.update();
  light4.update();

  // Read inputs
  boolean doesNeedBoilerFill = senseBoilerFill();
  if (button1.onPressed()){
    Serial.println("Btn pressed");
    doesUserWantDispense = !doesUserWantDispense;
  }

  // Compute next state from inputs
  int newState = IDLE;
  // Never start filling boiler if already dispensing.
  if (doesNeedBoilerFill && lastMachineState != DISPENSING){
    newState = FILLING_BOILER;
  } else if (doesUserWantDispense) {
    newState = DISPENSING;
  }

  // Persist state
  setMachineState(newState);
}

// TODO:
// - min boiler fill time
// - BTN4 manual op
// - test current version
// - leds on while dispensing
// - tank level
// - boiler switch
// - better pump throttle
// - LED timer while dispensing (flow + time), blink + fade? 
// - BTN1-3 programmable
// - BTN1+BTN4 cleaning mode 5x(15s + 20s pause)
// - Dispensing should have a limit to prevent boiler from running dry.
// - Never start dispensing if the boiler is dry to prevent coil damage