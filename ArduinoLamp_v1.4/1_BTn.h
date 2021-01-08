//1 кнопка
//#define CONTROL_PIN 2 //пин кнопки
#define BUTTON_TYPE 1  //0-Сенсорная, 1-Тактовая
//------------------------------------------------------------------------------
#include <GyverButtonOld.h>

#if (BUTTON_TYPE == 0)
GButton touch(CONTROL_PIN, LOW_PULL, NORM_OPEN);
#else
GButton touch(CONTROL_PIN, HIGH_PULL, NORM_OPEN);
#endif
boolean inDirection;

void controlTick() {
  touch.tick();
  if (!ONflag) {
  if (touch.isSingle()) {
    {
        ONflag = true;
        changePower();
        isDemo = false;
      }}

    if (touch.isDouble()) {
      ONflag = true;
      changePower();
      isDemo = true;
    }
  }
  if (ONflag) {                 // если включено
    if (touch.isSingle()) {
        ONflag = false;
        changePower();
    }
    if (touch.isDouble()) {
      if (++currentMode >= MODE_AMOUNT) currentMode = 0;
      FastLED.setBrightness(modes[currentMode].Brightness);
      loadingFlag = true;
      //settChanged = true;
      memset8( leds, 0, NUM_LEDS * 3);
      delay(1);
    }
    if (touch.isTriple()) {
      if (--currentMode < 0) currentMode = MODE_AMOUNT - 1;
      FastLED.setBrightness(modes[currentMode].Brightness);
      loadingFlag = true;
      //settChanged = true;
      memset8( leds, 0, NUM_LEDS * 3);
      delay(1);
    }
    if (touch.hasClicks())
      if (touch.getClicks() == 5) {      // если было пятикратное нажатие на кнопку, то производим сохранение параметров
        if (EEPROM.read(0) != 102) EEPROM.write(0, 102);
        if (EEPROM.read(1) != currentMode) EEPROM.write(1, currentMode);  // запоминаем текущий эфект
        for (byte x = 0; x < MODE_AMOUNT; x++) {                          // сохраняем настройки всех режимов
          if (EEPROM.read(x * 3 + 11) != modes[x].Brightness) EEPROM.write(x * 3 + 11, modes[x].Brightness);
          if (EEPROM.read(x * 3 + 12) != modes[x].Speed) EEPROM.write(x * 3 + 12, modes[x].Speed);
          if (EEPROM.read(x * 3 + 13) != modes[x].Scale) EEPROM.write(x * 3 + 13, modes[x].Scale);
        }
        // индикация сохранения
        ONflag = false;
        changePower();
        delay(200);
        ONflag = true;
        changePower();
      }
      else if (touch.getClicks() == 4) {     // если было четырёхкратное нажатие на кнопку, то переключаем демо
        if (palette >= 10) palette = 0;
        else palette ++;
        loadingFlag = true;
      }


    if (touch.isHolded()) {  // изменение яркости при удержании кнопки
      inDirection = !inDirection;
      numHold = 1;
    }

    if (touch.isHolded2()) {  // изменение скорости "speed" при двойном нажатии и удержании кнопки
      inDirection = !inDirection;
      numHold = 2;
    }

    if (touch.isHolded3()) {  // изменение масштаба "scale" при тройном нажатии и удержании кнопки
      inDirection = !inDirection;
      numHold = 3;
    }

    if (touch.isStep()) {
      if (numHold != 0) numHold_Timer = millis(); loadingFlag = true;
      switch (numHold) {
        case 1:
          modes[currentMode].Brightness = constrain(modes[currentMode].Brightness + (modes[currentMode].Brightness / 25 + 1) * (inDirection * 2 - 1), 1 , BRIGHTNESS);
          break;
        case 2:
          modes[currentMode].Speed = constrain(modes[currentMode].Speed + (modes[currentMode].Speed / 25 + 1) * (inDirection * 2 - 1), 1 , 255);
          break;

        case 3:
          modes[currentMode].Scale = constrain(modes[currentMode].Scale + (modes[currentMode].Scale / 25 + 1) * (inDirection * 2 - 1), 1 , 255);
          break;
      }
    }
    if ((millis() - numHold_Timer) > numHold_Time) {
      numHold = 0;
      numHold_Timer = millis();
    }
    FastLED.setBrightness(modes[currentMode].Brightness);
  }
}

void SetUP(){
  touch.setStepTimeout(100);
  touch.setClickTimeout(500);
  }
