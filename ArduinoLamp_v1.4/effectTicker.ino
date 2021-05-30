// If you want to add effects or make copies for them to demonstrate in different settings, you need to do it in 3 places:
// 1. in the file effects.ino - the program code of the effect is added.
// 2. Constants.h specifies the total number of MODE_AMOUNT.
// 3. here in the file effectTicker.ino - the procedure of calling the effect of the case "number" is connected: "effect name"; break;
// It is possible to connect the same effect under different numbers. But that's too much.
void effectsTick() {
  if (!recievedFlag && ONflag && millis() - effTimer >= ((currentMode < 4 || currentMode > 6) ? 256 - Speed[currentMode] : 50) ) {
    effTimer = millis(); switch (currentMode) {
      //|num  |function name
      case 0 : sparklesRoutine();             break;
      case 1 : RainbowRoutine();              break;
      case 2 : FireRoutine();                 break;
      case 3 : sendVoxels();                  break;
      case 4 : Noise3D();                     break;
      case 5 : whiteLampRoutine();            break;
      case 6 : colorRoutine();                break;
      case 7 : colorsRoutine();               break;
      case 8 : matrixRoutine();               break;
      case 9 : RainRoutine();                 break;
      case 10: stormRoutine();                break;
      case 11: ballRoutine();                 break;
      case 12: ballsRoutine();                break;
      case 13: LLandRoutine();                break;
    }
    if (runningFlag) fillString(runningText, COL);
#ifdef INDICATOR
    if (numHold != 0)drawLevel();
#endif
#ifdef USE_IR
    if (!IRLremote.receiving())
      FastLED.show();
#else
    FastLED.show();
#endif
  }
}

void drawLevel() {
#if (ROATRE)
  switch (numHold) {
    case 1:
      ind = map(Brightness[currentMode], 0, 255, 0, WIDTH - 1);
      for (byte x = 0; x < HEIGHT ; x++) {
        if (ind > x) drawPixelXY(x, IND_POS, CHSV(10, 255, 255));
        else drawPixelXY(x, IND_POS,  0);
      }
      break;
    case 2:
      ind = map(Speed[currentMode], 0, 255, 0, WIDTH - 1);
      for (byte x = 0; x < WIDTH ; x++) {
        if (ind > x) drawPixelXY(x, IND_POS, CHSV(100, 255, 255));
        else drawPixelXY(x, IND_POS,  0);
      }
      break;
    case 3:
      ind = map(Scale[currentMode], 0, 255, 0, WIDTH - 1);
      for (byte x = 0; x < WIDTH ; x++) {
        if (ind > x) drawPixelXY(x, IND_POS, CHSV(150, 255, 255));
        else drawPixelXY(x, IND_POS,  0);
      }
      break;
  }
#else
  switch (numHold) {
    case 1:
      ind = map(Brightness[currentMode], 0, 255, 0, HEIGHT - 1);
      for (byte y = 0; y < HEIGHT ; y++) {
        if (ind > y) drawPixelXY(IND_POS, y, CHSV(10, 255, 255));
        else drawPixelXY(IND_POS, y,  0);
      }
      break;
    case 2:
      ind = map(Speed[currentMode], 0, 255, 0, HEIGHT - 1);
      for (byte y = 0; y <= HEIGHT ; y++) {
        if (ind > y) drawPixelXY(IND_POS, y, CHSV(100, 255, 255));
        else drawPixelXY(IND_POS, y,  0);
      }
      break;
    case 3:
      ind = map(Scale[currentMode], 0, 255, 0, HEIGHT - 1);
      for (byte y = 0; y < HEIGHT ; y++) {
        if (ind > y) drawPixelXY(IND_POS, y, CHSV(150, 255, 255));
        else drawPixelXY(IND_POS, y,  0);
      }
      break;
  }
#endif
}

void demoTick() {
  if (isDemo && ONflag && millis() >= DemTimer) {
#ifdef RANDOM_DEMO
    GoToEffect(random8(MODE_AMOUNT)); // если нужен следующий случайный эффект
#else
    NextEffect();
#endif
#ifdef RANDOM_EFF
    Speed[currentMode] = random8(); Scale[currentMode] = random8();
#endif
    memset8( leds, 0, NUM_LEDS * 3);
    DemTimer = millis() + DEMOT * 1000;
    loadingFlag = true;
  }
}
