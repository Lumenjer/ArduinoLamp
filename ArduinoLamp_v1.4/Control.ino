
void changePower() {
  if (ONflag) {
    effectsTick();
    for (int i = 0; i < Brightness[currentMode]; i += 8) {
      FastLED.setBrightness(i);
      delay(1);
      FastLED.show();
    }
    FastLED.setBrightness(Brightness[currentMode]);
    delay(2);
    FastLED.show();
  } else {
    effectsTick();
    for (int i = Brightness[currentMode]; i > 8; i -= 8) {
      FastLED.setBrightness(i);
      delay(1);
      FastLED.show();
    }
    memset8( leds, 0, NUM_LEDS * 3);
    delay(2);
    FastLED.show();
  }
#if defined(MOSFET_PIN) && defined(MOSFET_LEVEL)          // установка сигнала в пин, управляющий MOSFET транзистором, соответственно состоянию вкл/выкл матрицы
  digitalWrite(MOSFET_PIN, ONflag ? MOSFET_LEVEL : !MOSFET_LEVEL);
#endif
}


void ONOFF() {
  if (ONflag) {
    ONflag = false;
    changePower();
  } else {
    ONflag = true;
    changePower();
  }
}

void NextEffect() {
  if (++currentMode >= MODE_AMOUNT) currentMode = 0;
  FastLED.setBrightness(Brightness[currentMode]);
  loadingFlag = true;
  memset8( leds, 0, NUM_LEDS * 3);
  delay(1);
}

void PrevEffect() {
  if (--currentMode < 0) currentMode = MODE_AMOUNT - 1;
  FastLED.setBrightness(Brightness[currentMode]);
  loadingFlag = true;
  memset8( leds, 0, NUM_LEDS * 3);
  delay(1);
}

void GoToEffect(byte Mode) {
  currentMode = Mode;
  FastLED.setBrightness(Brightness[currentMode]);
  loadingFlag = true;
  memset8( leds, 0, NUM_LEDS * 3);
  delay(1);
}

void SaveSettings() {
  if (EEPROM.read(0) != 102) EEPROM.write(0, 102);
  if (EEPROM.read(1) != currentMode) EEPROM.write(1, currentMode);  // запоминаем текущий эфект
  for (byte x = 0; x < MODE_AMOUNT; x++) {                          // сохраняем настройки всех режимов
    if (EEPROM.read(x * 3 + 11) != Brightness[x]) EEPROM.write(x * 3 + 11, Brightness[x]);
    if (EEPROM.read(x * 3 + 12) != Speed[x]) EEPROM.write(x * 3 + 12, Speed[x]);
    if (EEPROM.read(x * 3 + 13) != Scale[x]) EEPROM.write(x * 3 + 13, Scale[x]);
  }
  ONOFF();
  delay(200);
  ONOFF();
}

void DeleteSettings() {
  if (EEPROM.read(0) != 0) EEPROM.write(0, 0);
  ONOFF();
  delay(200);
  ONOFF();
}

#define PARSE_AMOUNT 2    // максимальное количество значений в массиве, который хотим получить
#define header '$'        // стартовый символ
#define divider ' '       // разделительный символ
#define ending ';'        // завершающий символ


byte intData[PARSE_AMOUNT];     // массив численных значений после парсинга
boolean parseStarted;
boolean runningFlag;

byte parse_index;
String string_convert = "";
enum modes {NORMAL, TEXT} parseMode;

String runningText;

// ********************* ПРИНИМАЕМ ДАННЫЕ **********************
void parsing() {
  // ****************** ОБРАБОТКА *****************
  if (recievedFlag) {      // если получены данные
    recievedFlag = false;

    switch (intData[0]) {
      case 1:
        switch (intData[1]) {
          case 0:
            ONOFF();
            break;
          case 1:
            NextEffect();
            break;
          case 2:
            PrevEffect();
            break;
          case 3:
            isDemo = !isDemo;
            DemTimer = 0;
            break;
          case 4: loadingFlag = true; runningFlag = true; break;
          case 5: loadingFlag = true; runningFlag = false; break;
          case 6:  SaveSettings();
            break;
        }
      case 2:
        loadingFlag = true;
        Brightness[currentMode] = map(intData[1], 1, 255, 10, BRIGHTNESS);
        FastLED.setBrightness(Brightness[currentMode]);
        break;
      case 3:
        Speed[currentMode] = intData[1];
        loadingFlag = true;
        break;
      case 4:
        loadingFlag = true;
        Scale[currentMode] = intData[1];

        break;
      case 5:
        loadingFlag = true;
        palette = map(intData[1], 1, 255, 1, 10);
        break;
      case 6:
        loadingFlag = true;
        break;
      case 7:
        loadingFlag = true;
        //DEMOTIME = intData[1];
        break;
    }
  }

  // ****************** ПАРСИНГ *****************
  if (Serial.available() > 0) {
    char incomingByte;
    if (parseMode == TEXT) {     // если нужно принять строку
      runningText = Serial.readString();  // принимаем всю
      incomingByte = ending;              // сразу завершаем парс
      parseMode = NORMAL;
    } else {
      incomingByte = Serial.read();        // обязательно ЧИТАЕМ входящий символ
    }
    if (parseStarted) {                         // если приняли начальный символ (парсинг разрешён)
      if (incomingByte != divider && incomingByte != ending) {   // если это не пробел И не конец
        string_convert += incomingByte;       // складываем в строку
      } else {                                // если это пробел или ; конец пакета
        if (parse_index == 0) {
          byte thisMode = string_convert.toInt();
          if (thisMode == 6) parseMode = TEXT;
          else parseMode = NORMAL;
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
void BTTick() {
  parsing();                           // принимаем данные

  if (!parseStarted) {                // на время принятия данных матрицу не обновляем!
  }
}

//------------------------------------------------------------------------------
#include <GyverButtonOld.h>

GButton touch(BUTTON_PIN, BUT_PULL , NORM_OPEN);

boolean inDirection;

void ButtonTick() {
  touch.tick();

  if (touch.isSingle()) {
    ONOFF();
  }
  if (!ONflag) {
    if (touch.isDouble()) {
      ONflag = true;
      changePower();
      isDemo = true;
    }
  }
  if (ONflag) {
    if (touch.isDouble()) {
      NextEffect();
    }
    if (touch.isTriple()) {
      PrevEffect();
    }
    if (touch.hasClicks())
      if (touch.getClicks() == 5) {      // если было пятикратное нажатие на кнопку, то производим сохранение параметров
        SaveSettings();
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

// ----- IR REMOTE ----- from MusicColor v2 by AlexGyver
//--------------------------------------------------------------------------------------
#include <IRLremote.h>
CHashIR IRLremote;
uint32_t IRdata;
boolean ir_flag = false;

void debugPrint() {
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

void IRTick() {
  if (IRLremote.available())  {
    auto data = IRLremote.read();
    IRdata = data.command;
    ir_flag = true;
  }
  if (ir_flag) { // если данные пришли
    switch (IRdata) {
      // режимы
      case IR_ON:
        changePower();
        break;
      case IR_NEXT:
        NextEffect();
        break;
      case IR_PREVIOUS:
        PrevEffect();
        break;
      case IR_DEMO:
        isDemo = !isDemo;
        break;
      case IR_PALETTE:
        if (palette >= 10) palette = 0;
        else palette ++;
        loadingFlag = true;
        break;
      case IR_SAVE:
        SaveSettings();
        break;
      case IR_ERASE: DeleteSettings();
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

void controlTick() {
#ifdef USE_BUTTON
  ButtonTick();
#endif
#ifdef USE_IR
  IRTick();
#endif
#ifdef USE_BT
  BTTick();
#endif
}

void SetUP() {
#ifdef USE_BUTTON
  touch.setStepTimeout(100);
  touch.setClickTimeout(500);
#endif
#ifdef USE_IR
  IRLremote.begin(IR_PIN);
#endif
#ifdef USE_BT
  Serial.begin(9600);
  Serial.println();
#endif
}
