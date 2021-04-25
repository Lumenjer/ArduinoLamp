// ----- ПУЛЬТ ИК -----
#define IR_ON 0x1AED14B5              // код пульта для Включения/Выключения         
#define IR_NEXT 0xA21606B5            // код пульта для Следующего Эффекта
#define IR_PREVIOUS 0xA52ACBB5        // код пульта для Предыдущего Эффекта
#define IR_SAVE 0x8D1B67B5            // код пульта для сохранения настроек эффектов 
#define IR_DEMO 0xB50F59B5            // код пульта для включения или выключения демо
#define IR_BRIGHT_DOWN 0xB1AE17B5     // код пульта для понижения Яркости
#define IR_BRIGHT_UP 0x78BBC7B5       // код пульта для повышения Яркости
#define IR_SPEED_DOWN 0xC2F8F2B5      // код пульта для понижения Скорости
#define IR_SPEED_UP 0xBFE42DB5        // код пульта для повышения Скорости
#define IR_SCALE_DOWN 0x2F4CB4B5      // код пульта для понижения Масштаба
#define IR_SCALE_UP 0xD7F391B5        // код пульта для повышения Масштаба
#define IR_PALETTE 0x21631BB5         // код пульта для Палитр
#define IR_ERASE 0x1E4E56B5           // код пульта для установки настроек по умолчанию
#define BUTTON_TYPE 1  //0-Сенсорные, 1-Тактовые
//#define CONTROL_PIN 2 //пин первой кнопки/Пульта
#define CONTROL_PIN2 3 //пин второй кнопки
#define CONTROL_PIN3 5 //пин третий кнопки
#define CONTROL_PIN4 6 //пин третий кнопки

#if (CONTROL_TYPE == 0)
#define PARSE_AMOUNT 2    // максимальное количество значений в массиве, который хотим получить
#define header '$'        // стартовый символ
#define divider ' '       // разделительный символ
#define ending ';'        // завершающий символ


byte intData[PARSE_AMOUNT];     // массив численных значений после парсинга
boolean parseStarted;
boolean runningFlag;

byte parse_index;
String string_convert = "";
enum modes {NORMAL} parseMode;


// ********************* ПРИНИМАЕМ ДАННЫЕ **********************
void parsing() {
  // ****************** ОБРАБОТКА *****************
  if (recievedFlag) {      // если получены данные
    recievedFlag = false;

    switch (intData[0]) {
      case 1:
        switch (intData[1]) {
          case 0:
            if (ONflag) {
              ONflag = false;
              changePower();
            } else {
              ONflag = true;
              changePower();
            }
            break;
          case 1:
            if (++currentMode >= MODE_AMOUNT) currentMode = 0;
            FastLED.setBrightness(Brightness[currentMode]);
            loadingFlag = true;
            memset8( leds, 0, NUM_LEDS * 3);
            delay(1);
            break;
          case 2:
            if (--currentMode < 0) currentMode = MODE_AMOUNT - 1;
            FastLED.setBrightness(Brightness[currentMode]);
            loadingFlag = true;
            memset8( leds, 0, NUM_LEDS * 3);
            delay(1);
            break;
          case 3:
            isDemo = !isDemo;
            DemTimer = 0;
            break;
          case 4: runningFlag = true; break;
          case 5: runningFlag = false; break;
          case 6:
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
            break;
        }
        break;
      case 2:
        Brightness[currentMode] = map(intData[1], 1, 255, 10, BRIGHTNESS);
        FastLED.setBrightness(Brightness[currentMode]);
        loadingFlag = true;
        break;
      case 3:
        Speed[currentMode] = intData[1];
        loadingFlag = true;
        break;
      case 4:
        Scale[currentMode] = intData[1];
        loadingFlag = true;
        break;
      case 5:
        palette = map(intData[1], 1, 255, 1, 10);
        loadingFlag = true;
        break;
      case 6:
        loadingFlag = true;
        // строка принимается в переменную runningText
        break;
    }
    //lastMode = intData[0];  // запомнить предыдущий режим
  }

  // ****************** ПАРСИНГ *****************
  if (Serial.available() > 0) {
    char incomingByte;
    incomingByte = Serial.read();        // обязательно ЧИТАЕМ входящий символ
    if (parseStarted) {                         // если приняли начальный символ (парсинг разрешён)
      if (incomingByte != divider && incomingByte != ending) {   // если это не пробел И не конец
        string_convert += incomingByte;       // складываем в строку
      } else {                                // если это пробел или ; конец пакета
        if (parse_index == 0) {
          byte thisMode = string_convert.toInt();
          parseMode = NORMAL;
          //if (thisMode != 7 || thisMode != 0) runningFlag = false;
        }
        if (parse_index == 1) {       // для второго (с нуля) символа в посылке
          if (parseMode == NORMAL) intData[parse_index] = string_convert.toInt();             // преобразуем строку в int и кладём в массив}
        } else {
          intData[parse_index] = string_convert.toInt();  // преобразуем строку в int и кладём в массив
        }
        string_convert = "";                  // очищаем строку
        parse_index++;                              // переходим к парсингу следующего элемента массива
      }
    }
    if (incomingByte == header) {             // если это $
      parseStarted = true;                      // поднимаем флаг, что можно парсить
      parse_index = 0;                              // сбрасываем индекс
      string_convert = "";                    // очищаем строку
    }
    if (incomingByte == ending) {             // если таки приняли ; - конец парсинга
      parseMode == NORMAL;
      parseStarted = false;                     // сброс
      recievedFlag = true;                    // флаг на принятие
    }
  }
}

void SetUP() {
  Serial.begin(9600);
  Serial.println();
}

void controlTick() {
  parsing();                           // принимаем данные

  if (!parseStarted) {                // на время принятия данных матрицу не обновляем!
  }
}
//1 кнопка
#elif(CONTROL_TYPE == 1)
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
      }
    }

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
      FastLED.setBrightness(Brightness[currentMode]);
      loadingFlag = true;
      //settChanged = true;
      memset8( leds, 0, NUM_LEDS * 3);
      delay(1);
    }
    if (touch.isTriple()) {
      if (--currentMode < 0) currentMode = MODE_AMOUNT - 1;
      FastLED.setBrightness(Brightness[currentMode]);
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

void SetUP() {
  touch.setStepTimeout(100);
  touch.setClickTimeout(500);
}
#elif(CONTROL_TYPE == 2)
//------------------------------------------------------------------------------
#include <GyverButtonOld.h>

#if (BUTTON_TYPE == 0)
GButton touch(CONTROL_PIN, LOW_PULL, NORM_OPEN);
GButton touch2(CONTROL_PIN2, LOW_PULL, NORM_OPEN);
#else
GButton touch(CONTROL_PIN, HIGH_PULL, NORM_OPEN);
GButton touch2(CONTROL_PIN2, HIGH_PULL, NORM_OPEN);
#endif

boolean inDirection;

void controlTick() {
  touch.tick();
  touch2.tick();

  if (touch.isSingle()) {
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
    if (touch.isDouble()) {
      if (++currentMode >= MODE_AMOUNT) currentMode = 0;
      FastLED.setBrightness(Brightness[currentMode]);
      loadingFlag = true;
      //settChanged = true;
      memset8( leds, 0, NUM_LEDS * 3);
      delay(1);
    }
    if (touch2.isDouble()) {
      if (--currentMode < 0) currentMode = MODE_AMOUNT - 1;
      FastLED.setBrightness(Brightness[currentMode]);
      loadingFlag = true;
      //settChanged = true;
      memset8( leds, 0, NUM_LEDS * 3);
      delay(1);
    }
    if (touch.isTriple()) {      // если было пятикратное нажатие на кнопку, то производим сохранение параметров
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
    if (touch2.isTriple()) {     // если было четырёхкратное нажатие на кнопку, то переключаем демо
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

    if (touch.isHolded2()) {  // изменение скорости "speed" при двойном нажатии и удержании кнопки
      inDirection = true;
      numHold = 2;
    }
    if (touch2.isHolded2()) {  // изменение яркости при удержании кнопки
      inDirection = false;
      numHold = 2;
    }
    if (touch.isHolded3()) {  // изменение масштаба "scale" при тройном нажатии и удержании кнопки
      inDirection = true;
      numHold = 3;
    }
    if (touch2.isHolded3()) {  // изменение яркости при удержании кнопки
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
void SetUP() {
  touch.setStepTimeout(100);
  touch.setClickTimeout(500);
  touch2.setStepTimeout(100);
  touch2.setClickTimeout(500);

}
#elif(CONTROL_TYPE == 3)
//3 кнопки
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

void SetUP() {
  touch.setStepTimeout(100);
  touch.setClickTimeout(500);
  touch2.setStepTimeout(100);
  touch2.setClickTimeout(500);
  touch3.setStepTimeout(100);
  touch3.setClickTimeout(500);
}
#elif(CONTROL_TYPE == 4)
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
      if (touch.getClicks() == 2) {     // если было пятикратное нажатие на кнопку, то производим сохранение параметров
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

    if (touch.isStep() or touch2.isStep() or touch3.isStep() or touch3.isStep()) {
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

void SetUP() {
  touch.setStepTimeout(100);
  touch.setClickTimeout(500);
  touch2.setStepTimeout(100);
  touch2.setClickTimeout(500);
  touch3.setStepTimeout(100);
  touch3.setClickTimeout(500);
  touch4.setStepTimeout(100);
  touch4.setClickTimeout(500)
}
#elif(CONTROL_TYPE == 5)
// ----- IR REMOTE / ИК ПУЛЬТ ----- from MusicColor v2 by AlexGyver
//--------------------------------------------------------------------------------------
#include <IRLremote.h>
CHashIR IRLremote;
uint32_t IRdata;
boolean ir_flag = false;

bool inDirection; void debugPrint() {
#ifdef DEBUG
  Serial.print(F(" brightness:"));
  Serial.print(Brightness[currentMode]);
  Serial.print(F(" speed:"));
  Serial.print(Speed[currentMode]);
  Serial.print(F(" scale:"));
  Serial.print(Scale[currentMode]);
  Serial.print(F("mode:"));
  Serial.print(currentMode);
#else
  return;
#endif
}

void controlTick() {
  if (IRLremote.available())  {
    auto data = IRLremote.read();
    IRdata = data.command;
    ir_flag = true;
  }
  if (ir_flag) { // если данные пришли
    switch (IRdata) {
      // режимы
      case IR_ON:   if (ONflag) {
          ONflag = false;
          changePower();
        } else {
          ONflag = true;
          changePower();
        }
        break;
      case IR_NEXT:
        if (++currentMode >= MODE_AMOUNT) currentMode = 0;
        FastLED.setBrightness(Brightness[currentMode]);
        loadingFlag = true;
        //settChanged = true;
        memset8( leds, 0, NUM_LEDS * 3);
        debugPrint();
        delay(1);
        break;
      case IR_PREVIOUS:
        if (--currentMode < 0) currentMode = MODE_AMOUNT - 1;
        FastLED.setBrightness(Brightness[currentMode]);
        loadingFlag = true;
        //settChanged = true;
        memset8( leds, 0, NUM_LEDS * 3);
        debugPrint();
        delay(1);
        break;
      case IR_DEMO:
        isDemo = !isDemo;

        break;
      case IR_PALETTE:
        if (palette >= 10) palette = 0;
        else palette ++;
        loadingFlag = true;
        break;
      case IR_SAVE:   if (EEPROM.read(0) != 102) EEPROM.write(0, 102);
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
        break;
      case IR_ERASE:   if (EEPROM.read(0) == 102) EEPROM.write(0, 0);
        // индикация сохранения
        ONflag = false;
        changePower();
        delay(200);
        ONflag = true;
        changePower();
        break;
      case IR_BRIGHT_UP:  numHold = 1;
        inDirection = true;
        break;
      case IR_BRIGHT_DOWN: numHold = 1;
        inDirection = false;
        break;
      case IR_SPEED_UP: numHold = 2;
        inDirection = true;
        break;
      case IR_SPEED_DOWN: numHold = 2;
        inDirection = false;
        break;
      case IR_SCALE_UP: numHold = 3;
        inDirection = true;
        break;
      case IR_SCALE_DOWN: numHold = 3;
        inDirection = false;
        break;
    }
    ir_flag = false;

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
void SetUP() {
  IRLremote.begin(CONTROL_PIN);
}
#elif(CONTROL_TYPE == 6)
//Кнопка с ИК пультом
// ---- Кнопка ----
// -----------------------
#include <GyverButtonOld.h>
#include <IRLremote.h>
// ------- КНОПКА---------
#if (BUTTON_TYPE == 0)
GButton touch(СONTROL_PIN2, LOW_PULL, NORM_OPEN);
#else
GButton touch(CONTROL_PIN2, HIGH_PULL, NORM_OPEN);
#endif
boolean inDirection;
CHashIR IRLremote;
uint32_t IRdata;
boolean ir_flag = false;

void controlTick() {
  if (IRLremote.available())  {
    auto data = IRLremote.read();
    IRdata = data.command;
    ir_flag = true;
  }
  if (ir_flag) { // если данные пришли
    switch (IRdata) {
      // режимы
      case IR_ON:
        if (ONflag) {
          ONflag = false;
          changePower();
        } else {
          ONflag = true;
          changePower();
        }
        break;
      case IR_NEXT:
        if (++currentMode >= MODE_AMOUNT) currentMode = 0;
        FastLED.setBrightness(Brightness[currentMode]);
        loadingFlag = true;
        //settChanged = true;
        memset8( leds, 0, NUM_LEDS * 3);
        delay(1);
        break;
      case IR_PREVIOUS:
        if (--currentMode < 0) currentMode = MODE_AMOUNT - 1;
        FastLED.setBrightness(Brightness[currentMode]);
        loadingFlag = true;
        //settChanged = true;
        memset8( leds, 0, NUM_LEDS * 3);
        delay(1);
        break;
      case IR_DEMO:
        isDemo = !isDemo;

        break;
      case IR_PALETTE:
        if (palette >= 10) palette = 0;
        else palette ++;
        loadingFlag = true;
        break;
      case IR_SAVE:   if (EEPROM.read(0) != 102) EEPROM.write(0, 102);
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
        break;
      case IR_ERASE:   if (EEPROM.read(0) == 102) EEPROM.write(0, 0);
        // индикация сохранения
        ONflag = false;
        changePower();
        delay(200);
        ONflag = true;
        changePower();
        break;
      case IR_BRIGHT_UP:  numHold = 1;
        inDirection = true;
        break;
      case IR_BRIGHT_DOWN: numHold = 1;
        inDirection = false;
        break;
      case IR_SPEED_UP: numHold = 2;
        inDirection = true;
        break;
      case IR_SPEED_DOWN: numHold = 2;
        inDirection = false;
        break;
      case IR_SCALE_UP: numHold = 3;
        inDirection = true;
        break;
      case IR_SCALE_DOWN: numHold = 3;
        inDirection = false;
        break;
    }
    ir_flag = false;

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


  touch.tick();
  if (!ONflag) {
    if (touch.isSingle()) {
      {
        ONflag = true;
        changePower();
        isDemo = false;
      }
    }

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
      FastLED.setBrightness(Brightness[currentMode]);
      loadingFlag = true;
      //settChanged = true;
      memset8( leds, 0, NUM_LEDS * 3);
      delay(1);
    }
    if (touch.isTriple()) {
      if (--currentMode < 0) currentMode = MODE_AMOUNT - 1;
      FastLED.setBrightness(Brightness[currentMode]);
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
// ------- ПУЛЬТ---------
void SetUP() {
  touch.setStepTimeout(100);
  touch.setClickTimeout(500);
  IRLremote.begin(CONTROL_PIN);
}
#endif
