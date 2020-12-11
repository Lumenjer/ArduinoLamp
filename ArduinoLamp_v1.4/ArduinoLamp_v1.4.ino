/*
  Скетч к проекту "Многофункциональный RGB светильник"
  Страница проекта (схемы, описания): https://alexgyver.ru/GyverLamp/
  Нравится, как написан код? Поддержи автора! https://alexgyver.ru/support_alex/
  Автор: AlexGyver, AlexGyver Technologies, 2019(Портировал на Ардуино Norovl(+ эффекты, демо от stepko) 
  https://AlexGyver.ru/
*/
// ---------------- БИБЛИОТЕКИ -----------------
#include <FastLED.h>
#include <EEPROM.h>
//-----------------            -----------------
#include "Constants.h"
#ifdef TEXTo
#include "fonts.h"
#endif
// ----------------- ПЕРЕМЕННЫЕ ------------------
static const byte maxDim = max(WIDTH, HEIGHT);
byte Brightness = 10; byte Speed = 30; byte Scale = 10;
String runningText = "";
int8_t currentMode = 2;
boolean loadingFlag = true;
boolean ONflag = true;
byte palette; //палитры/Другой Масштаб
boolean fullTextFlag = false;
uint32_t DemTimer = 0UL;                      // тут будет храниться время следующего переключения эффекта
boolean BTcontrol = false;
uint32_t effTimer;
//int8_t hrs = 10, mins = 25, secs;

void setup() {
  // ЛЕНТА
  randomSeed(analogRead(1));
  FastLED.addLeds<WS2812B, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(0xFFB0F0);
  FastLED.setBrightness(BRIGHTNESS);
  if (CURRENT_LIMIT > 0) FastLED.setMaxPowerInVoltsAndMilliamps(5, CURRENT_LIMIT);
  memset8( leds, 0, NUM_LEDS * 3);
  FastLED.show();
  Serial.begin(9600);
  Serial.println();
    if (EEPROM.read(0) == 102) {                    // если было сохранение настроек, то восстанавливаем их (с)НР
    currentMode = EEPROM.read(1);
      Brightness = EEPROM.read(3 + 11); // (2-10 байт - резерв)
      Speed = EEPROM.read(3 + 12);
      Scale = EEPROM.read(3 + 13);
      palette = EEPROM.read(3 + 14);
FastLED.setBrightness(Brightness);
}}

void loop() {
  effectsTick();
  demoTick();
  bluetoothRoutine();        
}
