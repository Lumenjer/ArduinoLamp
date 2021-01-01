// вкладка работы с bluetooth


#define PARSE_AMOUNT 2    // максимальное количество значений в массиве, который хотим получить
#define header '$'        // стартовый символ
#define divider ' '       // разделительный символ
#define ending ';'        // завершающий символ


byte intData[PARSE_AMOUNT];     // массив численных значений после парсинга
boolean recievedFlag;
boolean parseStarted;
boolean runningFlag;





byte parse_index;
String string_convert = "";
enum modes {NORMAL, COLOR, TEXT} parseMode;


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
            FastLED.setBrightness(modes[currentMode].Brightness);
            loadingFlag = true;
            memset8( leds, 0, NUM_LEDS * 3);
            delay(1);
            break;
          case 2:
            if (--currentMode < 0) currentMode = MODE_AMOUNT - 1;
            FastLED.setBrightness(modes[currentMode].Brightness);
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
          if (EEPROM.read(x * 3 + 11) != modes[x].Brightness) EEPROM.write(x * 3 + 11, modes[x].Brightness);
          if (EEPROM.read(x * 3 + 12) != modes[x].Speed) EEPROM.write(x * 3 + 12, modes[x].Speed);
          if (EEPROM.read(x * 3 + 13) != modes[x].Scale) EEPROM.write(x * 3 + 13, modes[x].Scale);
            // индикация сохранения
            ONflag = false;
            changePower();
            delay(200);
            ONflag = true;
            changePower();
            break;}
        }
        break;
      case 2:
        modes[currentMode].Brightness = map(intData[1], 1, 255, 10, BRIGHTNESS);
        FastLED.setBrightness(modes[currentMode].Brightness);
        loadingFlag = true;
        break;
      case 3:
        modes[currentMode].Speed = intData[1];
        loadingFlag = true;
        break;
      case 4:
        modes[currentMode].Scale = intData[1];
        loadingFlag = true;
        break;
      case 5:
        palette = map(intData[1],1,255,1,10);
        loadingFlag = true;
        break;
    }
    //lastMode = intData[0];  // запомнить предыдущий режим
  }

  // ****************** ПАРСИНГ *****************
  if (Serial.available() > 0) {
    char incomingByte;  
    incomingByte = Serial.read();
    if (parseStarted) {                         // если приняли начальный символ (парсинг разрешён)
      if (incomingByte != divider && incomingByte != ending) {   // если это не пробел И не конец
        string_convert += incomingByte;       // складываем в строку
      } else {                                // если это пробел или ; конец пакета
        if (parse_index == 0) {
          byte thisMode = string_convert.toInt();
          if (thisMode == 0 || thisMode == 5) parseMode = COLOR;    // передача цвета (в отдельную переменную)
          else if (thisMode == 6) parseMode = TEXT;
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

void controlTick() {
  parsing();                           // принимаем данные
  if (!parseStarted) {                // на время принятия данных матрицу не обновляем!
  }
}

void SetUP(){
  Serial.begin(9600);
  Serial.println();
  }
