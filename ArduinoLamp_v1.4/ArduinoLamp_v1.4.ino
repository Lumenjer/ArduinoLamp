/*
  Скетч к проекту "Многофункциональный RGB светильник"
  Страница проекта (схемы, описания): https://alexgyver.ru/GyverLamp/
  Нравится, как написан код? Поддержи автора! https://alexgyver.ru/support_alex/
  Автор: AlexGyver, AlexGyver Technologies, 2019(Портировал на Ардуино Norovl(+ эффекты, демо от stepko365)
  https://AlexGyver.ru/
*/
// ---------------- БИБЛИОТЕКИ -----------------
//#include <EEPROM.h>
#include <FastLED.h>
#include <GyverButtonOld.h>
//-----------------            -----------------
#include "Constants.h"
// ----------------- ПЕРЕМЕННЫЕ ------------------
static const byte maxDim = max(WIDTH, HEIGHT);
byte Brightness = 10;
byte Speed = 30;
byte Scale = 10;
int8_t currentMode = 10;
boolean loadingFlag = true;
byte numHold;
byte palette;
unsigned long numHold_Timer = 0;
unsigned long userTimer = 0UL;
int val1;
int val2;
int val3;

void setup() {
  // ЛЕНТА
  FastLED.addLeds<WS2812B, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(0xFFB0F0);
  FastLED.setBrightness(BRIGHTNESS);
  if (CURRENT_LIMIT > 0) FastLED.setMaxPowerInVoltsAndMilliamps(5, CURRENT_LIMIT);
  memset8( leds, 0, NUM_LEDS * 3);
  FastLED.show();
#ifdef DEBUG
  Serial.begin(9600);
  Serial.println();
#endif
  touch.setStepTimeout(100);
  touch.setClickTimeout(500);


}

void loop() {
  effectsTick();
  buttonTick();
}
