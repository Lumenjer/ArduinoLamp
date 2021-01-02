#define BUTTON_TYPE 1  //0-Сенсорные, 1-Тактовые
#define CONTROL_PIN2 3 //пин второй кнопки
#define CONTROL_PIN3 5 //пин третий кнопки
#define CONTROL_PIN4 6 //пин третий кнопки
//------------------------------------------------------------------------------
#include <GyverButtonOld.h>

#if (BUTTON_TYPE == 0)
GButton touch(CONTROL_PIN, LOW_PULL, NORM_OPEN);
GButton touch2(CONTROL_PIN2, LOW_PULL, NORM_OPEN);
GButton touch3(CONTROL_PIN3, LOW_PULL, NORM_OPEN);
GButton touch4(CONTROL_PIN4, LOW_PULL, NORM_OPEN);
#else
GButton touch(CONTROL_PIN, HIGH_PULL, NORM_OPEN);
GButton touch2(CONTROL_PIN2, HIGH_PULL, NORM_OPEN);
GButton touch3(CONTROL_PIN3, HIGH_PULL, NORM_OPEN);
GButton touch4(CONTROL_PIN4, HIGH_PULL, NORM_OPEN);
#endif

boolean inDirection;
void controlTick() {
  touch.tick();//Вверх
  touch2.tick();//Вниз
  touch3.tick();//Влево
  touch4.tick();//Вправо

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
    if (touch4.isSingle()) {
      if (++currentMode >= MODE_AMOUNT) currentMode = 0;
      FastLED.setBrightness(modes[currentMode].Brightness);
      loadingFlag = true;
      //settChanged = true;
      memset8( leds, 0, NUM_LEDS * 3);
      delay(1);
    }
    if (touch3.isSingle()) {
      if (--currentMode < 0) currentMode = MODE_AMOUNT - 1;
      FastLED.setBrightness(modes[currentMode].Brightness);
      loadingFlag = true;
      //settChanged = true;
      memset8( leds, 0, NUM_LEDS * 3);
      delay(1);
    }
    if (touch.hasClicks()) {
      if (touch.getClicks() == 2) {     // если было пятикратное нажатие на кнопку, то производим сохранение параметров
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
    }
    if (touch.isSingle()) {     
      isDemo = !isDemo;
      DemTimer = 0UL;
    }
    if (touch2.isSingle()) {
      if (palette >= 10) palette = 0;
      else palette ++;
      loadingFlag = true;
    }

    if (touch.isHolded()) {  // изменение яркости при удержании кнопки
      inDirection = true;
      numHold = 1;
    }
    if (touch2.isHolded()) {  // изменение яркости при удержании кнопки
      inDirection = false;
      numHold = 1;
    }

    if (touch4.isHolded()) {  // изменение скорости "speed" при двойном нажатии и удержании кнопки
      inDirection = true;
      numHold = 2;
    }
    if (touch3.isHolded()) {  // изменение яркости при удержании кнопки
      inDirection = false;
      numHold = 2;
    }
    if (touch4.isHolded2()) {  // изменение масштаба "scale" при тройном нажатии и удержании кнопки
      inDirection = true;
      numHold = 3;
    }
    if (touch4.isHolded2()) {  // изменение яркости при удержании кнопки
      inDirection = false;
      numHold = 3;
    }

    if (touch.isStep() or touch2.isStep()or touch3.isStep() or touch3.isStep()) {
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
  touch2.setStepTimeout(100);
  touch2.setClickTimeout(500);
  touch3.setStepTimeout(100);
  touch3.setClickTimeout(500);
  touch4.setStepTimeout(100);
  touch4.setClickTimeout(500)
  }
