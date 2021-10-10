// =================== ATTENTION !!! ===========================
// Almost all settings are made on the Constants.h tab
// Next, go to Control.ino to configure controling.
// =============================================================
/*
  Sketch for the project "Multifunctional RGB lamp"
  Project page (schemes, descriptions): https://alexgyver.ru/GyverLamp/
  Do you like how the code is written? Supports the author! https://alexgyver.ru/support_alex/
  Author: AlexGyver, AlexGyver Technologies, 2019 (Ported to Arduino Norovl (+ effects, demo from Stepko)
  https://AlexGyver.ru/
*/

#ifndef _MAIN_H
#define _MAIN_H

// ----------------- LIBRARES -----------------
#include "Arduino.h"
#include "Constants.h"
#include "FastLED.h"
#include <EEPROM.h>
#include "effectTicker.h"
#include "effects.h"
#include "utility.h"
#include "Control.h"
#include "R_text.h"
// ----------------- VARIABLES ------------------
#define NUM_LEDS WIDTH * HEIGHT
extern CRGB leds[NUM_LEDS];
extern boolean loadingFlag;
extern boolean ONflag;
extern byte numHold;
extern byte palette;
extern boolean recievedFlag;
extern unsigned long numHold_Timer;
extern uint32_t DemTimer;
//-------------------------------------------------------------
// ----------- START ------------------
extern const byte maxDim;
#endif // _MAIN_H