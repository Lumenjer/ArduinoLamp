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
// ----------------- LIBRARES -----------------
#include <FastLED.h>
#include <EEPROM.h>
// ----------------- VARIABLES ------------------
#include "Constants.h"
#define NUM_LEDS WIDTH * HEIGHT
CRGB leds[NUM_LEDS];
boolean loadingFlag = true;
boolean ONflag = true;
byte numHold;
byte palette;
boolean recievedFlag;
unsigned long numHold_Timer = 0;
uint32_t DemTimer = 0UL;
//-------------------------------------------------------------
// ----------- START ------------------
static const byte maxDim = max(WIDTH, HEIGHT);
void setup() {
  // STRIP
  FastLED.addLeds<WS2812B, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(0xFFB0F0);
  FastLED.setBrightness(BRIGHTNESS);
  if (CURRENT_LIMIT > 0) FastLED.setMaxPowerInVoltsAndMilliamps(5, CURRENT_LIMIT);
  memset8( leds, 0, NUM_LEDS * 3);
  FastLED.show();
#ifdef DEBUG
  Serial.begin(9600);
  Serial.println();
#endif
  SetUP();
  if (EEPROM.read(0) == 102) {
    currentMode = EEPROM.read(1);
    for (byte x = 0; x < MODE_AMOUNT; x++) {
      Brightness[x] = EEPROM.read(x * 3 + 11);
      Speed[x] = EEPROM.read(x * 3 + 12);
      Scale[x] = EEPROM.read(x * 3 + 13);
    }
  }
}
void loop() {
  effectsTick();
  controlTick();
  demoTick();
}