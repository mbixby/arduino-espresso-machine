/*
 * See README.md
 *
 * USE AT YOUR OWN RISK.
 * This project is posted for educational use only. It involves electricity,
 * high heat, and water/steam.
 * I will not be held liable damages or injuries resulting from the use of this code
 * or from copying this project.
 */
#include <Bounce2.h>

#define BOILER_LEVEL_SENSOR_PIN A0
#define BOILER_LEVEL_VIN_PIN 2
#define PUMP_SWITCH_PIN 3 // SSR
#define BOILER_FILL_SOLENOID_PIN 4 // SSR
#define GROUPHEAD_SOLENOID_PIN 5 // SSR
#define BUTTON_PIN 6

#define IDLE 0
#define FILLING_BOILER 1
#define DISPENSING 2

#define PUMP_DELAY 1000

Bounce button = Bounce(); 

int machineState = IDLE;
boolean doesUserWantDispense = false;
int lastPumpSwitch = 0;
int lastBoilerFillSenseTime = 0;
boolean lastBoilerFillSenseValue = false;

// Boiler water level probe is sensed each 1s to prevent
// corrosion of the metal probe.
//
// BOILER_LEVEL_VIN_PIN supplies input voltage to the sensor.
boolean senseBoilerFill() {
  int interval = machineState == FILLING_BOILER ? 200 : 1000;
  if (lastBoilerFillSenseTime && millis() - lastBoilerFillSenseTime < interval){
    return lastBoilerFillSenseValue;
  }
  lastBoilerFillSenseTime = millis();
  // digitalWrite(LED_BUILTIN, HIGH);
  // delay(50);
  // digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(BOILER_LEVEL_VIN_PIN, HIGH);
  int boilerLevelInputValue = analogRead(BOILER_LEVEL_SENSOR_PIN);
  digitalWrite(BOILER_LEVEL_VIN_PIN, LOW);
  lastBoilerFillSenseValue = boilerLevelInputValue < 100;
  return lastBoilerFillSenseValue;
}

void setMachineState(int newState) {
  if (machineState == newState){
    return;
  }
  // Ensure at least PUMP_DELAY between switching pump on/off
  if (lastPumpSwitch && millis() - lastPumpSwitch < PUMP_DELAY){
    Serial.println("Throttling pump switch");
    return;
  }
  if (newState == IDLE){
    Serial.println("Changing state IDLE");
    // Everything off
    digitalWrite(GROUPHEAD_SOLENOID_PIN, LOW);
    digitalWrite(BOILER_FILL_SOLENOID_PIN, LOW);
    digitalWrite(PUMP_SWITCH_PIN, LOW);
  } else if (newState == FILLING_BOILER){
    Serial.println("Changing state FILLING_BOILER");
    // SN1+PUMP
    digitalWrite(GROUPHEAD_SOLENOID_PIN, LOW);
    digitalWrite(BOILER_FILL_SOLENOID_PIN, HIGH);
    digitalWrite(PUMP_SWITCH_PIN, HIGH);
  } else if (newState == DISPENSING){
    Serial.println("Changing state DISPENSING");
    // SN2+PUMP
    digitalWrite(GROUPHEAD_SOLENOID_PIN, HIGH);
    digitalWrite(BOILER_FILL_SOLENOID_PIN, LOW);
    digitalWrite(PUMP_SWITCH_PIN, HIGH);
  }
  lastPumpSwitch = millis();
  machineState = newState;
}

void setup() {
  pinMode(BOILER_LEVEL_SENSOR_PIN, INPUT);
  pinMode(BOILER_LEVEL_VIN_PIN, OUTPUT);
  pinMode(PUMP_SWITCH_PIN, OUTPUT);
  pinMode(BOILER_FILL_SOLENOID_PIN, OUTPUT);
  pinMode(GROUPHEAD_SOLENOID_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);

  button.attach(BUTTON_PIN);
  button.interval(50);

  setMachineState(IDLE);

  Serial.begin(9600);
}

void loop() {
  boolean doesNeedBoilerFill = senseBoilerFill();
  button.update();
  if (button.fell()){
    doesUserWantDispense = !doesUserWantDispense;
  }
  int newState = IDLE;
  // Never start filling boiler if already dispensing.
  // TODO Dispensing should have a limit to prevent boiler from running dry.
  if (doesNeedBoilerFill && machineState != DISPENSING){
    newState = FILLING_BOILER;
  // TODO Never start dispensing if the boiler is dry to prevent coil damage
  } else if (doesUserWantDispense) {
    newState = DISPENSING;
  }
  setMachineState(newState);
}