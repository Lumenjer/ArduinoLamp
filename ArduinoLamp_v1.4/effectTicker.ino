uint32_t effTimer;
byte ind;
void effectsTick() {
  {
    if (millis() - effTimer >= ((currentMode < 2 || currentMode > 8) ? Speed : 50) ) {
      effTimer = millis(); switch (currentMode) {
        //|номер   |название функции эффекта     |тоже надо|
        case 0 : sparklesRoutine();             break;
        case 1 : RainbowRoutine();              break;
        case 2 : fireRoutine();                 break;
        case 3 : shadowsRoutine();              break;
        case 4 : madnessNoise();                break;
        case 5 : noise3D();                     break;
        case 6 : whiteLampRoutine();            break;
        case 7 : colorRoutine();                break;
        case 8 : colorsRoutine();               break;
        case 9 : matrixRoutine();               break;
        case 10: snowRoutine();                 break;
        case 11: stormRoutine();                break;
        case 12: ballRoutine();                 break;
        case 13: ballsRoutine();                break;
        case 14: MunchRoutine();                break;
        case 15: WaveRoutine();                 break;
        case 16: LavaLampRoutine();             break;
      }
      }
      FastLED.show();
    }
  }
