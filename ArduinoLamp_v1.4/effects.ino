// ================================= ЭФФЕКТЫ ===========================================
// ------------------------------ Конфетти ---------------------------------------------
#define FADE_OUT_SPEED        (70U)                         // скорость затухания
//--------------------------------Шторм,Метель------------------------------------------
#define SNOW_DENSE (32U) // плотность снега - меньше = плотнее
//--------------------------------Светлячки со шлейфом-----------------------------------------
#define BALLS_AMOUNT          (4U)     // количество "шариков"
#define TRACK_STEP            (70U)    // длина хвоста шарика (чем больше цифра, тем хвост короче)
//===================Коды эффектов==============================================

uint8_t deltaHue, deltaHue2; // ещё пара таких же, когда нужно много
uint8_t step; // какой-нибудь счётчик кадров или постедовательностей операций
uint8_t pcnt;
uint8_t line[WIDTH];
uint8_t deltaValue; // просто повторно используемая переменная
uint8_t shiftHue[HEIGHT];
uint8_t shiftValue[HEIGHT];
byte hue, hue2;
CRGB _pulse_color;
extern const TProgmemRGBPalette16 WaterfallColors_p FL_PROGMEM = {0x000000, 0x060707, 0x101110, 0x151717, 0x1C1D22, 0x242A28, 0x363B3A, 0x313634, 0x505552, 0x6B6C70, 0x98A4A1, 0xC1C2C1, 0xCACECF, 0xCDDEDD, 0xDEDFE0, 0xB2BAB9};
static const TProgmemRGBPalette16 ZeebraColors_p FL_PROGMEM = {CRGB::White, CRGB::Black, CRGB::Black, CRGB::Black, CRGB::White, CRGB::Black, CRGB::Black, CRGB::Black, CRGB::White, CRGB::Black, CRGB::Black, CRGB::Black, CRGB::White, CRGB::Black, CRGB::Black, CRGB::Black};
static const TProgmemRGBPalette16 HeatColors2_p FL_PROGMEM = {0x000000, 0x330000, 0x660000, 0x990000, 0xCC0000, 0xFF0000, 0xFF3300, 0xFF6600, 0xFF9900, 0xFFCC00, 0xFFFF00, 0xFFFF33, 0xFFFF66, 0xFFFF99, 0xFFFFCC, 0xFFFFFF};
static const TProgmemRGBPalette16 WoodFireColors_p FL_PROGMEM = {CRGB::Black, 0x330e00, 0x661c00, 0x992900, 0xcc3700, CRGB::OrangeRed, 0xff5800, 0xff6b00, 0xff7f00, 0xff9200, CRGB::Orange, 0xffaf00, 0xffb900, 0xffc300, 0xffcd00, CRGB::Gold};             //* Orange
static const TProgmemRGBPalette16 NormalFire_p FL_PROGMEM = {CRGB::Black, 0x330000, 0x660000, 0x990000, 0xcc0000, CRGB::Red, 0xff0c00, 0xff1800, 0xff2400, 0xff3000, 0xff3c00, 0xff4800, 0xff5400, 0xff6000, 0xff6c00, 0xff7800};                             // пытаюсь сделать что-то более приличное
static const TProgmemRGBPalette16 NormalFire2_p FL_PROGMEM = {CRGB::Black, 0x560000, 0x6b0000, 0x820000, 0x9a0011, CRGB::FireBrick, 0xc22520, 0xd12a1c, 0xe12f17, 0xf0350f, 0xff3c00, 0xff6400, 0xff8300, 0xffa000, 0xffba00, 0xffd400};                      // пытаюсь сделать что-то более приличное
static const TProgmemRGBPalette16 LithiumFireColors_p FL_PROGMEM = {CRGB::Black, 0x240707, 0x470e0e, 0x6b1414, 0x8e1b1b, CRGB::FireBrick, 0xc14244, 0xd16166, 0xe08187, 0xf0a0a9, CRGB::Pink, 0xff9ec0, 0xff7bb5, 0xff59a9, 0xff369e, CRGB::DeepPink};        //* Red
static const TProgmemRGBPalette16 SodiumFireColors_p FL_PROGMEM = {CRGB::Black, 0x332100, 0x664200, 0x996300, 0xcc8400, CRGB::Orange, 0xffaf00, 0xffb900, 0xffc300, 0xffcd00, CRGB::Gold, 0xf8cd06, 0xf0c30d, 0xe9b913, 0xe1af1a, CRGB::Goldenrod};           //* Yellow
static const TProgmemRGBPalette16 CopperFireColors_p FL_PROGMEM = {CRGB::Black, 0x001a00, 0x003300, 0x004d00, 0x006600, CRGB::Green, 0x239909, 0x45b313, 0x68cc1c, 0x8ae626, CRGB::GreenYellow, 0x94f530, 0x7ceb30, 0x63e131, 0x4bd731, CRGB::LimeGreen};     //* Green
static const TProgmemRGBPalette16 AlcoholFireColors_p FL_PROGMEM = {CRGB::Black, 0x000033, 0x000066, 0x000099, 0x0000cc, CRGB::Blue, 0x0026ff, 0x004cff, 0x0073ff, 0x0099ff, CRGB::DeepSkyBlue, 0x1bc2fe, 0x36c5fd, 0x51c8fc, 0x6ccbfb, CRGB::LightSkyBlue};  //* Blue
static const TProgmemRGBPalette16 RubidiumFireColors_p FL_PROGMEM = {CRGB::Black, 0x0f001a, 0x1e0034, 0x2d004e, 0x3c0068, CRGB::Indigo, CRGB::Indigo, CRGB::Indigo, CRGB::Indigo, CRGB::Indigo, CRGB::Indigo, 0x3c0084, 0x2d0086, 0x1e0087, 0x0f0089, CRGB::DarkBlue};        //* Indigo
static const TProgmemRGBPalette16 PotassiumFireColors_p FL_PROGMEM = {CRGB::Black, 0x0f001a, 0x1e0034, 0x2d004e, 0x3c0068, CRGB::Indigo, 0x591694, 0x682da6, 0x7643b7, 0x855ac9, CRGB::MediumPurple, 0xa95ecd, 0xbe4bbe, 0xd439b0, 0xe926a1, CRGB::DeepPink}; //* Violet
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
  &HeatColors2_p,
  &NormalFire2_p,
  &CopperFireColors_p,
  &SodiumFireColors_p,
  &PotassiumFireColors_p,
  &RubidiumFireColors_p,
  &AlcoholFireColors_p,
  &LithiumFireColors_p,
  &NormalFire_p,
  &WoodFireColors_p,
};
const TProgmemRGBPalette16 *curPalette = palette_arr[0];
void setCurrentPalette(uint8_t palIdx) {
  curPalette = palette_arr[palIdx];
}
//// ----------------------------- СВЕТЛЯКИ ------------------------------ в ардуине под него не хватет пямяти
#define LIGHTERS_AM (WIDTH+HEIGHT)
int16_t lightersPos[2][LIGHTERS_AM];
int8_t lightersSpeed[2][LIGHTERS_AM];
byte lightersColor[LIGHTERS_AM];
byte lightersBright[LIGHTERS_AM];
void lightersRoutine() {
  if (loadingFlag) {
    loadingFlag = false;
    randomSeed(millis());
    for (byte i = 0; i < LIGHTERS_AM; i++) {
      lightersPos[0][i] = random(0, WIDTH * 10);
      lightersPos[1][i] = random(0, HEIGHT * 10);
      lightersSpeed[0][i] = random(-10, 10);
      lightersSpeed[1][i] = random(-10, 10);
      lightersColor[i] = random(0, 255);
      lightersBright[i] = random(1U, 3U) * 127U;
    }
  }
  FastLED.clear();
  for (byte i = 0; i < map(Scale, 1, 255, 1, LIGHTERS_AM); i++) {
    EVERY_N_MILLIS(333)
    {
      lightersSpeed[0][i] += random(-3, 4);
      lightersSpeed[1][i] += random(-3, 4);
      lightersSpeed[0][i] = constrain(lightersSpeed[0][i], -10, 10);
      lightersSpeed[1][i] = constrain(lightersSpeed[1][i], -10, 10);
      lightersBright[i] = random(255U - (Scale * 8), 255U);
      if (!random(Scale + 3))
        lightersBright[i] = 127;
    }

    lightersPos[0][i] += lightersSpeed[0][i];
    lightersPos[1][i] += lightersSpeed[1][i];

    if (lightersPos[0][i] < 0) lightersPos[0][i] = (WIDTH - 1) * 10;
    if (lightersPos[0][i] >= WIDTH * 10) lightersPos[0][i] = 0;

    if (lightersPos[1][i] < 0) {
      lightersPos[1][i] = 0;
      lightersSpeed[1][i] = -lightersSpeed[1][i];
    }
    if (lightersPos[1][i] >= (HEIGHT - 1) * 10) {
      lightersPos[1][i] = (HEIGHT - 1) * 10;
      lightersSpeed[1][i] = -lightersSpeed[1][i];
    }
    drawPixelXYF((float)lightersPos[0][i] / 10, (float)lightersPos[1][i] / 10, CHSV(lightersColor[i], 255U - (i * 2), lightersBright[i]));
  }
}
// --------------------------------- конфетти ------------------------------------
void sparklesRoutine()
{
  for (uint8_t i = 0; i < Scale; i++)
  {
    uint8_t x = random8(0U, WIDTH);
    uint8_t y = random8(0U, HEIGHT);
    if (getPixColorXY(x, y) == 0U)
    {
      drawPixelXY(x, y, CHSV(random8(), 255U, 255U));
    }
  }
  fader(FADE_OUT_SPEED);
}

// радуги 2D
// ------------- Pадуга вертикальная/горизонтальная ----------------
void rainbowHorVertRoutine(bool isVertical)
{ hue += 4;
  for (uint8_t i = 0U; i < (isVertical ? WIDTH : HEIGHT); i++)
  {
    CHSV thisColor = CHSV((uint8_t)(hue + i * Scale % 170), 255, 255);
    for (uint8_t j = 0U; j < (isVertical ? HEIGHT : WIDTH); j++)
    {
      drawPixelXY((isVertical ? i : j), (isVertical ? j : i), thisColor);
    }
  }
}

// ------------- Радуга диагональная -------------

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
        CRGB thisColor = CHSV((uint8_t)(hue + ((float)WIDTH / HEIGHT * i + j * twirlFactor) * ((float)255 / maxDim)), 255, 255);
        drawPixelXY(i, j, thisColor);
      }
    }
  }
}


// ---------------------------------------- ЦВЕТА -----------------------------
void colorsRoutine() {
  hue += Speed;
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(hue, Scale, 255);
  }
}

// --------------------------------- ЦВЕТ ------------------------------------
//Настройка насищенности - stepko
void colorRoutine() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(Scale * 2.5, Speed, 255);
  }
}

// ------------------------------ МАТРИЦА ------------------------------
//@kostyamat
void matrixRoutine() {
  if (loadingFlag) {
    randomSeed(millis());
    for (uint8_t i = 0U; i < LIGHTERS_AM; i++)
    {
      lightersPos[0U][i] = random(0, WIDTH * 10);
      lightersPos[1U][i] = random(HEIGHT * 10 - HEIGHT * 5, HEIGHT * 10);
      lightersSpeed[0U][i] = 1;
      lightersSpeed[1U][i] = random(1, 10);
      lightersColor[i] = hue;
      lightersBright[i] = random(196, 255);
    } loadingFlag = false;
  }
  //float speedfactor = (float)Speed / 1048.0f + 0.05f;
  dimAll(map(Speed, 1, 255, 250, 240));

  CHSV color;

  for (uint8_t i = 0U; i < map(Scale, 1, 255, 1, LIGHTERS_AM); i++)
  {
    lightersPos[1U][i] -= lightersSpeed[1U][i];

    if(Scale%2) {
      color = CHSV(++hue, 255, lightersBright[i]);
    } else {
      color = CHSV(89, 255, lightersBright[i]);
    }


    drawPixelXY(lightersPos[0U][i] / 10, lightersPos[1U][i] / 10, color);

    if (palette > 1)
      if (random8() < palette) {
        lightersPos[0U][i] = lightersPos[0U][i] + random(10, 20);
        lightersBright[i] = random(196, 255);
      }

    if (lightersPos[1U][i] < -1) {
      lightersPos[0U][i] = random(0, WIDTH*10);
      lightersPos[1U][i] = random(HEIGHT * 10 - HEIGHT * 5, HEIGHT * 10);
      lightersSpeed[1U][i] = random(1, 10);
      lightersBright[i] = random(127U, 255U);
      lightersColor[i] = hue;
    }
  }
}



// ------------- белый свет (светится горизонтальная полоса по центру лампы; масштаб - высота центральной горизонтальной полосы; скорость - регулировка от холодного к тёплому; яркость - общая яркость) -------------
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

// ------------- звездопад/метель -------------
void blurScreen(fract8 blur_amount, CRGB *LEDarray = leds)
{
  blur2d(LEDarray, WIDTH, HEIGHT, blur_amount);
}

void stormRoutine() {
  // заполняем головами комет левую и верхнюю линию
  for (byte i = HEIGHT / 2; i < HEIGHT; i++) {
    if (getPixColorXY(0, i) == 0
        && (random(0, SNOW_DENSE) == 0)
        && getPixColorXY(0, i + 1) == 0
        && getPixColorXY(0, i - 1) == 0)
      leds[getPixelNumber(0, i)] = CHSV(random(0, 200), Scale,  255);
  }
  for (byte i = 0; i < WIDTH / 2; i++) {
    if (getPixColorXY(i, HEIGHT - 1) == 0
        && (random(0, SNOW_DENSE) == 0)
        && getPixColorXY(i + 1, HEIGHT - 1) == 0
        && getPixColorXY(i - 1, HEIGHT - 1) == 0)
      leds[getPixelNumber(i, HEIGHT - 1)] = CHSV(random(0, 200), Scale, 255);
  }

  // сдвигаем по диагонали
  for (byte y = 0; y < HEIGHT - 1; y++) {
    for (byte x = WIDTH - 1; x > 0; x--) {
      drawPixelXY(x, y, getPixColorXY(x - 1, y + 1));
    }
  }

  // уменьшаем яркость левой и верхней линии, формируем "хвосты"
  for (byte i = HEIGHT / 2; i < HEIGHT; i++) {
    fadePixel(0, i, 120);
  }
  for (byte i = 0; i < WIDTH / 2; i++) {
    fadePixel(i, HEIGHT - 1, 70);
  }
}

// ------------- блуждающий кубик -------------
#define RANDOM_COLOR          (1U)                          // случайный цвет при отскоке
//int8_t deltaValue; //ballSize;

void ballRoutine()
{
  if (loadingFlag)
  {
    loadingFlag = false;
    //FastLED.clear();

    for (uint8_t i = 0U; i < 2U; i++)
    {
      lightersPos[i][0] = WIDTH / 2 * 10;
      lightersSpeed[i][0] = random(4, 10);
    }
    deltaValue = map(Scale * 2.55, 0U, 255U, 2U, max((uint8_t)min(WIDTH, HEIGHT) / 3, 2));
    lightersColor[0] = random(0, 9) * 28;
    //    _pulse_color = CHSV(random(0, 9) * 28, 255U, 255U);
  }

  //  if (!(modes[currentMode].Scale & 0x01))
  //  {
  //    hue += (modes[currentMode].Scale - 1U) % 11U * 8U + 1U;

  //    ballColor = CHSV(hue, 255U, 255U);
  //  }

  if ((Scale & 0x01))
    for (uint8_t i = 0U; i < deltaValue; i++)
      for (uint8_t j = 0U; j < deltaValue; j++)
        leds[XY(lightersPos[0][0] / 10 + i, lightersPos[1][0] / 10 + j)] = _pulse_color;

  for (uint8_t i = 0U; i < 2U; i++)
  {
    lightersPos[i][0] += lightersSpeed[i][0];
    if (lightersPos[i][0] < 0)
    {
      lightersPos[i][0] = 0;
      lightersSpeed[i][0] = -lightersSpeed[i][0];
      if (RANDOM_COLOR) lightersColor[0] = random(0, 9) * 28; // if (RANDOM_COLOR && (modes[currentMode].Scale & 0x01))
      //vectorB[i] += random(0, 6) - 3;
    }
  }
  if (lightersPos[0][0] > (int16_t)((WIDTH - deltaValue) * 10))
  {
    lightersPos[0][0] = (WIDTH - deltaValue) * 10;
    lightersSpeed[0][0] = -lightersSpeed[0][0];
    if (RANDOM_COLOR) lightersColor[0] = random(0, 9) * 28;
    //vectorB[0] += random(0, 6) - 3;
  }
  if (lightersPos[1][0] > (int16_t)((HEIGHT - deltaValue) * 10))
  {
    lightersPos[1][0] = (HEIGHT - deltaValue) * 10;
    lightersSpeed[1][0] = -lightersSpeed[1][0];
    if (RANDOM_COLOR) lightersColor[0] = random(0, 9) * 28;
    //vectorB[1] += random(0, 6) - 3;
  }

  //  if (modes[currentMode].Scale & 0x01)
  //    dimAll(135U);
  //    dimAll(255U - (modes[currentMode].Scale - 1U) % 11U * 24U);
  //  else
  FastLED.clear();

  for (uint8_t i = 0U; i < deltaValue; i++)
    for (uint8_t j = 0U; j < deltaValue; j++)
      drawPixelXYF((float)lightersPos[0][0] / 10 + i, (float)lightersPos[1][0] / 10 + j, CHSV(lightersColor[0], 255, 255));
}

//-----------------Светлячки со шлейфом----------------------------------
void ballsRoutine()
{
  if (loadingFlag)
  {
    loadingFlag = false;

    for (uint8_t j = 0U; j < (WIDTH + HEIGHT) / 4; j++)
    {
      int8_t sign;
      // забиваем случайными данными
      lightersPos[0][j] = WIDTH / 2 * 10;
      random(0, 2) ? sign = 1 : sign = -1;
      lightersSpeed[0][j] = random(4, 15) * sign;
      lightersPos[1][j] = HEIGHT / 2 * 10;
      random(0, 2) ? sign = 1 : sign = -1;
      lightersSpeed[1][j] = random(4, 15) * sign;
      //ballColors[j] = CHSV(random(0, 9) * 28, 255U, 255U);
      // цвет зависит от масштаба
      lightersColor[j] = (Scale * (j + 1)) % 256U;
    }
  }

  dimAll(256U - TRACK_STEP);

  // движение шариков
  for (uint8_t j = 0U; j < map(Scale, 1, 255, 1, (WIDTH + HEIGHT) / 4); j++)
  {
    // движение шариков
    for (uint8_t i = 0U; i < 2U; i++)
    {
      lightersPos[i][j] += lightersSpeed[i][j];
      if (lightersPos[i][j] < 0)
      {
        lightersPos[i][j] = 0;
        lightersSpeed[i][j] = -lightersSpeed[i][j];
      }
    }

    if (lightersPos[0][j] > (int16_t)((WIDTH - 1) * 10))
    {
      lightersPos[0][j] = (WIDTH - 1) * 10;
      lightersSpeed[0][j] = -lightersSpeed[0][j];
    }
    if (lightersPos[1][j] > (int16_t)((HEIGHT - 1) * 10))
    {
      lightersPos[1][j] = (HEIGHT - 1) * 10;
      lightersSpeed[1][j] = -lightersSpeed[1][j];
    }
    drawPixelXYF((float)lightersPos[0][j] / 10, (float)lightersPos[1][j] / 10, CHSV(lightersColor[j], 255, 255));
  }
}

// ------------------Узоры-----------------------
// https://github.com/vvip-68/GyverPanelWiFi/blob/master/firmware/GyverPanelWiFi_v1.02/patterns.ino
uint8_t patternIdx = -1;
int8_t lineIdx = 0;
bool dir = true;


CHSV colorMR[5] = {
  CHSV(0, 0, 0),              // 0 - Black
  CHSV(HUE_RED, 255, 255),    // 1 - Red
  CHSV(0, 0, 220),            // 2 - White
  CHSV(0, 255, 255),              // 3 - плавно меняеться в цикле (фон)
  CHSV(0, 255, 255),              // 4 - цвет равен 6 но +64
};

#include "Patterns.h"

void drawPattern(uint8_t ptrn, uint8_t X, uint8_t Y, uint8_t W, uint8_t H, bool dir) {


  if (dir)
    shiftDown();
  else
    shiftUp();


  uint8_t y = dir ? (HEIGHT - 1) : 0;

  // Если ширина паттерна не кратна ширине матрицы - отрисовывать со сдвигом? чтобы рисунок был максимально по центру
  int8_t offset_x = -((WIDTH % W) / 2) + 3;

  for (uint8_t x = 0; x < WIDTH + W; x++) {
    int8_t xx = offset_x + x;
    if (xx >= 0 && xx < (int8_t)WIDTH) {
      uint8_t in = (uint8_t)pgm_read_byte(&(patterns[ptrn][lineIdx][x % 10]));
      CHSV color = colorMR[in];
      CHSV color2 = color.v != 0 ? CHSV(color.h, color.s, 255) : color;
      drawPixelXY(xx, y, color2);
    }
  }
  if (dir) {
    lineIdx = (lineIdx > 0) ? (lineIdx - 1) : (H - 1);
  } else {
    lineIdx = (lineIdx < H - 1) ? (lineIdx + 1) : 0;
  }
}



// Отрисовка указанной картинки с размерами WxH в позиции XY
void drawPicture_XY(uint8_t iconIdx, uint8_t X, uint8_t Y, uint8_t W, uint8_t H) {
  if (loadingFlag) {
    loadingFlag = false;
  }

  for (byte x = 0; x < W; x++) {
    for (byte y = 0; y < H; y++) {
      uint8_t in = (uint8_t)pgm_read_byte(&(patterns[iconIdx][y][x]));
      if (in != 0) {
        CHSV color = colorMR[in];
        CHSV color2 = color.v != 0 ? CHSV(color.h, color.s, 255) : color;
        drawPixelXY(X + x, Y + H - y, color2);
      }
    }
  }
}
void patternsRoutine() {


  if (loadingFlag) {
    loadingFlag = false;
    patternIdx = map(Scale, 1U, MAX_PATTERN + 1, -1, MAX_PATTERN);  // мапим к ползунку масштаба
    if (patternIdx < 0) {
      patternIdx = random8(0U, MAX_PATTERN);
    }
    //fadeToBlackBy(leds, NUM_LEDS, 25);
    if (dir)
      lineIdx = 9;         // Картинка спускается сверху вниз - отрисовка с нижней строки паттерна (паттерн 10x10)
    else
      lineIdx = 0;         // Картинка поднимается сверху вниз - отрисовка с верхней строки паттерна
    // Цвета с индексом 6 и 7 - случайные, определяются в момент настройки эффекта
    colorMR[3] = CHSV(random8(), 255U, 255U);
    colorMR[4].hue = colorMR[3].hue + 96; //(beatsin8(1, 0, 255, 0, 127), 255U, 255U);
  }
  drawPattern(patternIdx, 0, 0, 10, 10, dir);
  EVERY_N_MILLIS((1005000U / Speed)) {
    if (Scale == 1)
      dir = !dir;
    loadingFlag = true;
  }
}

//---------------Лаволампа------------------------------
//Основа @SottNick
//Оптимизация @Stepko
float ball[(WIDTH / 2) -  ((WIDTH - 1) & 0x01)][3];
void drawBlob(uint8_t l, CRGB color) { //раз круги нарисовать не получается, будем попиксельно вырисовывать 2 варианта пузырей
  if (lightersSpeed[0][l] == 2)
  {
    for (int8_t x = -2; x < 3; x++)
      for (int8_t y = -2; y < 3; y++)
        if (abs(x) + abs(y) < 4)
          drawPixelXYF(fmod(ball[l][1] + x + WIDTH, WIDTH), ball[l][0] + y, color);
  }
  else
  {
    for (int8_t x = -1; x < 3; x++)
      for (int8_t y = -1; y < 3; y++)
        if (!(x == -1 && (y == -1 || y == 2) || x == 2 && (y == -1 || y == 2)))
          drawPixelXYF(fmod(ball[l][1] + x + WIDTH, WIDTH), ball[l][0] + y, color);
  }
}

void LavaLampRoutine() {
  if (loadingFlag)
  { for (byte i = 0; i < (WIDTH / 2) -  ((WIDTH - 1) & 0x01); i++) {
      lightersSpeed[0][i] = random(1, 3);
      ball[i][2] = (float)random8(5, 11) / (Speed) / 4.0;
      ball[i][0] = 0;
      ball[i][1] = i * 2U + random8(2);
      if ( ball[i][2] == 0)
        ball[i][2] = 1;
    }
    loadingFlag = false;
    setCurrentPalette(map(palette, 1, 255, 1, 20));
  }
  dimAll(100);
  blurScreen(20);
  for (byte i = 0; i < (WIDTH / 2) -  ((WIDTH - 1) & 0x01); i++) {
    // Draw 'ball'
    if (palette <= 255)
      drawBlob(i, ColorFromPalette(*curPalette, ball[i][0] * 16));
    else
      drawBlob(i, CHSV(Scale, 255, 255));

    if (ball[i][0] + lightersSpeed[0][i] >= HEIGHT - 1)
      ball[i][0] += (ball[i][2] * ((HEIGHT - 1 - ball[i][0]) / lightersSpeed[0][i] + 0.005));
    else if (ball[i][0] - lightersSpeed[0][i] <= 0)
      ball[i][0] += (ball[i][2] * (ball[i][0] / lightersSpeed[0][i] + 0.005));
    else
      ball[i][0] += ball[i][2];
    if (ball[i][0] < 0.01) {                  // почему-то при нуле появляется мерцание (один кадр, еле заметно)
      ball[i][2] = (float)random8(5, 11) / (257U - Speed) / 4.0;
      ball[i][0] = 0.01;
    }
    else if (ball[i][0] > HEIGHT - 1.01) {    // тоже на всякий пожарный
      ball[i][2] = (float)random8(5, 11) / (257U - Speed) / 4.0;
      ball[i][2] = -ball[i][2];
      ball[i][0] = HEIGHT - 1.01;
    }
  }
}


// ============= Огонь 2020 ===============
// (c) SottNick
//сильно по мотивам https://pastebin.com/RG0QGzfK
//Perlin noise fire procedure by Yaroslaw Turbin
//https://www.reddit.com/r/FastLED/comments/hgu16i/my_fire_effect_implementation_based_on_perlin/
uint16_t ff_y, ff_z;// используем для сдвига нойза переменные из общих
void fireRoutine() {
  if (loadingFlag) {
    setCurrentPalette(map(palette, 1, 255, 10, 20));
    loadingFlag = false;
    deltaValue = Scale * 0.0899;// /100.0F * ((sizeof(palette_arr) /sizeof(TProgmemRGBPalette16 *))-0.01F));
    deltaValue = (((Scale - 1U) % 11U + 1U) << 4U) - 8U; // ширина языков пламени (масштаб шума Перлина)
    deltaHue = map(deltaValue, 8U, 168U, 8U, 84U); // высота языков пламени должна уменьшаться не так быстро, как ширина
    step = map(255U - deltaValue, 87U, 247U, 4U, 32U); // вероятность смещения искорки по оси ИКС
    for (uint8_t j = 0; j < HEIGHT; j++) {
      shiftHue[j] = (HEIGHT - 1 - j) * 255 / (HEIGHT - 1); // init colorfade table
    }

    for (uint8_t i = 0; i < (WIDTH / 8U); i++) {
      lightersSpeed[1][i] = random8(HEIGHT);
      lightersSpeed[0][i] = random8(WIDTH);
    }
  }
  for (uint8_t i = 0; i < WIDTH; i++) {
    for (uint8_t j = 0; j < HEIGHT; j++) {
      leds[XY(i, HEIGHT - 1U - j)] = ColorFromPalette(*curPalette, qsub8(inoise8(i * deltaValue, (j + ff_y + random8(2)) * deltaHue, ff_z), shiftHue[j]), 255U);
    }
  }

  //вставляем искорки из отдельного массива
  for (uint8_t i = 0; i < (WIDTH / 8U); i++) {
    if (lightersSpeed[1][i] > 3U) {
      leds[XY(lightersSpeed[0][i], lightersSpeed[1][i])] = leds[XY(lightersSpeed[0][i], 3U)];
      leds[XY(lightersSpeed[0][i], lightersSpeed[1][i])].fadeToBlackBy( lightersSpeed[1][i] * 2U );
    }
    lightersSpeed[1][i]++;
    if (lightersSpeed[1][i] >= HEIGHT) {
      lightersSpeed[1][i] = random8(4U);
      lightersSpeed[0][i] = random8(WIDTH);
    }
    if (!random8(step))
      lightersSpeed[0][i] = (WIDTH + lightersSpeed[0][i] + 1U - random8(3U)) % WIDTH;
  }
  ff_y++;
  if (ff_y & 0x01)
    ff_z++;
}

// ---------- Эффект "Тикси Ленд"
byte animation = 0;// (c)  Martin Kleppe @aemkei, https://github.com/owenmcateer/tixy.land-display
void TLandRoutine() {

  double t = (double)millis() / map(Speed, 1, 255, 1200, 128);
  hue++;
  for ( double x = 0; x < WIDTH; x++) {
    for ( double y = 0; y < HEIGHT; y++) {
      processFrame(leds, t, x, y);
    }
  }
  if (Scale == 0) {
    EVERY_N_SECONDS(60) {
      animation++;
    }
  } else {
    animation = Scale;
  }
}
void processFrame(CRGB *leds, double t, double x, double y) {
  double i = (y * 16) + x;
  double frame = constrain(code(t, i, x, y), -1, 1) * 255;

  if (frame >= 0) {
    leds[getPixelNumber(x, HEIGHT - 1 - y)] = CHSV(hue, frame, frame);
  }
  else {
    leds[getPixelNumber(x, HEIGHT - 1 - y)] = CHSV(hue + 64, abs(frame), abs(frame));
  }
}

float code(double t, double i, double x, double y) {
  //float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

  switch (animation) {
    case 1:
      //Plasma
      return sin(x + t) + sin(y + t) + sin(x + y + t) / 3;
      break;
    case 2:
      // Up&Down
      return sin(cos(x) * y / 8 + t);
      break;

    case 3:
      return sin(y * (t / 4.)) * cos(x * (t / 4.));
      break;

    case 4:
      return sin(i / 5 + (t));
      break;

    case 5:
      return sin(cos(y) * t) * cos(sin(x) * t);
      break;


    /**
       tixy.land website
    */
    case 6:
      // Emitting rings
      return sin(t - sqrt(pow((x - 7.5), 2) + pow((y - 6), 2)));
      break;

    case 7:
      // Rotation
      return sin(PI * 2 * atan((y - 8) / (x - 8)) + 5 * t);
      break;

    case 8:
      // Vertical fade
      return sin(y / 8 + t);
      break;

    case 9:
      // Smooth noise
      return cos(t + i + x * y);
      break;

    case 10:
      // Waves
      return sin(x / 2) - sin(x - t * 4) - y + 6;
      break;

    case 11:
      // Drop
      return fmod(8 * t, 13) - hypot(x - 7.5, y - 7.5);
      break;



    case 12:
      // Ripples @thespite
      return sin(t - sqrt(x * x + y * y));
      break;

    case 13:
      // Bloop bloop bloop @v21
      return  (x - 8) * (y - 8) - sin(t / 2) * 64;;
      break;


    /**
       Reddit
    */
    case 14:
      // lurkerurke https://www.reddit.com/r/programming/comments/jpqbux/minimal_16x16_dots_coding_environment/gbgcwsn/
      return sin((x - 7.5) * (y - 7.5) / 5 * t + t);
      break;

    case 15:
      // SN0WFAKER https://www.reddit.com/r/programming/comments/jpqbux/minimal_16x16_dots_coding_environment/gbgk7c0/
      return sin(atan((y - 7.5) / (x - 7.5)) + t);
      break;

    case 16:
      // LeadingNegotiation9 https://www.reddit.com/r/programming/comments/jpqbux/minimal_16x16_dots_coding_environment/gbjcoho/
      return pow(cos(((int)y ^ (int)x) + t), cos((x > y) + t));
      break;

    case 17:
      // detunized https://www.reddit.com/r/programming/comments/jpqbux/minimal_16x16_dots_coding_environment/gbgk30l/
      return sin(y / 8 + t * 0.5) + x / 16 - 0.5;
      break;

    case 18:
      // Andres_A https://www.reddit.com/r/programming/comments/jpqbux/minimal_16x16_dots_coding_environment/gbgzdnj/
      return 1 - hypot(sin(t) * 9 - x, cos(t) * 9 - y) / 9;
      break;


    /**
       @akella
       https://twitter.com/akella/status/1323549082552619008
    */
    case 19:
      return sin(6 * atan2(y - 8, x) + t);
      break;


    /**
       Paul Malin
       https://twitter.com/P_Malin/
    */
    case 20:
      // Parallax Scrolling Checkerboard https://twitter.com/P_Malin/status/1323609539648905218
      return ((int)((x - 8) / y + t * 2) & 1 ^ (int)(1 / y * 8) & 1) * y / 5;
      break;

    case 21:
      // Matrix Rain https://twitter.com/P_Malin/status/1323583013880553472
      return 1 - fmod((x * x - y + t * (fmod(1 + x * x, 5.0)) * 3.0), 16.0) / 16.0;
      break;

    case 22:
      // Burst https://twitter.com/P_Malin/status/1323605999274594304
      return -10. / ((x - 8.) * (x - 8.) + (y - 8.) * (y - 8.) - fmod(t * 0.3, 0.7) * 200.);
      break;

    case 23:
      // Rays
      return sin(atan2(x, y) * 5 + t * 2);
      break;

    case 24:
      // Starfield https://twitter.com/P_Malin/status/1323702220320313346
      return !((int)(x + (t / 2.) * 50. / (fmod(y * y, 5.9) + 1.)) & 15) / (fmod(y * y, 5.9) + 1.);
      break;

    default:
      animation = 1;
      return sin(x + t) + sin(y + t) + sin(x + y + t) / 3;
      break;
  }
}
//-------------Дождь---------------------------
bool Lightning(uint8_t chanse)
{
  //uint8_t lightning[WIDTH][HEIGHT];
  // ESP32 does not like static arrays  https://github.com/espressif/arduino-esp32/issues/2567
  if (random16() < chanse)
  {
    uint8_t *lightning = (uint8_t *)malloc(WIDTH * HEIGHT);                                                           // Odds of a lightning bolt
    lightning[scale8(random8(), WIDTH - 1) + (HEIGHT - 1) * WIDTH] = 255; // Random starting location
    for (uint8_t ly = HEIGHT - 1; ly > 1; ly--)
    {
      for (uint8_t lx = 1; lx < WIDTH - 1; lx++)
      {
        if (lightning[lx + ly * WIDTH] == 255)
        {
          lightning[lx + ly * WIDTH] = 0;
          uint8_t dir = random8(4);
          switch (dir)
          {
            case 0:
              setLed(getPixelNumber(lx + 1, ly - 1), CHSV(30, 90, 255));
              lightning[(lx + 1) + (ly - 1) * WIDTH] = 255; // move down and right
              break;
            case 1:
              setLed(getPixelNumber(lx, ly - 1), CHSV(30, 90, 255)); // я без понятия, почему у верхней молнии один оттенок, а у остальных - другой
              lightning[lx + (ly - 1) * WIDTH] = 255;                                 // move down
              break;
            case 2:
              setLed(getPixelNumber(lx - 1, ly - 1), CHSV(30, 90, 255));
              lightning[(lx - 1) + (ly - 1) * WIDTH] = 255; // move down and left
              break;
            case 3:
              setLed(getPixelNumber(lx - 1, ly - 1), CHSV(30, 90, 255));
              lightning[(lx - 1) + (ly - 1) * WIDTH] = 255; // fork down and left
              setLed(getPixelNumber(lx - 1, ly - 1), CHSV(30, 90, 255));
              lightning[(lx + 1) + (ly - 1) * WIDTH] = 255; // fork down and right
              break;
          }
        }
      }
    }

    //free(lightning);
    return true;
  }
  return false;
}

// Функция рисует тучу в верхней части матрицы
void Clouds(bool flash)
{
  const CRGBPalette16 rainClouds_p(0x000000, 0x333C3C, 0x2D3333, 0xB5B5B5);
  //uint32_t random = millis();
  uint8_t dataSmoothing = 50; //196
  uint16_t noiseX = beatsin16(1, 10, 4000, 0, 150);
  uint16_t noiseY = beatsin16(1, 1000, 10000, 0, 50);
  uint16_t noiseZ = beatsin16(1, 10, 4000, 0, 100);
  uint16_t noiseScale = 50; // A value of 1 will be so zoomed in, you'll mostly see solid colors. A value of 4011 will be very zoomed out and shimmery
  const uint8_t cloudHeight = (HEIGHT * 0.2) + 1;

  // This is the array that we keep our computed noise values in
  //static uint8_t noise[WIDTH][cloudHeight];
  static uint8_t *noise = (uint8_t *)malloc(WIDTH * cloudHeight);
  for (uint8_t x = 0; x < WIDTH; x++)
  {
    int xoffset = noiseScale * x;

    for (int z = 0; z < cloudHeight; z++)
    {
      int yoffset = noiseScale * z;
      uint8_t noiseData = qsub8(inoise8(noiseX + xoffset, noiseY + yoffset, noiseZ), 16);
      noiseData = qadd8(noiseData, scale8(noiseData, 39));
      noise[x * cloudHeight + z] = scale8(noise[x * cloudHeight + z], dataSmoothing) + scale8(noiseData, 256 - dataSmoothing);
      if (flash)
        drawPixelXY(x, HEIGHT - z - 1, CHSV(random8(20, 30), 250, random8(64, 100)));
      nblend(getUnsafeLedsArray()[getPixelNumber(x, HEIGHT - z - 1)], ColorFromPalette(rainClouds_p, noise[x * cloudHeight + z], noise[x * cloudHeight + z]), (500 / cloudHeight));
    }
    noiseZ++;
  }
  if (flash) {
    for (uint16_t i = 0; i < WIDTH; i++)
    {
      for (byte z = 0; z < 10; z++)
        drawPixelXYF(i, randomf((float)HEIGHT - 4., (float)HEIGHT - 1.), CHSV(0, 250, random8(120, 200)));
    }
    blurScreen(100);
  }
}

void RainRoutine() {
  if (loadingFlag) {
    randomSeed(millis());
    for (uint8_t i = 0U; i < LIGHTERS_AM; i++)
    {
      lightersPos[0U][i] = random8(0, WIDTH * 10);
      lightersPos[1U][i] = random8((HEIGHT - 4) * 10, (HEIGHT + 4) * 10);
      lightersSpeed[0U][i] = 1;
      lightersSpeed[1U][i] = random8(5, 10);
      lightersColor[i] = 130;
      lightersBright[i] = random8(200, 255);
      loadingFlag = false;
    }
  }
  FastLED.clear();

  for (uint8_t i = 0U; i < map(Scale, 1, 255, 1, LIGHTERS_AM); i++)
  {
    lightersPos[1U][i] -= lightersSpeed[1U][i];
    lightersSpeed[0U][i] = beatsin88(Speed, 0, 20) - 10;
    lightersPos[0U][i] -= lightersSpeed[0U][i];
    drawPixelXY(lightersPos[0U][i] / 10, lightersPos[1U][i] / 10, CHSV(lightersColor[i], 100, lightersBright[i]));

    if (lightersPos[1U][i] < 0) {
      lightersPos[0U][i] = random(0, WIDTH * 10);
      lightersPos[1U][i] = HEIGHT * 10;
      lightersSpeed[1U][i] = random(5, 10);
      lightersBright[i] = random8(200, 255);
    } //(float)random(15, 25) / 10.0f;
    if (lightersPos[0][i] < 0) {
      lightersPos[0][i] = WIDTH * 10;
    }
    if (lightersPos[0][i] > WIDTH * 10) {
      lightersPos[0][i] = 0;
    }

  }
  switch (map(palette, 1, 255, 1, 4)) {
    case 1:
      break;
    case 2:
      Clouds(false);
      Lightning(200);
      break;
    case 3:
      Clouds((Lightning(200)));
      break;
    case 4:
      Lightning(200);
      break;
  }
}
