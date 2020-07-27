/*
  Скетч к проекту "Многофункциональный RGB светильник"
  Страница проекта (схемы, описания): https://alexgyver.ru/GyverLamp/
  Нравится, как написан код? Поддержи автора! https://alexgyver.ru/support_alex/
  Автор: AlexGyver, AlexGyver Technologies, 2019(Портировал на Ардуино Norvol(+ эффекты stepko365) 
  https://AlexGyver.ru/
*/
// ============= НАСТРОЙКИ =============

// ---------- МАТРИЦА ---------
#define BRIGHTNESS 10         // стандартная маскимальная яркость (0-255)
#define CURRENT_LIMIT 1000    // лимит по току в миллиамперах, автоматически управляет яркостью (пожалей свой блок питания!) 0 - выключить лимит

#define WIDTH 8              // ширина матрицы
#define HEIGHT 8             // высота матрицы

#define COLOR_ORDER GRB       // порядок цветов на ленте. Если цвет отображается некорректно - меняйте. Начать можно с RGB

#define MATRIX_TYPE 1         // тип матрицы: 0 - зигзаг, 1 - параллельная
#define CONNECTION_ANGLE 1    // угол подключения: 0 - левый нижний, 1 - левый верхний, 2 - правый верхний, 3 - правый нижний
#define STRIP_DIRECTION 0     // направление ленты из угла: 0 - вправо, 1 - вверх, 2 - влево, 3 - вниз
// при неправильной настройке матрицы вы получите предупреждение "Wrong matrix parameters! Set to default"
// шпаргалка по настройке матрицы здесь! https://alexgyver.ru/matrix_guide/

#define numHold_Time (1*1000)     // время отображения индикатора уровня яркости/скорости/масштаба

// ============= ДЛЯ РАЗРАБОТЧИКОВ =============
#define LED_PIN 6             // пин ленты
#define BTN_PIN 2
#define MODE_AMOUNT 33        //кол-во эффектов
#define NUM_LEDS WIDTH * HEIGHT
#define SEGMENTS 1            // диодов в одном "пикселе" (для создания матрицы из кусков ленты)

// ---------------- БИБЛИОТЕКИ -----------------
#include <EEPROM.h>
#include <FastLED.h>
#include <GyverButton.h>
#include "timerMinim.h"
#include "bitmap2.h"
// ------------------- ТИПЫ --------------------
CRGB leds[NUM_LEDS];
GButton touch(BTN_PIN, HIGH_PULL, NORM_OPEN); //если сенсорна кнопка LOW_PULL
// ----------------- ПЕРЕМЕННЫЕ ------------------
static const byte maxDim = max(WIDTH, HEIGHT);
struct { byte Brightness = 10; byte Speed = 30; byte Scale = 10; } modes[MODE_AMOUNT]; //настройки эффекта по умолчанию
int8_t currentMode = 10;
boolean loadingFlag = true;
boolean ONflag = true;
byte numHold;
unsigned long numHold_Timer = 0;
unsigned char matrixValue[8][16];


void setup() {
  // ЛЕНТА
  FastLED.addLeds<WS2812B, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS)/*.setCorrection( TypicalLEDStrip )*/;
  FastLED.setBrightness(BRIGHTNESS);
  if (CURRENT_LIMIT > 0) FastLED.setMaxPowerInVoltsAndMilliamps(5, CURRENT_LIMIT);
  FastLED.clear();
  FastLED.show();

  touch.setStepTimeout(100);
  touch.setClickTimeout(500);

  //Serial.begin(9600);
  //Serial.println();

  if (EEPROM.read(0) == 102) {                    // если было сохранение настроек, то восстанавливаем их (с)НР
    currentMode = EEPROM.read(1);
    for (byte x = 0; x < MODE_AMOUNT; x++) {
      modes[x].Brightness = EEPROM.read(x * 3 + 11); // (2-10 байт - резерв)
      modes[x].Speed = EEPROM.read(x * 3 + 12);
      modes[x].Scale = EEPROM.read(x * 3 + 13);
    }

  }
}

void loop() {
  effectsTick();
  buttonTick();
}
