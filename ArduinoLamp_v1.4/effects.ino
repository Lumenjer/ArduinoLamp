// ================================= ЭФФЕКТЫ =====================================
// --------------------------------- конфетти ------------------------------------
#define FADE_OUT_SPEED        (70U)                         // скорость затухания
//--------------------------Шторм,Метель------------------------------------------
#define SNOW_DENSE (32U) // плотность снега - меньше = плотнее
#define TAIL_STEP (127U) // длина хвоста
//-------------------Светлячки со шлейфом-----------------------------------------
#define BALLS_AMOUNT          (3U)     // количество "шариков"
#define CLEAR_PATH            (1U)     // очищать путь
#define TRACK_STEP            (70U)    // длина хвоста шарика (чем больше цифра, тем хвост короче)
int16_t coord[BALLS_AMOUNT][2U];
int8_t vector[BALLS_AMOUNT][2U];
CRGB ballColors[BALLS_AMOUNT];
//===================Коды эффектов==============================================
uint8_t deltaHue, deltaHue2; // ещё пара таких же, когда нужно много
uint8_t deltaValue; // просто повторно используемая переменная
byte hue, hue2;
CRGB _pulse_color;
void blurScreen(fract8 blur_amount, CRGB *LEDarray = leds)
{
  blur2d(LEDarray, WIDTH, HEIGHT, blur_amount);
}
extern const TProgmemRGBPalette16 WaterfallColors_p FL_PROGMEM = {0x000000, 0x060707, 0x101110, 0x151717, 0x1C1D22, 0x242A28, 0x363B3A, 0x313634, 0x505552, 0x6B6C70, 0x98A4A1, 0xC1C2C1, 0xCACECF, 0xCDDEDD, 0xDEDFE0, 0xB2BAB9};
extern const TProgmemRGBPalette16 ZeebraColors_p FL_PROGMEM = {CRGB::White, CRGB::Black, CRGB::Black, CRGB::Black, CRGB::White, CRGB::Black, CRGB::Black, CRGB::Black, CRGB::White, CRGB::Black, CRGB::Black, CRGB::Black, CRGB::White, CRGB::Black, CRGB::Black, CRGB::Black};
const TProgmemRGBPalette16 *palette_arr[] = {
  &PartyColors_p,
  &OceanColors_p,
  &LavaColors_p,
  &HeatColors_p,
  &ZeebraColors_p,
  &WaterfallColors_p,
  &CloudColors_p,
  &ForestColors_p,
  &RainbowColors_p,
  &RainbowStripeColors_p,
};
const TProgmemRGBPalette16 *curPalette = palette_arr[0];
void setCurrentPalette(uint8_t palIdx) {
  curPalette = palette_arr[palIdx];
}
// --------------------------------- конфетти ------------------------------------
void sparklesRoutine()
{
  for (uint8_t i = 0; i < Scale; i++)
  {
    uint8_t x = random(0U, WIDTH);
    uint8_t y = random(0U, HEIGHT);
    if (getPixColorXY(x, y) == 0U)
    {
      drawPixelXY(x, y,CHSV(random(0U, 255U), 255U, 255U));
    }
  }
  fader(FADE_OUT_SPEED);
}
// функция плавного угасания цвета для всех пикселей
void fader(uint8_t step)
{
  for (uint8_t i = 0U; i < WIDTH; i++)
  {
    for (uint8_t j = 0U; j < HEIGHT; j++)
    {
      fadePixel(i, j, step);
    }
  }
}
void fadePixel(uint8_t i, uint8_t j, uint8_t step)          // новый фейдер
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

// радуги 2D
// ------------- Pадуга вертикальная/горизонтальная ----------------
void rainbowHorVertRoutine(bool isVertical)
{
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
  // коэф. влияния замаплен на скорость, 4 ползунок нафиг не нужен
  hue += (6.0 * (Speed / 255.0) + 0.05 ); // скорость смещения цвета зависит от кривизны наклна линии, коэф. 6.0 и 0.05

  if (Scale < 85) {
    rainbowHorVertRoutine(false);
  } else if (Scale > 170) {
    rainbowHorVertRoutine(true);
  } else {

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
  hue += Scale;
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(hue, 255, 255);
  }
}

// --------------------------------- ЦВЕТ ------------------------------------
void colorRoutine() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(Scale * 2.5, Speed * 2.5, 255);
  }
}

// ------------------------------ снегопад 2.0 --------------------------------
void snowRoutine() {
  shiftDown();

  for (byte x = 0; x < WIDTH; x++) {
    // заполняем случайно верхнюю строку
    // а также не даём двум блокам по вертикали вместе быть
    if (getPixColorXY(x, HEIGHT - 2) == 0 && (random(0, Scale) == 0))
      drawPixelXY(x, HEIGHT - 1U, 0xE0FFFF - 0x101010 * random(0, 4));
    else
      drawPixelXY(x, HEIGHT - 1, 0x000000);
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
      else if (thisColor == 0U && random((100 - Scale) * HEIGHT) == 0U)  // если наш пиксель ещё не горит, иногда зажигаем новые цепочки
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
      if (random(100 - Scale) == 0U)
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

//--------------------------Шторм,Метель-------------------------
void stormRoutine() {
  // заполняем головами комет левую и верхнюю линию
  for (byte i = HEIGHT / 2; i < HEIGHT; i++) {
    if (getPixColorXY(0, i) == 0
        && (random(0, SNOW_DENSE) == 0)
        && getPixColorXY(0, i + 1) == 0
        && getPixColorXY(0, i - 1) == 0)
      leds[getPixelNumber(0, i)] = CHSV(random(0, 200), Scale, 255);
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
    fadePixel(i, HEIGHT - 1, TAIL_STEP);
  }
}

//-------------------------Блуждающий кубик-----------------------
int16_t coordB[2U];
int8_t vectorB[2U];
CRGB ballColor;

void ballRoutine()
{
  if (loadingFlag)
  {
    loadingFlag = false;
  
  if (Scale <= 85) 
    deltaValue = map(Scale, 1, 85, 1U, max((uint8_t)min(WIDTH,HEIGHT) / 3, 1));
  else if (Scale > 85 and Scale <= 170)
    deltaValue = map(Scale, 170, 86, 1U, max((uint8_t)min(WIDTH,HEIGHT) / 3, 1));
  else
    deltaValue = map(Scale, 171, 255, 1U, max((uint8_t)min(WIDTH,HEIGHT) / 3, 1));
    
    for (uint8_t i = 0U; i < 2U; i++)
    {
      coordB[i] = WIDTH / 2 * 10;
      vectorB[i] = random(8, 20);
    }
    ballColor = CHSV(random(0, 9) * 28, 255U, 255U);
  }

 
  if ((Scale & 0x01))
    for (uint8_t i = 0U; i < deltaValue; i++)
      for (uint8_t j = 0U; j < deltaValue; j++)
        leds[XY(coordB[0U] / 10 + i, coordB[1U] / 10 + j)] = _pulse_color;

  for (uint8_t i = 0U; i < 2U; i++)
  {
    coordB[i] += vectorB[i];
    if (coordB[i] < 0)
    {
      coordB[i] = 0;
      vectorB[i] = -vectorB[i];
       ballColor = CHSV(random(0, 9) * 28, 255U, 255U);
 
    }
  }
  if (coordB[0U] > (int16_t)((WIDTH - deltaValue) * 10))
  {
    coordB[0U] = (WIDTH - deltaValue) * 10;
    vectorB[0U] = -vectorB[0U];
    ballColor = CHSV(random(0, 9) * 28, 255U, 255U);

  }
  if (coordB[1U] > (int16_t)((HEIGHT - deltaValue) * 10))
  {
    coordB[1U] = (HEIGHT - deltaValue) * 10;
    vectorB[1U] = -vectorB[1U];
      ballColor = CHSV(random(0, 9) * 28, 255U, 255U);

  }
  
if (Scale <= 85)  // при масштабе до 85 выводим кубик без шлейфа
    memset8( leds, 0, NUM_LEDS * 3);
  else if (Scale > 85 and Scale <= 170)
    fadeToBlackBy(leds, NUM_LEDS, 255 - (10 * (Speed) /255) + 30); // выводим кубик со шлейфом, длинна которого зависит от скорости.
  else
    fadeToBlackBy(leds, NUM_LEDS, 255 - (10 * (Speed) /255) + 15); // выводим кубик с длинным шлейфом, длинна которого зависит от скорости.

     
  for (uint8_t i = 0U; i < deltaValue; i++)
    for (uint8_t j = 0U; j < deltaValue; j++)
      leds[XY(coordB[0U] / 10 + i, coordB[1U] / 10 + j)] = ballColor;
}

//-------------------Светлячки со шлейфом----------------------------
void ballsRoutine()
{
  if (loadingFlag)
  {
    loadingFlag = false;

    for (uint8_t j = 0U; j < BALLS_AMOUNT; j++)
    {
      int8_t sign;
      // забиваем случайными данными
      coord[j][0U] = WIDTH / 2 * 10;
      random(0, 2) ? sign = 1 : sign = -1;
      vector[j][0U] = random(4, 15) * sign;
      coord[j][1U] = HEIGHT / 2 * 10;
      random(0, 2) ? sign = 1 : sign = -1;
      vector[j][1U] = random(4, 15) * sign;
      //ballColors[j] = CHSV(random(0, 9) * 28, 255U, 255U);
      // цвет зависит от масштаба
      ballColors[j] = CHSV((Scale * (j + 1)) % 256U, 255U, 255U);
    }
  }

    fader(TRACK_STEP);

  // движение шариков
  for (uint8_t j = 0U; j < BALLS_AMOUNT; j++)
  {
    // движение шариков
    for (uint8_t i = 0U; i < 2U; i++)
    {
      coord[j][i] += vector[j][i];
      if (coord[j][i] < 0)
      {
        coord[j][i] = 0;
        vector[j][i] = -vector[j][i];
      }
    }

    if (coord[j][0U] > (int16_t)((WIDTH - 1) * 10))
    {
      coord[j][0U] = (WIDTH - 1) * 10;
      vector[j][0U] = -vector[j][0U];
    }
    if (coord[j][1U] > (int16_t)((HEIGHT - 1) * 10))
    {
      coord[j][1U] = (HEIGHT - 1) * 10;
      vector[j][1U] = -vector[j][1U];
    }
    leds[XY(coord[j][0U] / 10, coord[j][1U] / 10)] =  ballColors[j];
  }
}

//-----------------Эффект Вышиванка-------------
byte count = 0;
byte direct = 1;
byte flip = 0;
byte generation = 0;
void MunchRoutine() { 
  if (loadingFlag)
  { setCurrentPalette(palette);
    loadingFlag = false;
  }
  for (byte x = 0; x < WIDTH; x++) {
    for (byte y = 0; y < HEIGHT; y++) {
      drawPixelXY(x, y,(x ^ y ^ flip) < count ? ColorFromPalette(*curPalette, ((x ^ y) << 4) + generation) : CRGB::Black);
    }
  }

  count += direct;

  if (count <= 0 || count >= WIDTH) {
    direct = -direct;
  }

  if (count <= 0) {
    if (flip == 0)
      flip = 7;
    else
      flip = 0;
  }

  generation++;
}

// ---- Эффект "Тени" 
// https://github.com/vvip-68/GyverPanelWiFi/blob/master/firmware/GyverPanelWiFi_v1.02/effects.ino
    uint16_t sPseudotime = 0;
    uint16_t sLastMillis = 0;
    uint16_t sHue16 = 0;
void shadowsRoutine() {
  
  uint8_t sat8 = beatsin88( 87, 220, 250);
  uint8_t brightdepth = beatsin88( 341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 225), (40 * 256));
  uint8_t msmultiplier = beatsin88(map(Speed, 1, 255, 100, 255), 32, map(Speed, 1, 255, 60, 255)); // beatsin88(147, 32, 60);
  byte effectBrightness = Scale;
  uint16_t hue16 = sHue16;//gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 1, 3000);
  
  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis ;

  sLastMillis  = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88( 400, 5,9);
  uint16_t brightnesstheta16 = sPseudotime;

  for( uint16_t i = 0 ; i < NUM_LEDS; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;

    brightnesstheta16  += brightnessthetainc16;
    uint16_t b16 = sin16( brightnesstheta16  ) + 32768U;

    uint32_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536U;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536U;
    bri8 += (255 - brightdepth);
    
    CRGB newcolor = CHSV( hue8, sat8, map8(bri8, map(effectBrightness, 1, 255, 32, 125), map(effectBrightness, 1, 255, 125, 250))); 
    
    uint16_t pixelnumber = i;
    pixelnumber = (NUM_LEDS-1) - pixelnumber;
    
    nblend( leds[pixelnumber], newcolor, 64);
  }
}

// ============= ЭФФЕКТ ВОЛНЫ ===============
// https://github.com/pixelmatix/aurora/blob/master/PatternWave.h
// Адаптация от (c) SottNick

    byte waveThetaUpdate = 0;
    byte waveThetaUpdateFrequency = 0;
    byte waveTheta = 0;

    byte hueUpdate = 0;
    byte hueUpdateFrequency = 0;
//    byte hue = 0; будем использовать сдвиг от эффектов Радуга

    byte waveRotation = 0;
    uint8_t waveScale = 256 / WIDTH;
    uint8_t waveCount = 1;

void WaveRoutine() {
    if (loadingFlag)
    {
      loadingFlag = false;
      waveRotation = (Scale - 1) / 25U;
      waveCount = Speed % 2;
      setCurrentPalette(palette);
    }
 
        dimAll(254);
  
        int n = 0;

        switch (waveRotation) {
            case 0:
                for (uint8_t x = 0; x < WIDTH; x++) {
                    n = quadwave8(x * 2 + waveTheta) / waveScale;
                    drawPixelXY(x, n, ColorFromPalette(*curPalette, hue + x));
                    if (waveCount != 1)
                        drawPixelXY(x, HEIGHT - 1 - n, ColorFromPalette(*curPalette, hue + x));
                }
                break;

            case 1:
                for (uint8_t y = 0; y < HEIGHT; y++) {
                    n = quadwave8(y * 2 + waveTheta) / waveScale;
                    drawPixelXY(n, y, ColorFromPalette(*curPalette, hue + y));
                    if (waveCount != 1)
                        drawPixelXY(WIDTH - 1 - n, y, ColorFromPalette(*curPalette, hue + y));
                }
                break;

            case 2:
                for (uint8_t x = 0; x < WIDTH; x++) {
                    n = quadwave8(x * 2 - waveTheta) / waveScale;
                    drawPixelXY(x, n, ColorFromPalette(*curPalette, hue + x));
                    if (waveCount != 1)
                        drawPixelXY(x, HEIGHT - 1 - n, ColorFromPalette(*curPalette, hue + x));
                }
                break;

            case 3:
                for (uint8_t y = 0; y < HEIGHT; y++) {
                    n = quadwave8(y * 2 - waveTheta) / waveScale;
                    drawPixelXY(n, y, ColorFromPalette(*curPalette, hue + y));
                    if (waveCount != 1)
                        drawPixelXY(WIDTH - 1 - n, y, ColorFromPalette(*curPalette, hue + y));
                }
                break;
        }


        if (waveThetaUpdate >= waveThetaUpdateFrequency) {
            waveThetaUpdate = 0;
            waveTheta++;
        }
        else {
            waveThetaUpdate++;
        }

        if (hueUpdate >= hueUpdateFrequency) {
            hueUpdate = 0;
            hue++;
        }
        else {
            hueUpdate++;
        }
        }

// ---------------------Огненная Лампа-------------------------------
// Yaroslaw Turbin, 22.06.2020 
// https://vk.com/ldirko
// https://pastebin.com/eKqe4zzA
// доработки - kostyamat
void fireRoutine() {
if(loadingFlag){
  setCurrentPalette(palette);
  loadingFlag=false; 
}

  uint8_t speedy = map(Speed, 1, 255, 255, 0);
  uint8_t _scale = Scale + 30;

  uint32_t a = millis();
  for (byte i = 0U; i < WIDTH; i++) {
    for (float j = 0.; j < HEIGHT; j++) {
      drawPixelXY((WIDTH - 1) - i, (HEIGHT - 1) - j, ColorFromPalette(*curPalette, qsub8(inoise8(i * _scale, j * _scale + a, a / speedy), abs8(j - (HEIGHT - 1)) * 255 / (HEIGHT - 1)), 255));    }
  }
}

//---------------Лаволампа------------------------------
//Основа @SottNick
//Оптимизация @Stepko 
float ball[(WIDTH / 2) -  ((WIDTH - 1) & 0x01)][4];
void drawBlob(uint8_t l, CRGB color) { //раз круги нарисовать не получается, будем попиксельно вырисовывать 2 варианта пузырей
  if (ball[l][3] == 2)
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
      ball[i][3] = random(1,3);
      ball[i][2] = (float)random8(5, 11) / (257U - Speed) / 4.0;
      ball[i][0] = 0;
      ball[i][1] = i * 2U + random8(2);
      if ( ball[i][2] == 0)
      ball[i][2] = 1;     
    }
    loadingFlag = false;
    setCurrentPalette(palette);
  }
  dimAll(100);
  blurScreen(20);
  for (byte i = 0; i < (WIDTH / 2) -  ((WIDTH - 1) & 0x01); i++) {
    // Draw 'ball'
    drawBlob(i, ColorFromPalette(*curPalette, ball[i][0] * 16));
    if (ball[i][0] + ball[i][3] >= HEIGHT - 1)
      ball[i][0] += (ball[i][2] * ((HEIGHT - 1 - ball[i][0]) / ball[i][3] + 0.005));
    else if (ball[i][0] - ball[i][3] <= 0)
      ball[i][0] += (ball[i][2] * (ball[i][0] / ball[i][3] + 0.005));
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
    }}}
