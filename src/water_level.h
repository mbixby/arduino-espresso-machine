#ifndef WATER_LEVEL_H
#define WATER_LEVEL_H

#include <Arduino.h>

struct WaterLevelState_
{
  boolean boiler;
  boolean tank;
};

typedef struct WaterLevelState_ WaterLevelState;

WaterLevelState senseWaterLevel();

#endif