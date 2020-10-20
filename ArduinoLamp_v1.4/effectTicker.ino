uint32_t effTimer;
byte ind;
void effectsTick() {
  {
    if (ONflag && millis() - effTimer >= ((currentMode < 6 || currentMode > 20) ? modes[currentMode].Speed : 50) ) {
      effTimer = millis(); switch (currentMode) {
        //|номер   |название функции эффекта     |тоже надо|
        case 0 : sparklesRoutine();             break;
        case 1 : rainbowVertical();             break;
        case 2 : rainbowHorizontal();           break;
        case 3 : rainbowDiagonalRoutine();      break;
        case 4 : Fire2020();                    break;
        case 5 : shadowsRoutine();              break;
        case 6 : madnessNoise();                break;
        case 7 : cloudNoise();                  break;
        case 8 : lavaNoise();                   break;
        case 9 : plasmaNoise();                 break;
        case 10: rainbowNoise();                break;
        case 11: rainbowStripeNoise();          break;
        case 12: zebraNoise();                  break;
        case 13: forestNoise();                 break;
        case 14: oceanNoise();                  break;
        case 15: smokeNoise();                  break;
        case 16: colorRoutine();                break;
        case 17: colorsRoutine();               break;
        case 18: whiteLampRoutine();            break;
        case 19: matrixRoutine();               break;
        case 20: snowRoutine();                 break;
        case 21: stormRoutine();                break;
        case 22: ballRoutine();                 break;
        case 23: ballsRoutine();                break;
        case 24: MunchRoutine();                break;
        case 25: WaveRoutine();                 break;
        case 26: LavaLampRoutine();             break;
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
      FastLED.show();
    }
  }
}

void changePower() {    // плавное включение/выключение
  if (ONflag) {
    effectsTick();
    for (int i = 0; i < modes[currentMode].Brightness; i += 8) {
      FastLED.setBrightness(i);
      delay(1);
      FastLED.show();
    }
    FastLED.setBrightness(modes[currentMode].Brightness);
    delay(2);
    FastLED.show();
  } else {
    effectsTick();
    for (int i = modes[currentMode].Brightness; i > 8; i -= 8) {
      FastLED.setBrightness(i);
      delay(1);
      FastLED.show();
    }
    memset8( leds, 0, NUM_LEDS * 3);
    delay(2);
    FastLED.show();
  }
}
