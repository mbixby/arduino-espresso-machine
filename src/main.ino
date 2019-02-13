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
#define REL_SLN A4
// Active = pump on
#define REL_PUMP A3
// Active = pump on
#define REL_BOILER A2

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

typedef struct {
  boolean boiler;
  boolean tank;
} WaterLevelState;

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
int lastWaterLevelSenseTime = 0;
int lastMachineStateChangeTime = 0;
boolean doesUserWantDispense = false;
boolean lastNeedsBoilerRefill = false;
WaterLevelState lastWaterLevelState = { .boiler = OK, .tank = OK };

// Water level in the boiler and the tank is sensed each 1s to prevent
// corrosion of the metal probes.
//
// BOILER_PIN / TANK_PIN supplies input voltage to the sensor.
// TODO Debounce (boiling water in the tank)
WaterLevelState senseWaterLevel() {
  int elapsed = millis() - lastWaterLevelSenseTime;
  if (lastWaterLevelSenseTime && elapsed < WATER_PROBE_INTERVAL){
    return lastWaterLevelState;
  }
  lastWaterLevelSenseTime = millis();
  digitalWrite(BOILER_PIN, HIGH);
  lastWaterLevelState.boiler = analogRead(CHASIS_PIN) < 100 ? NEEDS_FILL : OK;
  digitalWrite(BOILER_PIN, LOW);
  digitalWrite(TANK_PIN, HIGH);
  lastWaterLevelState.tank = analogRead(CHASIS_PIN) < 100 ? NEEDS_FILL : OK;
  digitalWrite(TANK_PIN, LOW);
  return lastWaterLevelState;
}

void setInitialMachineState() {
  // Turn off LEDs
  light1.on();
  light2.on();
  light3.on();
  light4.on();
  // TODO Reset relays here without causing re-trigger
}

void setMachineState(int newState, bool needsBoilerRefill) {
  boolean stateDidChange = lastMachineState != newState ||
    lastNeedsBoilerRefill != needsBoilerRefill;
  if (!stateDidChange){
    return;
  }

  // Time elapsed since the last state change
  int elapsed = millis() - lastMachineStateChangeTime;

  // Water level should not be at the tip of the probe since the boiling
  // water could cause jitter. Minimum boiler fill time should avoid that.
  // Empty tank will always stop boiler fill.
  if (lastMachineState == FILLING_BOILER && newState == IDLE &&
    elapsed < MIN_BOILER_FILL_TIME){
    Serial.println("Throttling boiler fill");
    return;
  }

  setInitialMachineState();
  
  digitalWrite(REL_BOILER, needsBoilerRefill ? REL_OFF : REL_ON);

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
    // E61 solenoid + pump
    digitalWrite(REL_SLN, REL_ON);
    digitalWrite(REL_PUMP, REL_ON);
    // LED stopwatch
    light4.fade(100, 1900, 100, 1900);
  } else if (newState == NEEDS_TANK_REFILL){
    Serial.println("Changing state NEEDS_TANK_REFILL");
    // Everything off
    digitalWrite(REL_SLN, REL_OFF);
    digitalWrite(REL_PUMP, REL_OFF);
    // Flash LEDs
    light1.fade(300, 100, 1200, 300);
    light2.fade(300, 100, 1200, 300);
    light3.fade(300, 100, 1200, 300);
    light4.fade(300, 100, 1200, 300);
  }

  lastMachineStateChangeTime = millis();
  lastMachineState = newState;
  lastNeedsBoilerRefill = needsBoilerRefill;
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

  setMachineState(IDLE, false);

  Serial.begin(9600);
}

void loop() {
  light1.update();
  light2.update();
  light3.update();
  light4.update();

  // Read inputs
  WaterLevelState waterLevelState = senseWaterLevel();
  if (button4.onPressed()){
    Serial.println("Btn pressed");
    doesUserWantDispense = !doesUserWantDispense;
  }

  // Compute next state from inputs
  int newState = IDLE;

  if (waterLevelState.tank == NEEDS_FILL){
    newState = NEEDS_TANK_REFILL;
  // Never start filling boiler if already dispensing.
  } else if (waterLevelState.boiler == NEEDS_FILL && lastMachineState != DISPENSING){
    newState = FILLING_BOILER;
  } else if (doesUserWantDispense) {
    newState = DISPENSING;
  }

  // Persist state
  setMachineState(newState, waterLevelState.boiler == NEEDS_FILL);
}

// TODO:
// - programmable BTN1-3 (flowmeter, eeprom)
// - BTN1+BTN4 cleaning mode 5x(15s + 20s pause)
// - RBD_Light - reverse LED on/off