#include <Arduino.h>
#include "pins.h"
#include "water_level.h"

int lastWaterLevelSenseTime = 0;
WaterLevelState lastWaterLevelState = {.boiler = OK, .tank = OK};

// Water level in the boiler and the tank is sensed every 1s to prevent
// corrosion of the metal probes. Voltage/current from digital pins should
// be limited further to minimise electrolysis.
//
// BOILER_PIN and TANK_PIN are alternated to supply input current
// which runs through the boiler (or tank) and is then read by CHASIS_PIN.
//
// Boiling water will cause fluctuating resistance so it's important that
// the water level in the boiler is not right at the tip of the probe.
//
// Filtered water has different resistance to unfiltered water.
//
// The resistance of water and chasis could be more than 50Ω. This also
// means that multimeter continuity test may not work to check the water
// level.
//
// TODO Debounce (boiling water in the tank)
WaterLevelState senseWaterLevel()
{
  int elapsed = millis() - lastWaterLevelSenseTime;
  if (lastWaterLevelSenseTime && elapsed < WATER_PROBE_INTERVAL)
  {
    return lastWaterLevelState;
  }
  lastWaterLevelSenseTime = millis();

  digitalWrite(BOILER_PIN, HIGH);
  int boilerPinValue = analogRead(CHASIS_PIN);
  digitalWrite(BOILER_PIN, LOW);

  digitalWrite(TANK_PIN, HIGH);
  int tankPinValue = analogRead(CHASIS_PIN);
  digitalWrite(TANK_PIN, LOW);

  if ((boilerPinValue + tankPinValue) < 500)
  {
    lastWaterLevelState.boiler = NEEDS_FILL;
    lastWaterLevelState.tank = NEEDS_FILL;
  }
  else if (boilerPinValue < 20)
  {
    lastWaterLevelState.boiler = NEEDS_FILL;
    lastWaterLevelState.tank = OK;
  }
  else if (tankPinValue < 20)
  {
    lastWaterLevelState.boiler = OK;
    lastWaterLevelState.tank = NEEDS_FILL;
  }
  else
  {
    lastWaterLevelState.boiler = OK;
    lastWaterLevelState.tank = OK;
  }

  // Serial.print(boilerPinValue, DEC);
  // Serial.println(" - boiler");
  // Serial.print(tankPinValue, DEC);
  // Serial.println(" - tank");
  // Serial.println("");

  return lastWaterLevelState;
}