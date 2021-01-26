boolean brightDirection;
#include <GyverButton.h>
GButton touch(CONTROL_PIN, LOW_PULL, NORM_OPEN);
void buttonTick() {
  touch.tick();
  if (touch.isSingle()) {
    
      if (ONflag) {
        ONflag = false;
        changePower();
      } else {
        ONflag = true;
        changePower();
      }
    }
 

  if (ONflag && touch.isDouble()) {
    if (++currentMode >= MODE_AMOUNT) currentMode = 0;
    FastLED.setBrightness(Brightness);
    loadingFlag = true;
    FastLED.clear();
    delay(1);
  }
  if (ONflag && touch.isTriple()) {
    if (--currentMode < 0) currentMode = MODE_AMOUNT - 1;
    FastLED.setBrightness(Brightness);
    loadingFlag = true;
    FastLED.clear();
    delay(1);
  }

  // 
  if (ONflag && touch.hasClicks()) {
    if (touch.getClicks() == 5) {
      if (EEPROM.read(0) != 102) EEPROM.write(0, 102);
      if (EEPROM.read(1) != currentMode) EEPROM.write(1, currentMode);  // запоминаем текущий эфект
      if (EEPROM.read(3 + 11) != Brightness) EEPROM.write(3 + 11, Brightness);
      if (EEPROM.read(3 + 12) != Speed) EEPROM.write(3 + 12, Speed);
      if (EEPROM.read(3 + 13) != Scale) EEPROM.write(3 + 13, Scale);
      if (EEPROM.read(3 + 14) != palette) EEPROM.write(3 + 14, palette);
  }}

  if (ONflag && touch.isHolded()) {
    brightDirection = !brightDirection;
  }
  if (ONflag && touch.isStep()) {
    if (brightDirection) {
      if (Brightness < 10) Brightness += 1;
      else if (Brightness < 250) Brightness += 5;
      else Brightness = 255;
    } else {
      if (Brightness > 15) Brightness -= 5;
      else if (Brightness > 1) Brightness -= 1;
      else Brightness = 1;
    }
    FastLED.setBrightness(Brightness);
  }
}
