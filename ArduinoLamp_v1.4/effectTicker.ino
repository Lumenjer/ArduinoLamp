void effectsTick() {
  {
    if (!runningFlag && ONflag && millis() - effTimer >= ((currentMode < 3 || currentMode > 8) ? 256 - Speed : 50) ) {
      effTimer = millis(); switch (currentMode) {
        //|номер   |название функции эффекта     |тоже надо|
        case 0 : sparklesRoutine();             break;
        case 1 : RainbowRoutine();              break;
        case 2 : fireRoutine();                 break;
        //case 3 : LavaLampRoutine();             break;
        case 3 : Noise3D();                     break;
        case 4 : TLandRoutine();                break;
        case 5 : whiteLampRoutine();            break;
        case 6 : colorRoutine();                break;
        case 7 : colorsRoutine();               break;
        case 8 : ballRoutine();                 break;
        case 9 : ballsRoutine();                break;
        case 10: matrixRoutine();               break;
        case 11: RainRoutine();                 break;
        case 12: stormRoutine();                break;
        case 13: lightersRoutine();             break;
      }
    }
    FastLED.show();
  }
}


void changePower() {    // плавное включение/выключение
  if (ONflag) {
    effectsTick();
    for (int i = 0; i < Brightness; i += 8) {
      FastLED.setBrightness(i);
      delay(1);
      FastLED.show();
    }
    FastLED.setBrightness(Brightness);
    delay(2);
    FastLED.show();
  } else {
    effectsTick();
    for (int i = Brightness; i > 8; i -= 8) {
      FastLED.setBrightness(i);
      delay(1);
      FastLED.show();
    }
    memset8( leds, 0, NUM_LEDS * 3);
    delay(2);
    FastLED.show();
  }
}

void demoTick() {
  if (isDemo && ONflag && millis() >= DemTimer) {
    if (RANDOM_DEMO == 1)
      currentMode = random8(MODE_AMOUNT); // если нужен следующий случайный эффект
    if (RANDOM_DEMO == 0)
      currentMode = currentMode + 1U < MODE_AMOUNT ? currentMode + 1U : 0U; // если нужен следующий по списку эффект
    if (RANDOM_DEMO == 2)
      currentMode = random8(MODE_AMOUNT); Speed = random8(); Scale = random8();
    memset8( leds, 0, NUM_LEDS * 3);
    DemTimer = millis() + DEMOTIME*1000;
    loadingFlag = true;
  }
}
