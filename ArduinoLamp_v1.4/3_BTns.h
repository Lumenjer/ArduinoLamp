//3 кнопки
#define BUTTON_TYPE 1  //0-Сенсорные, 1-Тактовые
//#define CONTROL_PIN 2 //пин первой кнопки
#define CONTROL_PIN2 3 //пин второй кнопки
#define CONTROL_PIN3 5 //пин третий кнопки
//------------------------------------------------------------------------------
#include <GyverButtonOld.h>

#if (BUTTON_TYPE == 0)
GButton touch(CONTROL_PIN, LOW_PULL, NORM_OPEN);
GButton touch2(CONTROL_PIN2, LOW_PULL, NORM_OPEN);
GButton touch3(CONTROL_PIN3, LOW_PULL, NORM_OPEN);
#else
GButton touch(CONTROL_PIN, HIGH_PULL, NORM_OPEN);
GButton touch2(CONTROL_PIN2, HIGH_PULL, NORM_OPEN);
GButton touch3(CONTROL_PIN3, HIGH_PULL, NORM_OPEN);
#endif

boolean inDirection;
void controlTick() {
  touch.tick();//Вперед/Вверх
  touch2.tick();//Главная
  touch3.tick();//Назад/Вниз

  if (touch2.isSingle()) {
    {
      if (ONflag) {
        ONflag = false;
        changePower();
      } else {
        ONflag = true;
        changePower();
      }
    }
  }

  if (ONflag) {                 // если включено
    if (touch.isSingle()) {
      if (++currentMode >= MODE_AMOUNT) currentMode = 0;
      FastLED.setBrightness(Brightness[currentMode]);
      loadingFlag = true;
      //settChanged = true;
      memset8( leds, 0, NUM_LEDS * 3);
      delay(1);
    }
    if (touch3.isSingle()) {
      if (--currentMode < 0) currentMode = MODE_AMOUNT - 1;
      FastLED.setBrightness(Brightness[currentMode]);
      loadingFlag = true;
      //settChanged = true;
      memset8( leds, 0, NUM_LEDS * 3);
      delay(1);
    }
    if (touch.hasClicks()) {
      if (touch.getClicks() == 5) {     // если было пятикратное нажатие на кнопку, то производим сохранение параметров
        if (EEPROM.read(0) != 102) EEPROM.write(0, 102);
        if (EEPROM.read(1) != currentMode) EEPROM.write(1, currentMode);  // запоминаем текущий эфект
        for (byte x = 0; x < MODE_AMOUNT; x++) {                          // сохраняем настройки всех режимов
          if (EEPROM.read(x * 3 + 11) != Brightness[x]) EEPROM.write(x * 3 + 11, Brightness[x]);
          if (EEPROM.read(x * 3 + 12) != Speed[x]) EEPROM.write(x * 3 + 12, Speed[x]);
          if (EEPROM.read(x * 3 + 13) != Scale[x]) EEPROM.write(x * 3 + 13, Scale[x]);
        }
        // индикация сохранения
        ONflag = false;
        changePower();
        delay(200);
        ONflag = true;
        changePower();
      }
    }
    if (touch2.isTriple()) {     // если было четырёхкратное нажатие на кнопку, то переключаем демо
      isDemo = !isDemo;
      DemTimer = 0UL;
    }
    if (touch2.isDouble()) {
      if (palette >= 10) palette = 0;
      else palette ++;
      loadingFlag = true;
    }

    if (touch.isHolded()) {  // изменение яркости при удержании кнопки
      inDirection = true;
      numHold = 1;
    }
    if (touch3.isHolded()) {  // изменение яркости при удержании кнопки
      inDirection = false;
      numHold = 1;
    }

    if (touch.isHolded2()) {  // изменение скорости "speed" при двойном нажатии и удержании кнопки
      inDirection = true;
      numHold = 2;
    }
    if (touch3.isHolded2()) {  // изменение яркости при удержании кнопки
      inDirection = false;
      numHold = 2;
    }
    if (touch.isHolded3()) {  // изменение масштаба "scale" при тройном нажатии и удержании кнопки
      inDirection = true;
      numHold = 3;
    }
    if (touch3.isHolded3()) {  // изменение яркости при удержании кнопки
      inDirection = false;
      numHold = 3;
    }

    if (touch.isStep() or touch2.isStep()) {
      if (numHold != 0) numHold_Timer = millis(); loadingFlag = true;
      switch (numHold) {
        case 1:
          Brightness[currentMode] = constrain(Brightness[currentMode] + (Brightness[currentMode] / 25 + 1) * (inDirection * 2 - 1), 1 , BRIGHTNESS);
          break;
        case 2:
          Speed[currentMode] = constrain(Speed[currentMode] + (Speed[currentMode] / 25 + 1) * (inDirection * 2 - 1), 1 , 255);
          break;

        case 3:
          Scale[currentMode] = constrain(Scale[currentMode] + (Scale[currentMode] / 25 + 1) * (inDirection * 2 - 1), 1 , 255);
          break;
      }
    }
    if ((millis() - numHold_Timer) > numHold_Time) {
      numHold = 0;
      numHold_Timer = millis();
    }
    FastLED.setBrightness(Brightness[currentMode]);
  }
}

void SetUP(){
  touch.setStepTimeout(100);
  touch.setClickTimeout(500);
  touch2.setStepTimeout(100);
  touch2.setClickTimeout(500);
  touch3.setStepTimeout(100);
  touch3.setClickTimeout(500);
  }
