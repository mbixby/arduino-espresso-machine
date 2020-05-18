// /*
//  * See README.md
//  *
//  * USE AT YOUR OWN RISK.
//  * This project is posted for educational use only. It involves electricity,
//  * high heat, and water/steam.
//  * I will not be held liable damages or injuries resulting from the use of this code
//  * or from copying this project.
//  */
// #include <Arduino.h>
// #include <RBD_Timer.h>
// #include <RBD_Button.h>
// #include <RBD_Light.h>
// #include "pins.cpp"
// #include "water_level.cpp"

// RBD::Button button1(BTN1);
// RBD::Button button2(BTN2);
// RBD::Button button3(BTN3);
// RBD::Button button4(BTN4);

// // On-off is reversed (LEDs are connected
// // to +5V pin and PWM output pin)
// RBD::Light light1(LED1);
// RBD::Light light2(LED2);
// RBD::Light light3(LED3);
// RBD::Light light4(LED4);

// int lastMachineState = -1;
// int lastMachineStateChangeTime = 0;
// boolean doesUserWantDispense = false;
// boolean lastNeedsBoilerRefill = false;

// void setInitialMachineState()
// {
//   // Turn off LEDs
//   light1.on();
//   light2.on();
//   light3.on();
//   light4.on();
//   // TODO Reset relays here without causing re-trigger
// }

// void setMachineState(int newState, bool needsBoilerRefill)
// {
//   boolean stateDidChange = lastMachineState != newState ||
//                            lastNeedsBoilerRefill != needsBoilerRefill;
//   if (!stateDidChange)
//   {
//     return;
//   }

//   // Time elapsed since the last state change
//   int elapsed = millis() - lastMachineStateChangeTime;

//   // Water level should not be at the tip of the probe since the boiling
//   // water could cause jitter. Minimum boiler fill time should avoid that.
//   // Empty tank will always stop boiler fill.
//   if (lastMachineState == FILLING_BOILER && newState == IDLE &&
//       elapsed < MIN_BOILER_FILL_TIME)
//   {
//     Serial.println("Throttling boiler fill");
//     return;
//   }

//   setInitialMachineState();

//   digitalWrite(REL_BOILER, needsBoilerRefill ? REL_OFF : REL_ON);

//   if (newState == IDLE)
//   {
//     Serial.println("Changing state IDLE");
//     // Everything off
//     digitalWrite(REL_SLN, REL_OFF);
//     digitalWrite(REL_PUMP, REL_OFF);
//   }
//   else if (newState == FILLING_BOILER)
//   {
//     Serial.println("Changing state FILLING_BOILER");
//     // Boiler fill solenoid + pump
//     digitalWrite(REL_SLN, REL_OFF);
//     digitalWrite(REL_PUMP, REL_ON);
//   }
//   else if (newState == DISPENSING)
//   {
//     Serial.println("Changing state DISPENSING");
//     // E61 solenoid + pump
//     digitalWrite(REL_SLN, REL_ON);
//     digitalWrite(REL_PUMP, REL_ON);
//     // LED stopwatch
//     light4.fade(100, 1900, 100, 1900);
//   }
//   else if (newState == NEEDS_TANK_REFILL)
//   {
//     Serial.println("Changing state NEEDS_TANK_REFILL");
//     // Everything off
//     digitalWrite(REL_SLN, REL_OFF);
//     digitalWrite(REL_PUMP, REL_OFF);
//     // Flash LEDs
//     light1.fade(300, 100, 1200, 300);
//     light2.fade(300, 100, 1200, 300);
//     light3.fade(300, 100, 1200, 300);
//     light4.fade(300, 100, 1200, 300);
//   }

//   lastMachineStateChangeTime = millis();
//   lastMachineState = newState;
//   lastNeedsBoilerRefill = needsBoilerRefill;
// }

// void setup()
// {
//   digitalWrite(REL_SLN, REL_OFF);
//   digitalWrite(REL_PUMP, REL_OFF);
//   digitalWrite(REL_BOILER, REL_OFF);
//   pinMode(REL_PUMP, OUTPUT);
//   pinMode(REL_SLN, OUTPUT);
//   pinMode(REL_BOILER, OUTPUT);

//   pinMode(TANK_PIN, OUTPUT);
//   pinMode(BOILER_PIN, OUTPUT);
//   pinMode(CHASIS_PIN, INPUT);
//   pinMode(FLW_PIN, INPUT);

//   setMachineState(IDLE, false);

//   Serial.begin(9600);
// }

// void loop()
// {
//   light1.update();
//   light2.update();
//   light3.update();
//   light4.update();

//   // Read inputs
//   WaterLevelState waterLevelState = senseWaterLevel();
//   if (button4.onPressed())
//   {
//     Serial.println("Btn pressed");
//     doesUserWantDispense = !doesUserWantDispense;
//   }

//   // Compute next state from inputs
//   int newState = IDLE;

//   if (waterLevelState.tank == NEEDS_FILL)
//   {
//     newState = NEEDS_TANK_REFILL;
//     // Never start filling boier if already dispensing.
//   }l
//   else if (waterLevelState.boiler == NEEDS_FILL && lastMachineState != DISPENSING)
//   {
//     newState = FILLING_BOILER;
//   }
//   else if (doesUserWantDispense)
//   {
//     newState = DISPENSING;
//   }

//   // If the tank/boiler level is too low user will have to press the button
//   // again. (Don't accidentally resume dispensing.)
//   if (newState != DISPENSING)
//   {
//     doesUserWantDispense = false;
//   }

//   // Persist state
//   setMachineState(newState, waterLevelState.boiler == NEEDS_FILL);
// }

// // TODO:
// // - programmable BTN1-3 (flowmeter, eeprom)
// // - BTN1+BTN4 cleaning mode 5x(15s + 20s pause)
// // - RBD_Light - reverse LED on/off