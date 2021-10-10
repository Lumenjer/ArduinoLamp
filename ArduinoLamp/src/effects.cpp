#include "main.h"
// =============================== EFFECTS ====================================
// --------------------------------- Confetti ------------------------------------
#define FADE_OUT_SPEED (70U) // attenuation rate
// --------------------------------- Storm, Blizzard ---------------------------------
#define SNOW_DENSE (32U) // snow density - less = denser
#define TAIL_STEP (127U) // tail length
// ------------------------------- Wandering Cube ------------------------------
#define RANDOM_COLOR (1U) // random color when bouncing
// ------------------------------ Fireflies with a train ----------------------------
#define BALLS_AMOUNT (3U) // number of "balls"
#define CLEAR_PATH (1U) // clear path
#define TRACK_STEP (70U) // ball tail length (the larger the number, the shorter the tail)
// --------------------------------- Fire (New) ----------- ---------------------
#define F_SCALE (10U) // fire scale
// ======================== VARIABLES =============================
uint8_t step;
uint8_t shiftValue[HEIGHT];
uint8_t shiftHue[HEIGHT];
uint8_t deltaHue, deltaHue2;
uint8_t deltaValue;
byte hue, hue2;
CRGB _pulse_color;
CRGBPalette16 currentPalette(PartyColors_p);
extern const TProgmemRGBPalette16 WaterfallColors_p FL_PROGMEM = {0x000000, 0x060707, 0x101110, 0x151717, 0x1C1D22, 0x242A28, 0x363B3A, 0x313634, 0x505552, 0x6B6C70, 0x98A4A1, 0xC1C2C1, 0xCACECF, 0xCDDEDD, 0xDEDFE0, 0xB2BAB9};
static const TProgmemRGBPalette16 ZeebraColors_p FL_PROGMEM = {CRGB::White, CRGB::Black, CRGB::Black, CRGB::Black, CRGB::White, CRGB::Black, CRGB::Black, CRGB::Black, CRGB::White, CRGB::Black, CRGB::Black, CRGB::Black, CRGB::White, CRGB::Black, CRGB::Black, CRGB::Black};
const TProgmemRGBPalette16 *palette_arr[] = {
  &PartyColors_p,
  &OceanColors_p,
  &LavaColors_p,
  &HeatColors_p,
  &WaterfallColors_p,
  &CloudColors_p,
  &ForestColors_p,
  &RainbowColors_p,
  &RainbowStripeColors_p,
  &ZeebraColors_p,
};
const TProgmemRGBPalette16 *curPalette = palette_arr[0];
void setCurrentPalette(uint8_t palIdx) {
  curPalette = palette_arr[palIdx];
}

#define LIGHTERS_AM (max(WIDTH, HEIGHT))
struct {
  int16_t PosX, PosY;
  int8_t SpeedX, SpeedY;
  uint8_t Color;
} Dot[LIGHTERS_AM];
// ========================== КОДЫ ЭФФЕКТОВ ===========================
/*class Eff {
  private:
  public:
    void Load() override;
    bool Run() override;
};

class EffSparkles :
  public Eff {
  private:
    uint8_t x, y;
  public:
  Load(){}
    Run() {
      for (uint8_t i = 0; i < Scale; i++)
      {
        x = random8(0, WIDTH);
        y = random8(0, HEIGHT);
        if (leds[XY(x, y)] == CRGB(0, 0, 0))
        {
          drawPixelXY(x, y, CHSV(random(0U, 255U), 255U, 255U));
        }
      }
      dimAll(255 - Speed);
      return true;
    }
};

class EffRainbow :
  public Eff {
  private:
    uint8_t hue;
    void rainbowHorVertRoutine(bool isVertical)
{ hue += 4;
  for (uint8_t i = 0U; i < (isVertical ? WIDTH : HEIGHT); i++)
  {
    for (uint8_t j = 0U; j < (isVertical ? HEIGHT : WIDTH); j++)
    {
      drawPixelXY((isVertical ? i : j), (isVertical ? j : i), CHSV((uint8_t)(hue + i * Scale % 170), 255, 255));
    }
  }
}
  public:
  Load(){}
  Run(){
if (Scale < 85) {
    rainbowHorVertRoutine(false);
  } else if (Scale > 170) {
    rainbowHorVertRoutine(true);
  } else {
    hue += 8;
    for (uint8_t i = 0U; i < WIDTH; i++)
    {
      for (uint8_t j = 0U; j < HEIGHT; j++)
      {
        float twirlFactor = fmap((float)Scale, 85, 170, 8.3, 24);      // на сколько оборотов будет закручена матрица, [0..3]
        drawPixelXY(i, j, CHSV((uint8_t)(hue + ((float)WIDTH / HEIGHT * i + j * twirlFactor) * ((float)255 / maxDim)), 255, 255));
      }
    }
  }  
  return true;}
};


class EffColor :
  public Eff {
  private:
    uint8_t hue;
  public:
  Load(){}
  Run(){
    hue += Scale;
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(hue, 255, 255);
  return true;}
};

class EffColorRun :
  public Eff {
  private:
  public:
  Load(){}
  Run(){
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(Scale, Speed, 255);}
  return true;}
};

class EffRain :
  public Eff {
  private:
  public:
  Load(){}
  Run(){
  for (uint8_t x = 0U; x < WIDTH; x++)
  {
    // заполняем случайно верхнюю строку
    CRGB thisColor = leds[XY(x, HEIGHT - 1U)];
    if ((uint32_t)thisColor == 0U)
    {
      if (random8(0, 50) == 0U)
      {
        if (Scale == 1) drawPixelXY(x, HEIGHT - 1U, CHSV(random(0, 9) * 28, 255U, 255U)); // Радужный дождь
        else if (Scale == 255) drawPixelXY(x, HEIGHT - 1U, 0xE0FFFF - 0x101010 * random(0, 4)); // Снег
        else
          drawPixelXY(x, HEIGHT - 1U, CHSV(Scale + random(0, 16), 255, 255)); // Цветной дождь
      }
    }
    else
      leds[XY(x, HEIGHT - 1U)] -= CHSV(0, 0, random(96, 128));
  }
  // сдвигаем всё вниз
  for (uint8_t x = 0U; x < WIDTH; x++)
  {
    for (uint8_t y = 0U; y < HEIGHT - 1U; y++)
    {
      drawPixelXY(x, y, getPixColorXY(x, y + 1U));
    }
  }
  return true;}
};

class EffMatrix :
  public Eff {
  private:
  public:
  Load(){}
  Run(){
  for (uint8_t x = 0U; x < WIDTH; x++)
  {
    // обрабатываем нашу матрицу снизу вверх до второй сверху строчки
    for (uint8_t y = 0U; y < HEIGHT - 1U; y++)
    {
      uint32_t thisColor = getPixColorXY(x, y);                                              // берём цвет нашего пикселя
      uint32_t upperColor = getPixColorXY(x, y + 1U);                                        // берём цвет пикселя над нашим
      if (upperColor >= 0x900000 && random(7 * HEIGHT) != 0U)                  // если выше нас максимальная яркость, игнорим этот факт с некой вероятностью или опускаем цепочку ниже
        drawPixelXY(x, y, upperColor);
      else if (thisColor == 0U && random((255 - Scale) * HEIGHT) == 0U)  // если наш пиксель ещё не горит, иногда зажигаем новые цепочки
        //else if (thisColor == 0U && random((100 - Scale) * HEIGHT*3) == 0U)  // для длинных хвостов
        drawPixelXY(x, y, 0x9bf800);
      else if (thisColor <= 0x050800)                                                        // если наш пиксель почти погас, стараемся сделать затухание медленней
      {
        if (thisColor >= 0x030000)
          drawPixelXY(x, y, 0x020300);
        else if (thisColor != 0U)
          drawPixelXY(x, y, 0U);
      }
      else if (thisColor >= 0x900000)                                                        // если наш пиксель максимальной яркости, резко снижаем яркость
        drawPixelXY(x, y, 0x558800);
      else
        drawPixelXY(x, y, thisColor - 0x0a1000);                                             // в остальных случаях снижаем яркость на 1 уровень
      //drawPixelXY(x, y, thisColor - 0x050800);                                             // для длинных хвостов
    }
    // аналогично обрабатываем верхний ряд пикселей матрицы
    uint32_t thisColor = getPixColorXY(x, HEIGHT - 1U);
    if (thisColor == 0U)                                                                     // если наш верхний пиксель не горит, заполняем его с вероятностью .Scale
    {
      if (random(255 - Scale) == 0U)
        drawPixelXY(x, HEIGHT - 1U, 0x9bf800);
    }
    else if (thisColor <= 0x050800)                                                          // если наш верхний пиксель почти погас, стараемся сделать затухание медленней
    {
      if (thisColor >= 0x030000)
        drawPixelXY(x, HEIGHT - 1U, 0x020300);
      else
        drawPixelXY(x, HEIGHT - 1U, 0U);
    }
    else if (thisColor >= 0x900000)                                                          // если наш верхний пиксель максимальной яркости, резко снижаем яркость
      drawPixelXY(x, HEIGHT - 1U, 0x558800);
    else
      drawPixelXY(x, HEIGHT - 1U, thisColor - 0x0a1000);                                     // в остальных случаях снижаем яркость на 1 уровень
    //drawPixelXY(x, HEIGHT - 1U, thisColor - 0x050800);                                     // для длинных хвостов
  }
return true;}
};

class EffStorm :
  public Eff {
  private:
  public:
  Load(){}
  Run(){
  // заполняем головами комет левую и верхнюю линию
  for (byte i = HEIGHT / 2; i < HEIGHT; i++) {
    if (leds[XY(0, i)].r == 0
        && (random(0, SNOW_DENSE) == 0)
        && leds[XY(0, i + 1)].r == 0
        && leds[XY(0, i - 1)].r == 0)
      leds[XY(0, i)] = CHSV(random(0, 200), Scale, 255);
  }
  for (byte i = 0; i < WIDTH / 2; i++) {
    if (leds[XY(i, HEIGHT - 1)].r == 0
        && (random(0, SNOW_DENSE) == 0)
        && leds[XY(i + 1, HEIGHT - 1)].r == 0
        && leds[XY(i - 1, HEIGHT - 1)].r == 0)
      leds[XY(i, HEIGHT - 1)] = CHSV(random(0, 200), Scale, 255);
  }

  // сдвигаем по диагонали
  for (byte y = 0; y < HEIGHT - 1; y++) {
    for (byte x = WIDTH - 1; x > 0; x--) {
      drawPixelXY(x, y, leds[XY(x - 1, y + 1)]);
    }
  }

  // уменьшаем яркость левой и верхней линии, формируем "хвосты"
  for (byte i = HEIGHT / 2; i < HEIGHT; i++) {
    fadePixel(0, i, 120);
  }
  for (byte i = 0; i < WIDTH / 2; i++) {
    fadePixel(i, HEIGHT - 1, TAIL_STEP);
  return true;}
}
};

*/

// ---------------------------- КОНФЕТТИ ------------------------------
void sparklesRoutine()
{
  for (uint8_t i = 0; i < Scale; i++)
  {
    uint8_t x = random(0U, WIDTH);
    uint8_t y = random(0U, HEIGHT);
    if (getPixColorXY(x, y) == 0U)
    {
      drawPixelXY(x, y, CHSV(random(0U, 255U), 255U, 255U));
    }
  }
  dimAll(255 - FADE_OUT_SPEED);
}
void fadePixel(uint8_t i, uint8_t j, uint8_t step)          // фейдер
{
  int32_t pixelNum = XY(i, j);
  if (getPixColor(pixelNum) == 0U) return;

  if (leds[pixelNum].r >= 30U ||
      leds[pixelNum].g >= 30U ||
      leds[pixelNum].b >= 30U)
  {
    leds[pixelNum].fadeToBlackBy(step);
  }
  else
  {
    leds[pixelNum] = 0U;
  }
}
// ----------------------------- РАДУГА 2Д --------------------------
// ________________ РАДУГА ВЕРТИКАЛЬНАЯ/ГОРИЗОНТАЛЬНАЯ ______________
void rainbowHorVertRoutine(bool isVertical)
{ hue += 4;
  for (uint8_t i = 0U; i < (isVertical ? WIDTH : HEIGHT); i++)
  {
    for (uint8_t j = 0U; j < (isVertical ? HEIGHT : WIDTH); j++)
    {
      drawPixelXY((isVertical ? i : j), (isVertical ? j : i), CHSV((uint8_t)(hue + i * Scale % 170), 255, 255));
    }
  }
}

// ________________________ РАДУГА ДИАГОНАЛЬНАЯ _______________________
void RainbowRoutine()
{
  if (Scale < 85) {
    rainbowHorVertRoutine(false);
  } else if (Scale > 170) {
    rainbowHorVertRoutine(true);
  } else {
    hue += 8;
    for (uint8_t i = 0U; i < WIDTH; i++)
    {
      for (uint8_t j = 0U; j < HEIGHT; j++)
      {
        float twirlFactor = fmap((float)Scale, 85, 170, 8.3, 24);      // на сколько оборотов будет закручена матрица, [0..3]
        drawPixelXY(i, j, CHSV((uint8_t)(hue + ((float)WIDTH / HEIGHT * i + j * twirlFactor) * ((float)255 / maxDim)), 255, 255));
      }
    }
  }
}

// --------------------------------- ЦВЕТА -----------------------------
void colorsRoutine() {
  hue += Scale;
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(hue, 255, 255);
  }
}

// --------------------------------- ЦВЕТ --------------------------------
void colorRoutine() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(Scale, Speed, 255);
  }
}
// --------------------------------- ДОЖДЬ -------------------------------
// от @Shaitan
void RainRoutine()
{
  for (uint8_t x = 0U; x < WIDTH; x++)
  {
    // заполняем случайно верхнюю строку
    CRGB thisColor = getPixColorXY(x, HEIGHT - 1U);
    if ((uint32_t)thisColor == 0U)
    {
      if (random8(0, 50) == 0U)
      {
        if (Scale == 1) drawPixelXY(x, HEIGHT - 1U, CHSV(random(0, 9) * 28, 255U, 255U)); // Радужный дождь
        else if (Scale == 255) drawPixelXY(x, HEIGHT - 1U, 0xE0FFFF - 0x101010 * random(0, 4)); // Снег
        else
          drawPixelXY(x, HEIGHT - 1U, CHSV(Scale + random(0, 16), 255, 255)); // Цветной дождь
      }
    }
    else
      leds[XY(x, HEIGHT - 1U)] -= CHSV(0, 0, random(96, 128));
  }
  // сдвигаем всё вниз
  for (uint8_t x = 0U; x < WIDTH; x++)
  {
    for (uint8_t y = 0U; y < HEIGHT - 1U; y++)
    {
      drawPixelXY(x, y, getPixColorXY(x, y + 1U));
    }
  }
}


// ------------------------------ МАТРИЦА ------------------------------
void matrixRoutine()
{
  for (uint8_t x = 0U; x < WIDTH; x++)
  {
    // обрабатываем нашу матрицу снизу вверх до второй сверху строчки
    for (uint8_t y = 0U; y < HEIGHT - 1U; y++)
    {
      uint32_t thisColor = getPixColorXY(x, y);                                              // берём цвет нашего пикселя
      uint32_t upperColor = getPixColorXY(x, y + 1U);                                        // берём цвет пикселя над нашим
      if (upperColor >= 0x900000 && random(7 * HEIGHT) != 0U)                  // если выше нас максимальная яркость, игнорим этот факт с некой вероятностью или опускаем цепочку ниже
        drawPixelXY(x, y, upperColor);
      else if (thisColor == 0U && random((255 - Scale) * HEIGHT) == 0U)  // если наш пиксель ещё не горит, иногда зажигаем новые цепочки
        //else if (thisColor == 0U && random((100 - Scale) * HEIGHT*3) == 0U)  // для длинных хвостов
        drawPixelXY(x, y, 0x9bf800);
      else if (thisColor <= 0x050800)                                                        // если наш пиксель почти погас, стараемся сделать затухание медленней
      {
        if (thisColor >= 0x030000)
          drawPixelXY(x, y, 0x020300);
        else if (thisColor != 0U)
          drawPixelXY(x, y, 0U);
      }
      else if (thisColor >= 0x900000)                                                        // если наш пиксель максимальной яркости, резко снижаем яркость
        drawPixelXY(x, y, 0x558800);
      else
        drawPixelXY(x, y, thisColor - 0x0a1000);                                             // в остальных случаях снижаем яркость на 1 уровень
      //drawPixelXY(x, y, thisColor - 0x050800);                                             // для длинных хвостов
    }
    // аналогично обрабатываем верхний ряд пикселей матрицы
    uint32_t thisColor = getPixColorXY(x, HEIGHT - 1U);
    if (thisColor == 0U)                                                                     // если наш верхний пиксель не горит, заполняем его с вероятностью .Scale
    {
      if (random(255 - Scale) == 0U)
        drawPixelXY(x, HEIGHT - 1U, 0x9bf800);
    }
    else if (thisColor <= 0x050800)                                                          // если наш верхний пиксель почти погас, стараемся сделать затухание медленней
    {
      if (thisColor >= 0x030000)
        drawPixelXY(x, HEIGHT - 1U, 0x020300);
      else
        drawPixelXY(x, HEIGHT - 1U, 0U);
    }
    else if (thisColor >= 0x900000)                                                          // если наш верхний пиксель максимальной яркости, резко снижаем яркость
      drawPixelXY(x, HEIGHT - 1U, 0x558800);
    else
      drawPixelXY(x, HEIGHT - 1U, thisColor - 0x0a1000);                                     // в остальных случаях снижаем яркость на 1 уровень
    //drawPixelXY(x, HEIGHT - 1U, thisColor - 0x050800);                                     // для длинных хвостов
  }
}


// ------------------------------- БЕЛАЯ ЛАМПА  -------------------------
// mod by @Fruity
void whiteLampRoutine()
{
  if (loadingFlag)
  {
    loadingFlag = false;
    FastLED.clear();
    //delay(1);

    uint8_t centerY =  (uint8_t)round(HEIGHT / 2.0F) - 1U;// max((uint8_t)round(HEIGHT / 2.0F) - 1, 0); нахрена тут максимум было вычислять? для ленты?!
    uint8_t bottomOffset = (uint8_t)(!(HEIGHT & 0x01));// && (HEIGHT > 1)); и высота больше единицы. супер!                     // если высота матрицы чётная, линий с максимальной яркостью две, а линии с минимальной яркостью снизу будут смещены на один ряд

    uint8_t fullRows =  centerY / 100.0 * Scale;
    uint8_t iPol = (centerY / 100.0 * Scale - fullRows) * 255;

    for (int16_t y = centerY; y >= 0; y--)
    {
      CRGB color = CHSV(
                     45U,                                                                              // определяем тон
                     map(Speed, 0U, 255U, 0U, 170U),                                // определяем насыщенность
                     y > (centerY - fullRows - 1)                                                      // определяем яркость
                     ? 255U                                                                            // для центральных горизонтальных полос
                     : iPol * (y > centerY - fullRows - 2));  // для остальных горизонтальных полос яркость равна либо 255, либо 0 в зависимости от масштаба

      for (uint8_t x = 0U; x < WIDTH; x++)
      {
        drawPixelXY(x, y, color);                              // при чётной высоте матрицы максимально яркими отрисуются 2 центральных горизонтальных полосы
        drawPixelXY(x, HEIGHT + bottomOffset - y - 2U, color); // при нечётной - одна, но дважды
      }
    }
  }
}

// ----------------------- ШТОРМ/МЕТЕЛЬ/МЕТЕОРЫ ------------------------
void stormRoutine() {
  // заполняем головами комет левую и верхнюю линию
  for (byte i = HEIGHT / 2; i < HEIGHT; i++) {
    if (leds[XY(0, i)].r == 0
        && (random(0, SNOW_DENSE) == 0)
        && leds[XY(0, i + 1)].r == 0
        && leds[XY(0, i - 1)].r == 0)
      leds[XY(0, i)] = CHSV(random(0, 200), Scale, 255);
  }
  for (byte i = 0; i < WIDTH / 2; i++) {
    if (leds[XY(i, HEIGHT - 1)].r == 0
        && (random(0, SNOW_DENSE) == 0)
        && leds[XY(i + 1, HEIGHT - 1)].r == 0
        && leds[XY(i - 1, HEIGHT - 1)].r == 0)
      leds[XY(i, HEIGHT - 1)] = CHSV(random(0, 200), Scale, 255);
  }

  // сдвигаем по диагонали
  for (byte y = 0; y < HEIGHT - 1; y++) {
    for (byte x = WIDTH - 1; x > 0; x--) {
      drawPixelXY(x, y, leds[XY(x - 1, y + 1)]);
    }
  }

  // уменьшаем яркость левой и верхней линии, формируем "хвосты"
  for (byte i = HEIGHT / 2; i < HEIGHT; i++) {
    fadePixel(0, i, 120);
  }
  for (byte i = 0; i < WIDTH / 2; i++) {
    fadePixel(i, HEIGHT - 1, TAIL_STEP);
  }
}

// ------------------------ БЛУЖДАЮЩИЙ КУБИК -----------------------
//int8_t deltaValue; //ballSize;

void ballRoutine()
{
  if (loadingFlag)
  {
    loadingFlag = false;
    //FastLED.clear();


    Dot[0].PosX = WIDTH / 2 * 10;
    Dot[0].PosY = HEIGHT / 2 * 10;
    Dot[0].SpeedX = random(4, 10);
    Dot[0].SpeedY = random(4, 10);

    deltaValue = map(Scale, 0, 255, 1, (uint8_t)min(WIDTH, HEIGHT)-2);
    Dot[0].Color = random(0, 9) * 28;
    //    _pulse_color = CHSV(random(0, 9) * 28, 255U, 255U);
  }

  //  if (!(Scale & 0x01))
  //  {
  //    hue += (Scale - 1U) % 11U * 8U + 1U;

  //    ballColor = CHSV(hue, 255U, 255U);
  //  }

  if ((Scale & 0x01))
    for (uint8_t i = 0U; i < deltaValue; i++)
      for (uint8_t j = 0U; j < deltaValue; j++)
        leds[XY(Dot[0].PosX / 10 + i, Dot[0].PosY / 10 + j)] = _pulse_color;


  Dot[0].PosX += Dot[0].SpeedX;

  if (Dot[0].PosX < 0)
  {
    Dot[0].PosX = 0;
    Dot[0].SpeedX = -Dot[0].SpeedX;
    if (RANDOM_COLOR) Dot[0].Color = random(0, 9) * 28; // if (RANDOM_COLOR && (Scale & 0x01))
    //vectorB[i] += random(0, 6) - 3;
  }
  Dot[0].PosY += Dot[0].SpeedY;
  if (Dot[0].PosY < 0)
  {
    Dot[0].PosY = 0;
    Dot[0].SpeedY = -Dot[0].SpeedY;
    if (RANDOM_COLOR) Dot[0].Color = random(0, 9) * 28; // if (RANDOM_COLOR && (Scale & 0x01))
    //vectorB[i] += random(0, 6) - 3;
  }

  if (Dot[0].PosX > (int16_t)((WIDTH - deltaValue) * 10))
  {
    Dot[0].PosX = (WIDTH - deltaValue) * 10;
    Dot[0].SpeedX = -Dot[0].SpeedX;
    if (RANDOM_COLOR) Dot[0].Color = random(0, 9) * 28;
    //vectorB[0] += random(0, 6) - 3;
  }
  if (Dot[0].PosY > (int16_t)((HEIGHT - deltaValue) * 10))
  {
    Dot[0].PosY = (HEIGHT - deltaValue) * 10;
    Dot[0].SpeedY = -Dot[0].SpeedY;
    if (RANDOM_COLOR) Dot[0].Color = random(0, 9) * 28;
    //vectorB[1] += random(0, 6) - 3;
  }

  //if (Scale & 0x01)
  //dimAll(135U);
  //dimAll(255U - (Scale - 1U) % 11U * 24U);
  else
    FastLED.clear();

  for (uint8_t i = 0U; i < deltaValue; i++)
    for (uint8_t j = 0U; j < deltaValue; j++)
      drawPixelXY(Dot[0].PosX / 10 + i, Dot[0].PosY / 10 + j, CHSV(Dot[0].Color, 255, 255));
}
// ------------------------- СВЕТЛЯЧКИ СО ШЛЕЙФОМ ------------------
void ballsRoutine()
{
  if (loadingFlag)
  {
    loadingFlag = false;

    for (uint8_t j = 0U; j < LIGHTERS_AM; j++)
    {
      int8_t sign;
      // забиваем случайными данными
      Dot[j].PosX = WIDTH / 2 * 10;
      random(0, 2) ? sign = 1 : sign = -1;
      Dot[j].SpeedX = random(4, 15) * sign;
      Dot[j].PosY = HEIGHT / 2 * 10;
      random(0, 2) ? sign = 1 : sign = -1;
      Dot[j].SpeedY = random(4, 15) * sign;
      //ballColors[j] = CHSV(random(0, 9) * 28, 255U, 255U);
      // цвет зависит от масштаба
      Dot[j].Color = random();
    }
  }

  dimAll(256U - TRACK_STEP);

  // движение шариков
  for (uint8_t j = 0U; j < map(Scale, 1, 255, 1, LIGHTERS_AM); j++)
  {

    Dot[j].PosX += Dot[j].SpeedX;
    Dot[j].PosY += Dot[j].SpeedY;
    if (Dot[j].PosX < 0)
    {
      Dot[j].PosX = 0;
      Dot[j].SpeedX = -Dot[j].SpeedX;
    }
    if (Dot[j].PosY < 0)
    {
      Dot[j].PosY = 0;
      Dot[j].SpeedY = -Dot[j].SpeedY;
    }

    if (Dot[j].PosX > (int16_t)((WIDTH - 1) * 10))
    {
      Dot[j].PosX = (WIDTH - 1) * 10;
      Dot[j].SpeedX = -Dot[j].SpeedY;
    }
    if (Dot[j].PosY > (int16_t)((HEIGHT - 1) * 10))
    {
      Dot[j].PosY = (HEIGHT - 1) * 10;
      Dot[j].SpeedY = -Dot[j].SpeedY;
    }
    drawPixelXY(Dot[j].PosX / 10, Dot[j].PosY / 10, CHSV(Dot[j].Color, 255, 255));
  }
}

// ------------------------------ ОГОНЬ(НОВЫЙ) -------------------------
// (c) SottNick
//сильно по мотивам https://pastebin.com/RG0QGzfK
//Perlin noise fire procedure by Yaroslaw Turbin
//https://www.reddit.com/r/FastLED/comments/hgu16i/my_fire_effect_implementation_based_on_perlin/
uint16_t ff_y, ff_z;// используем для сдвига нойза переменные из общих
void FireRoutine() {
  if (loadingFlag) {
    loadingFlag = false;
    deltaValue = F_SCALE * 0.0899;// /100.0F * ((sizeof(palette_arr) /sizeof(TProgmemRGBPalette16 *))-0.01F));
    deltaValue = (((F_SCALE - 1U) % 11U + 1U) << 4U) - 8U; // ширина языков пламени (масштаб шума Перлина)
    deltaHue = map(deltaValue, 8U, 168U, 8U, 84U); // высота языков пламени должна уменьшаться не так быстро, как ширина
    step = map(255U - deltaValue, 87U, 247U, 4U, 32U); // вероятность смещения искорки по оси ИКС
    for (uint8_t j = 0; j < HEIGHT; j++) {
      shiftHue[j] = (HEIGHT - 1 - j) * 255 / (HEIGHT - 1); // init colorfade table
      currentPalette = CRGBPalette16(CRGB::Black, CHSV(Scale, 255U, 255U) , CHSV(Scale + 50, 255U, 255U) , CHSV(Scale + 50, 100, 255U));
    }

    for (uint8_t i = 0; i < (WIDTH / 8); i++) {
      Dot[i].PosY = random8(HEIGHT);
      Dot[i].PosX = random8(WIDTH);
    }
  }
  for (uint8_t i = 0; i < WIDTH; i++) {
    for (uint8_t j = 0; j < HEIGHT; j++) {
      if (Scale == 255)
        leds[XY(i, (HEIGHT - 1) - j)] = ColorFromPalette(WaterfallColors_p, qsub8(inoise8(i * deltaValue, (j + ff_y + random8(2)) * deltaHue, ff_z), shiftHue[j]), 255U);
      else if (Scale == 1)
        leds[XY(i, (HEIGHT - 1) - j)] = ColorFromPalette(HeatColors_p, qsub8(inoise8(i * deltaValue, (j + ff_y + random8(2)) * deltaHue, ff_z), shiftHue[j]), 255U);
      else
        leds[XY(i, HEIGHT - 1 - j)] = ColorFromPalette(currentPalette, qsub8(inoise8(i * deltaValue, (j + ff_y + random8(2)) * deltaHue, ff_z), shiftHue[j]), 255U);
    }
  }

  //вставляем искорки из отдельного массива
  for (uint8_t i = 0; i < (WIDTH / 8); i++) {
    if (Dot[i].PosY > 3U) {
      leds[XY(Dot[i].PosX, Dot[i].PosY)] = leds[XY(Dot[i].PosY, 3U)];
      leds[XY(Dot[i].PosX, Dot[i].PosY)].fadeToBlackBy(Dot[i].PosY * 2U );
    }
    Dot[i].PosY++;
    if (Dot[i].PosY >= HEIGHT) {
      Dot[i].PosY = random8(4U);
      Dot[i].PosX = random8(WIDTH);
    }
    if (!random8(step))
      Dot[i].PosX = (WIDTH + Dot[i].PosX + 1U - random8(3U)) % WIDTH;
  }
  ff_y++;
  if (ff_y & 0x01)
    ff_z++;
}

byte selX;
bool sending, sendDirection;
void sendVoxels() { // remade by me
  if (loadingFlag) {
    FastLED.clear();
    for (uint8_t i = 0; i < WIDTH; i++) {
      Dot[i].PosX = i;
      Dot[i].PosY = (random(2) == 1) ? (HEIGHT - 1) * 10 : 0;
    }
    loadingFlag = false;
  }
  FastLED.clear();
  for (uint8_t i = 0; i < WIDTH; i++) {
    drawPixelXY(Dot[i].PosX, Dot[i].PosY / 10, CHSV(Scale, 255, 255));
    if (!sending) {
      selX = random(0, WIDTH - 1);
      if (Dot[selX].PosY == 0) {

        sendDirection = 1;
      } else if (Dot[selX].PosY == (HEIGHT - 1) * 10) {
        sendDirection = 0;
      }
      sending = true;
    } else {
      if (sendDirection == 1) {
        Dot[selX].PosY += 1;
        if (Dot[selX].PosY >= (HEIGHT - 1) * 10) {
          Dot[selX].PosY = (HEIGHT - 1) * 10;
          sending = false;
        }
      } else {
        Dot[selX].PosY -= 1;
        if (Dot[selX].PosY <= 0) {
          Dot[selX].PosY = 0;
          sending = false;
        }
      }
    }
  }
}

void N_Dot_Reload(byte id) {
  Dot[id].SpeedY = random(0, 4); //Direction
  Dot[id].Color = random();
  switch (Dot[id].SpeedY) {
    case 0:   // вверх
      Dot[id].PosX = random(0, (WIDTH - 1) * 10); // Разбрасываем капли по ширине
      Dot[id].PosY = 0;  // и по высоте
      break;
    case 1:   //  вниз
      Dot[id].PosX = random(0, (WIDTH - 1) * 10); // Разбрасываем капли по ширине
      Dot[id].PosY = (HEIGHT - 1) * 10; // и по высоте
      break;
    case 2:   // вправо
      Dot[id].PosX = 0; // Разбрасываем капли по ширине
      Dot[id].PosY = random(0, (HEIGHT - 1) * 10); // и по высоте
      break;
    case 3:   // влево
      Dot[id].PosX = (WIDTH - 1) * 10; // Разбрасываем капли по ширине
      Dot[id].PosY = random(0, (HEIGHT - 1) * 10); // и по высоте
      break;
    default:
      break;
  }
}

void Nexus() {
  if (loadingFlag) {
    setCurrentPalette(palette);
    for (byte i = 0; i < LIGHTERS_AM; i++) {
      N_Dot_Reload(i);
    }
    loadingFlag = false;
  }
  fadeToBlackBy(leds, NUM_LEDS, 11);
  for (byte i = 0; i < map(Scale, 1, 255, 1, LIGHTERS_AM); i++) {
    switch (Dot[i].SpeedY)
    {
      case 0:   // вверх
        Dot[i].PosY += 2;
        break;
      case 1:   //  вниз
        Dot[i].PosY -= 2;
        break;
      case 2:   // вправо
        Dot[i].PosX += 2;
        break;
      case 3:   // влево
        Dot[i].PosX -= 2;
        break;
      default:
        break;
    }

    // Обеспечиваем бесшовность по Y. И переносим каплю в начало трека
    if (Dot[i].PosY < 0) {
      N_Dot_Reload(i);
    }

    if (Dot[i].PosY > (HEIGHT - 1) * 10) {
      N_Dot_Reload(i);
    }

    // Обеспечиваем бесшовность по X.
    if (Dot[i].PosX < 0) {
      N_Dot_Reload(i);
    }
    if (Dot[i].PosX > (WIDTH - 1) * 10) {
      N_Dot_Reload(i);
    }
    drawPixelXY(Dot[i].PosX / 10, Dot[i].PosY / 10, ColorFromPalette(*curPalette, Dot[i].Color));
  }
}


//Noise3D
int x, y, z;

void FillNoise(bool ShiftX, bool ShiftY, bool ShiftZ, CRGBPalette16 palette, bool ShiftHue, bool BriNoise) {
  z += (ShiftZ) ? Speed : 0;
  x += (ShiftX) ? Speed : ((ShiftZ) ? Speed / 8 : 0);
  y -= (ShiftY) ? Speed : ((ShiftZ) ? Speed / 16 : 0);
  for (uint8_t i = 0; i < WIDTH; i++) {
    for (uint8_t j = 0; j < HEIGHT; j++) {
      byte noise = inoise8((i * Scale) + x, (j * Scale) + y, z);
      byte Inoise = inoise8((j * Scale) + y, (i * Scale) + x, z);
      leds[XY(i, j)] = ColorFromPalette(palette, noise + hue, (!BriNoise || Inoise > 127) ? 255 : dim8_raw(Inoise * 2));
    }
  }
  if (ShiftHue) {
    hue++;
  } else {
    hue = 0;
  }
}

void Noise3D() {
  if (loadingFlag)
  { setCurrentPalette(palette);
    loadingFlag = false;
  }
  FillNoise(0, 0, 1, *curPalette, (palette == 0 || palette == 7 || palette == 8) ? 1 : 0, 1);
}
