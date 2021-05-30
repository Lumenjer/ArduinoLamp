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
uint8_t step; // какой-нибудь счётчик кадров или постедовательностей операций
uint8_t shiftValue[HEIGHT];
uint8_t shiftHue[HEIGHT];
uint8_t deltaHue, deltaHue2; // ещё пара таких же, когда нужно много
uint8_t deltaValue; // просто повторно используемая переменная
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

#define LIGHTERS_AM ((WIDTH+HEIGHT)/4)
int16_t lightersPos[2][LIGHTERS_AM];
int8_t lightersSpeed[2][LIGHTERS_AM];
byte lightersColor[LIGHTERS_AM];
byte lightersBright[LIGHTERS_AM];
// ========================== КОДЫ ЭФФЕКТОВ ===========================
// ---------------------------- КОНФЕТТИ ------------------------------
void sparklesRoutine()
{
  for (uint8_t i = 0; i < Scale[currentMode]; i++)
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
      drawPixelXY((isVertical ? i : j), (isVertical ? j : i), CHSV((uint8_t)(hue + i * Scale[currentMode] % 170), 255, 255));
    }
  }
}

// ________________________ РАДУГА ДИАГОНАЛЬНАЯ _______________________
void RainbowRoutine()
{
  if (Scale[currentMode] < 85) {
    rainbowHorVertRoutine(false);
  } else if (Scale[currentMode] > 170) {
    rainbowHorVertRoutine(true);
  } else {
    hue += 8;
    for (uint8_t i = 0U; i < WIDTH; i++)
    {
      for (uint8_t j = 0U; j < HEIGHT; j++)
      {
        float twirlFactor = fmap((float)Scale[currentMode], 85, 170, 8.3, 24);      // на сколько оборотов будет закручена матрица, [0..3]
        drawPixelXY(i, j, CHSV((uint8_t)(hue + ((float)WIDTH / HEIGHT * i + j * twirlFactor) * ((float)255 / maxDim)), 255, 255));
      }
    }
  }
}

// --------------------------------- ЦВЕТА -----------------------------
void colorsRoutine() {
  hue += Scale[currentMode];
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(hue, 255, 255);
  }
}

// --------------------------------- ЦВЕТ --------------------------------
void colorRoutine() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(Scale[currentMode], Speed[currentMode], 255);
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
        if (Scale[currentMode] == 1) drawPixelXY(x, HEIGHT - 1U, CHSV(random(0, 9) * 28, 255U, 255U)); // Радужный дождь
        else if (Scale[currentMode] == 255) drawPixelXY(x, HEIGHT - 1U, 0xE0FFFF - 0x101010 * random(0, 4)); // Снег
        else
          drawPixelXY(x, HEIGHT - 1U, CHSV(Scale[currentMode] + random(0, 16), 255, 255)); // Цветной дождь
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
      else if (thisColor == 0U && random((255 - Scale[currentMode]) * HEIGHT) == 0U)  // если наш пиксель ещё не горит, иногда зажигаем новые цепочки
        //else if (thisColor == 0U && random((100 - Scale[currentMode]) * HEIGHT*3) == 0U)  // для длинных хвостов
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
      if (random(255 - Scale[currentMode]) == 0U)
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

    uint8_t fullRows =  centerY / 100.0 * Scale[currentMode];
    uint8_t iPol = (centerY / 100.0 * Scale[currentMode] - fullRows) * 255;

    for (int16_t y = centerY; y >= 0; y--)
    {
      CRGB color = CHSV(
                     45U,                                                                              // определяем тон
                     map(Speed[currentMode], 0U, 255U, 0U, 170U),                                // определяем насыщенность
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
    if (getPixColorXY(0, i) == 0
        && (random(0, SNOW_DENSE) == 0)
        && getPixColorXY(0, i + 1) == 0
        && getPixColorXY(0, i - 1) == 0)
      leds[getPixelNumber(0, i)] = CHSV(random(0, 200), Scale[currentMode], 255);
  }
  for (byte i = 0; i < WIDTH / 2; i++) {
    if (getPixColorXY(i, HEIGHT - 1) == 0
        && (random(0, SNOW_DENSE) == 0)
        && getPixColorXY(i + 1, HEIGHT - 1) == 0
        && getPixColorXY(i - 1, HEIGHT - 1) == 0)
      leds[getPixelNumber(i, HEIGHT - 1)] = CHSV(random(0, 200), Scale[currentMode], 255);
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

    for (uint8_t i = 0U; i < 2U; i++)
    {
      lightersPos[i][0] = WIDTH / 2 * 10;
      lightersSpeed[i][0] = random(4, 10);
    }
    deltaValue = map(Scale[currentMode], 0U, 255U, 2U, max((uint8_t)min(WIDTH, HEIGHT) / 3, 2));
    lightersColor[0] = random(0, 9) * 28;
    //    _pulse_color = CHSV(random(0, 9) * 28, 255U, 255U);
  }

  //  if (!(Scale[currentMode] & 0x01))
  //  {
  //    hue += (Scale[currentMode] - 1U) % 11U * 8U + 1U;

  //    ballColor = CHSV(hue, 255U, 255U);
  //  }

  if ((Scale[currentMode] & 0x01))
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
      if (RANDOM_COLOR) lightersColor[0] = random(0, 9) * 28; // if (RANDOM_COLOR && (Scale[currentMode] & 0x01))
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

  //if (Scale[currentMode] & 0x01)
  //dimAll(135U);
  //dimAll(255U - (Scale[currentMode] - 1U) % 11U * 24U);
  else
    FastLED.clear();

  for (uint8_t i = 0U; i < deltaValue; i++)
    for (uint8_t j = 0U; j < deltaValue; j++)
      drawPixelXY(lightersPos[0][0] / 10 + i, lightersPos[1][0] / 10 + j, CHSV(lightersColor[0], 255, 255));
}
// ------------------------- СВЕТЛЯЧКИ СО ШЛЕЙФОМ ------------------
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
      lightersColor[j] = (Scale[currentMode] * (j + 1)) % 256U;
    }
  }

  dimAll(256U - TRACK_STEP);

  // движение шариков
  for (uint8_t j = 0U; j < map(Scale[currentMode], 1, 255, 1, (WIDTH + HEIGHT) / 4); j++)
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
    drawPixelXY(lightersPos[0][j] / 10, lightersPos[1][j] / 10, CHSV(lightersColor[j], 255, 255));
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
      currentPalette = CRGBPalette16(CRGB::Black, CHSV(Scale[currentMode], 255U, 255U) , CHSV(Scale[currentMode] + 50, 255U, 255U) , CHSV(Scale[currentMode] + 50, 100, 255U));
    }

    for (uint8_t i = 0; i < (WIDTH / 8U); i++) {
      lightersSpeed[1][i] = random8(HEIGHT);
      lightersSpeed[0][i] = random8(WIDTH);
    }
  }
  for (uint8_t i = 0; i < WIDTH; i++) {
    for (uint8_t j = 0; j < HEIGHT; j++) {
      if (Scale[currentMode] == 255)
        leds[XY(i, (HEIGHT - 1) - j)] = ColorFromPalette(WaterfallColors_p, qsub8(inoise8(i * deltaValue, (j + ff_y + random8(2)) * deltaHue, ff_z), shiftHue[j]), 255U);
      else if (Scale[currentMode] == 1)
        leds[XY(i, (HEIGHT - 1) - j)] = ColorFromPalette(HeatColors_p, qsub8(inoise8(i * deltaValue, (j + ff_y + random8(2)) * deltaHue, ff_z), shiftHue[j]), 255U);
      else
        leds[XY(i, HEIGHT - 1U - j)] = ColorFromPalette(currentPalette, qsub8(inoise8(i * deltaValue, (j + ff_y + random8(2)) * deltaHue, ff_z), shiftHue[j]), 255U);
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

byte selX;
byte PosX[WIDTH];
int PosY[WIDTH];
bool sending, sendDirection;
void sendVoxels() { // remade by me
  if (loadingFlag) {
    FastLED.clear();
    for (uint8_t i = 0; i < WIDTH; i++) {
      PosX[i] = i;
      PosY[i] = (random(2) == 1) ? (HEIGHT - 1)*10 : 0;
    }
    loadingFlag = false;
  }
  FastLED.clear();
  for (uint8_t i = 0; i < WIDTH; i++) {
    if (i == selX)
      drawPixelXY(PosX[i], PosY[i] / 10, CHSV(Scale[currentMode], 255, 255));
    else
      leds[XY(PosX[i], PosY[i] / 10)] = CHSV(Scale[currentMode], 255, 255);
    if (!sending) {
      selX = random(0, WIDTH - 1);
      if (PosY[selX] == 0) {
        
        sendDirection = 1;
      } else if (PosY[selX] == (HEIGHT - 1) * 10) {
        sendDirection = 0;
      }
      sending = true;
    } else {
      if (sendDirection == 1) {
        PosY[selX] += 1;
        if (PosY[selX] >= (HEIGHT - 1) * 10) {
          PosY[selX] = (HEIGHT - 1) * 10;
          sending = false;
        }
      } else {
        PosY[selX] -= 1;
        if (PosY[selX] <= 0) {
          PosY[selX] = 0;
          sending = false;
        }
      }
    }
  }
}


// ----------------------- КИПЕНИЕ --------------------
// (c) SottNick
//по мотивам LDIRKO Ленд - эффект номер 10
//...ldir... Yaroslaw Turbin, 18.11.2020
//https://vk.com/ldirko
//https://www.reddit.com/user/ldirko/

void LLandRoutine() {
  if (loadingFlag) {
    loadingFlag = false;
    setCurrentPalette(palette);
    //speedfactor = fmap(Speed[currentMode], 1., 255., 20., 1.) / 16.;
    deltaValue = 10U * ((Scale[currentMode] - 1U) % 11U + 1U);// значения от 1 до 11
    // значения от 0 до 10 = ((Scale[currentMode] - 1U) % 11U)

  }
  hue2 += 32U;
  if (hue2 < 32U)
    hue++;
  //float t = (float)millis() / speedfactor;
  ff_y += 16U;

  for (uint8_t y = 0; y < HEIGHT; y++)
    for (uint16_t x = 0; x < WIDTH; x++)
      //drawPixelXY(x, y, ColorFromPalette (*curPalette, map(inoise8(x * 50, y * 50 - t, 0) - y * 255 / (HEIGHT - 1), 0, 255, 205, 255) + hue, 255));
      drawPixelXY(x, y, ColorFromPalette (*curPalette, map(inoise8(x * deltaValue, y * deltaValue - ff_y, ff_z) - y * 255 / (HEIGHT - 1), 0, 255, 205, 255) + hue, 255));
  ff_z++;
}

void FillNoise(bool ShiftX, bool ShiftY, bool ShiftZ, CRGBPalette16 palette, bool ShiftHue, bool BriNoise) {
  double t = (millis() / (map(Speed, 1, 255, 60, 0)));
  for (byte x = 0; x < WIDTH; x++) {
    for (byte y = 0; y < HEIGHT; y++) {
      uint8_t noise = inoise8((x * Scale[currentMode]) + ((ShiftX) ? t : t / 8), (y * Scale[currentMode]) + ((ShiftY) ? t : t / 16), ((ShiftZ) ? t : 0));
      uint8_t Inoise = inoise8((y * Scale[currentMode]) + ((ShiftY) ? t : t / 16), (x * Scale[currentMode]) + ((ShiftX) ? t : t / 8), ((ShiftZ) ? t : 0));
      leds[XY(x, y)] = ColorFromPalette(palette, noise + hue, (!BriNoise || Inoise > 127) ? 255 : dim8_raw(Inoise * 2));
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
