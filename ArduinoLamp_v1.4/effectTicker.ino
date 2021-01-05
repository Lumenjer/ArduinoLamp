uint32_t effTimer;
byte ind;
void effectsTick() {
  {
    if (ONflag && millis() - effTimer >= ((currentMode < 3 || currentMode > 6) ? 256-modes[currentMode].Speed : 50) ) {
      effTimer = millis(); switch (currentMode) {
        //|номер   |название функции эффекта     |тоже надо|
        case 0 : sparklesRoutine();             break;
        case 1 : RainbowRoutine();              break;
        case 2 : FireRoutine();                 break;//Новый-Fire.. Старый fire...
        case 3 : LavaLampRoutine();             break;
        case 4 : Noise3D();                     break;
        case 5 : whiteLampRoutine();            break;
        case 6 : colorRoutine();                break;
        case 7 : colorsRoutine();               break;
        case 8 : matrixRoutine();               break;
        case 9 : RainRoutine();                 break;
        case 10: stormRoutine();                break;
        case 11: ballRoutine();                 break;
        case 12: ballsRoutine();                break;
      }
#ifdef INDICATOR
      switch (numHold) {    // индикатор уровня яркости/скорости/масштаба
        case 1:
          ind = sqrt(modes[currentMode].Brightness + 1);
          for (byte y = 0; y < HEIGHT ; y++) {
            if (ind > y) drawPixelXY(0, y, CHSV(10, 255, 255));
            else drawPixelXY(0, y,  0);
          }
          break;
        case 2:
          ind = sqrt(modes[currentMode].Speed - 1);
          for (byte y = 0; y <= HEIGHT ; y++) {
            if (ind <= y) drawPixelXY(0, 15 - y, CHSV(100, 255, 255));
            else drawPixelXY(0, 15 - y,  0);
          }
          break;
        case 3:
          ind = sqrt(modes[currentMode].Scale + 1);
          for (byte y = 0; y < HEIGHT ; y++) {
            if (ind > y) drawPixelXY(0, y, CHSV(150, 255, 255));
            else drawPixelXY(0, y,  0);
          }
          break;
#endif
      }
      #if (CONTROL_TYPE == 1 || CONTROL_TYPE == 6)
      if (!IRLremote.receiving())    // если на ИК приёмник не приходит сигнал (без этого НЕ РАБОТАЕТ!)
        FastLED.show();
      #else
      FastLED.show();
      #endif
    }
  }
}

 void demoTick(){
  if (isDemo && ONflag && millis() >= DemTimer){
    if(RANDOM_DEMO)
    currentMode = random8(MODE_AMOUNT); // если нужен следующий случайный эффект
    else 
    currentMode = currentMode + 1U < MODE_AMOUNT ? currentMode + 1U : 0U; // если нужен следующий по списку эффект
    memset8( leds, 0, NUM_LEDS * 3);
    DemTimer = millis() + DEMOTIMELIMIT;
    loadingFlag = true;}
}
