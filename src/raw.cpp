/*
 * This is a raw interface to boiler functions with no automation and no safety checks
 * (boiler tank doesn't automatically refill).
 *
 * BTN1: toggle boiler; LED1 on = boiler on
 * BTN2: toggle boiler fill; LED2 on = boiler water level low
 * BTN3: toggle dispense; LED3 on = dispensing
 * BTN4: nothing; LED4 blinking = tank level low
 *
 * See README.md
 *
 * USE AT YOUR OWN RISK.
 * This project is posted for educational use only. It involves electricity,
 * high heat, and water/steam.
 * I will not be held liable damages or injuries resulting from the use of this code
 * or from copying this project.
 */
#include <Arduino.h>
#include <RBD_Timer.h>
#include <RBD_Button.h>
#include <RBD_Light.h>
#include "pins.h"
#include "water_level.h"

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

RBD::Timer logTimer(1000);

boolean isBoilerOn = false;
boolean isFillingBoiler = false;
boolean isDispensing = false;

void resetMachineState()
{
  // Turn off LEDs
  light1.on();
  light2.on();
  light3.on();
  light4.on();
  // TODO Reset relays here without causing re-trigger
}

String boolToStr(boolean val)
{
  return val ? "yes" : "no";
}

void setup()
{
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

  logTimer.restart();

  Serial.begin(9600);
}

void loop()
{
  light1.update();
  light2.update();
  light3.update();
  light4.update();

  // Read inputs
  WaterLevelState waterLevelState = senseWaterLevel();

  resetMachineState();

  if (button1.onPressed())
  {
    isBoilerOn = !isBoilerOn;
    digitalWrite(REL_BOILER, isBoilerOn ? REL_ON : REL_OFF);
    delay(50);
  }

  if (button2.onPressed())
  {
    isFillingBoiler = !isFillingBoiler;
    // Boiler fill solenoid + pump
    if (isFillingBoiler)
    {
      digitalWrite(REL_SLN, REL_OFF);
      // My circuit couldn't handle the switch current when turning
      // both relays on at the same time.
      delay(50);
      digitalWrite(REL_PUMP, REL_ON);
      delay(50);
    }
    else
    {
      digitalWrite(REL_PUMP, REL_OFF);
      delay(50);
      digitalWrite(REL_SLN, REL_OFF);
      delay(50);
    }
  }

  if (button3.onPressed())
  {
    isDispensing = !isDispensing;
    // E61 solenoid + pump
    if (isDispensing)
    {
      digitalWrite(REL_SLN, REL_ON);
      delay(50);
      digitalWrite(REL_PUMP, REL_ON);
      delay(50);
    }
    else
    {
      digitalWrite(REL_PUMP, REL_OFF);
      delay(50);
      digitalWrite(REL_SLN, REL_OFF);
      delay(50);
    }
  }

  if (isBoilerOn)
  {
    light1.off();
  }
  else
  {
    light1.on();
  }

  if (waterLevelState.boiler == NEEDS_FILL)
  {
    light2.off();
  }
  else
  {
    light2.on();
  }

  if (isDispensing)
  {
    light3.off();
  }
  else
  {
    light3.on();
  }

  if (waterLevelState.tank == NEEDS_FILL)
  {
    light4.fade(300, 100, 1200, 300);
  }
  else
  {
    light4.on();
  }

  if (logTimer.onExpired())
  {
    Serial.println("isBoilerOn: " + boolToStr(isBoilerOn));
    Serial.println("isDispensing: " + boolToStr(isDispensing));
    Serial.println("isFillingBoiler: " + boolToStr(isFillingBoiler));
    logTimer.restart();
  }
}