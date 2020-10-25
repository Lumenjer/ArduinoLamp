//взял от kDn
boolean brightDirection, speedDirection, scaleDirection;
void debugPrint() {
#ifdef DEBUG
  Serial.print(F(" brightness:"));
  Serial.print(modes[currentMode].Brightness);
  Serial.print(F(" speed:"));
  Serial.print(modes[currentMode].Speed);
  Serial.print(F(" scale:"));
  Serial.print(modes[currentMode].Scale);
  Serial.print(F("mode:"));
  Serial.print(currentMode);
#endif
}

void buttonTick() {
  touch.tick();
  if (touch.isSingle()) {
#ifdef DEBUG
    Serial.println(F("Double click"));
    //debugPrint(); // отладка
#endif
    if (++currentMode >= MODE_AMOUNT) currentMode = 0;
    FastLED.setBrightness(Brightness);
    loadingFlag = true;
    //settChanged = true;
    FastLED.clear();
    delay(1);
  }
  if (touch.isDouble()) {
#ifdef DEBUG
    Serial.println(F("Triple click"));
    //debugPrint(); // отладка
#endif
    if (--currentMode < 0) currentMode = MODE_AMOUNT - 1;
    FastLED.setBrightness(Brightness);
    loadingFlag = true;
    //settChanged = true;
    FastLED.clear();
    delay(1);
  }

  if (touch.hasClicks()) {
    if (touch.getClicks() == 4) {
      if (palette >= 11) palette = 0;
      else palette ++;
      loadingFlag = true;
    }
  }
  val1 = analogRead(POT_PIN);
  constrain(val1,0,1023);
  Brightness = map(val1,0,1023,1,255);
  val2 = analogRead(POT2_PIN);
  constrain(val1,0,1023);
  Speed = map(val1,0,1023,1,255);
  val3 = analogRead(POT3_PIN);
  constrain(val1,0,1023);
  Scale = map(val1,0,1023,1,255);


  if ((millis() - userTimer > DEMOTIME * 1000) && (numHold == 254)) {
    //FastLED.clear();
    //delay(2);
    for (byte i = 250; i > 10; i -= 10) {
      //fader(30);
      FastLED.delay(33);
      //FastLED.show();
    }

    if (RANDOM_DEMO)
      currentMode = random(255);
    else
      currentMode = (currentMode + 1);
#ifdef DEBUG
    Serial.print(F("Demo mode: "));
    Serial.println(currentMode);
#endif
    userTimer = millis();
  }

  FastLED.setBrightness(Brightness);
}
