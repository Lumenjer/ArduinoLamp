boolean inDirection;

void buttonTick() {
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
      if (touch.getClicks() == 5) {     // если было пятикратное нажатие на кнопку, то производим сохранение параметров
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
          modes[currentMode].Brightness = constrain(modes[currentMode].Brightness + (modes[currentMode].Brightness / 25 + 1) * (inDirection * 2 - 1), 1 , 255);
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
