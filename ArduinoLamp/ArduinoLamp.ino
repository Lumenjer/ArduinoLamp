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
    Brightness = EEPROM.read(currentMode * 3 + 11);
    Speed = EEPROM.read(currentMode * 3 + 12);
    Scale = EEPROM.read(currentMode * 3 + 13);

  }
#if defined(MOSFET_PIN) && defined(MOSFET_LEVEL)          // установка сигнала в пин, управляющий MOSFET транзистором, соответственно состоянию вкл/выкл матрицы
  digitalWrite(MOSFET_PIN, ONflag ? MOSFET_LEVEL : !MOSFET_LEVEL);
#endif
}

void loop() {
  effectsTick();
  controlTick();
  demoTick();
}
