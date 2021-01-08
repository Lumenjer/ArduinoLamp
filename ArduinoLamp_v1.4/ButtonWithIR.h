//Кнопка с ИК пультом
// ---- Кнопка ----
#define BUTTON_TYPE 1  //0-Сенсорна, 1-Тактовая
//#define CONTROL_PIN 2 //пин ИК приемника
#define BUTTON_PIN 6 //пин кнопки
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
#define IR_1 0x1E4E56B5               // код пульта 
// -----------------------
#include <GyverButtonOld.h>
#include <IRLremote.h>
// ------- КНОПКА---------
#if (BUTTON_TYPE == 0)
GButton touch(BUTTON_PIN, LOW_PULL, NORM_OPEN);
#else
GButton touch(BUTTON_PIN, HIGH_PULL, NORM_OPEN);
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
        FastLED.setBrightness(modes[currentMode].Brightness);
        loadingFlag = true;
        //settChanged = true;
        memset8( leds, 0, NUM_LEDS * 3);
        delay(1);
        break;
      case IR_PREVIOUS:
        if (--currentMode < 0) currentMode = MODE_AMOUNT - 1;
        FastLED.setBrightness(modes[currentMode].Brightness);
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
  }
  
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


    if (touch.isStep() || ir_flag) {
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
// ------- ПУЛЬТ---------
void SetUP(){  
  touch.setStepTimeout(100);
  touch.setClickTimeout(500);
  IRLremote.begin(CONTROL_PIN);
  }
