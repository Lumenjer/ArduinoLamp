// ================================= ЭФФЕКТЫ ====================================

uint8_t wrapX(int8_t x){
  return (x + WIDTH)%WIDTH;
}
uint8_t wrapY(int8_t y){
  return (y + HEIGHT)%HEIGHT;
}
uint8_t  hue2; // постепенный сдвиг оттенка или какой-нибудь другой цикличный счётчик
uint8_t deltaHue, deltaHue2; // ещё пара таких же, когда нужно много
uint8_t step; // какой-нибудь счётчик кадров или постедовательностей операций
uint8_t deltaValue; // просто повторно используемая переменная
CRGB ledsbuff[NUM_LEDS];
#define NUM_LAYERSMAX 2
uint8_t noise3d[NUM_LAYERSMAX][WIDTH][HEIGHT];
uint8_t shiftHue[HEIGHT];
uint8_t shiftValue[HEIGHT];
void dimAll(uint8_t value) {
  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    leds[i].nscale8(value); //fadeToBlackBy
  }
}



// стандартные функции библиотеки LEDraw от @Palpalych (для адаптаций его эффектов)
void blurScreen(fract8 blur_amount, CRGB *LEDarray = leds)
{
  blur2d(LEDarray, WIDTH, HEIGHT, blur_amount);
}

// палитра для типа реалистичного водопада (если ползунок Масштаб выставить на 100)
extern const TProgmemRGBPalette16 WaterfallColors_p FL_PROGMEM = {0x000000, 0x060707, 0x101110, 0x151717, 0x1C1D22, 0x242A28, 0x363B3A, 0x313634, 0x505552, 0x6B6C70, 0x98A4A1, 0xC1C2C1, 0xCACECF, 0xCDDEDD, 0xDEDFE0, 0xB2BAB9};

// добавлено изменение текущей палитры (используется во многих эффектах ниже для бегунка Масштаб)
const TProgmemRGBPalette16 *palette_arr[] = {
    &PartyColors_p,
    &OceanColors_p, 
    &LavaColors_p, 
    &HeatColors_p, 
    &WaterfallColors_p, 
    &CloudColors_p, 
    &ForestColors_p, 
    &RainbowColors_p, 
    &RainbowStripeColors_p};
const TProgmemRGBPalette16 *curPalette = palette_arr[0];
void setCurrentPalette(){
      if (modes[currentMode].scale > 100U) modes[currentMode].scale = 100U; // чтобы не было проблем при прошивке без очистки памяти
      curPalette = palette_arr[(uint8_t)(modes[currentMode].scale/100.0F*((sizeof(palette_arr)/sizeof(TProgmemRGBPalette16 *))-0.01F))];
}
CRGBPalette16 palette;
CRGB _pulse_color;
// --------------------------------- конфетти ------------------------------------
#define FADE_OUT_SPEED        (70U)                         // скорость затухания
void sparklesRoutine()
{
  for (uint8_t i = 0; i < modes[currentMode].scale; i++)
  {
    uint8_t x = random(0U, WIDTH);
    uint8_t y = random(0U, HEIGHT);
    if (getPixColorXY(x, y) == 0U)
    {
      leds[XY(x, y)] = CHSV(random(0U, 255U), 255U, 255U);
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

// -------------------------------------- огонь ---------------------------------------------
// эффект "огонь"
#define SPARKLES 1        // вылетающие угольки вкл выкл
unsigned char line[WIDTH];
int pcnt = 0;

//these values are substracetd from the generated values to give a shape to the animation
const unsigned char valueMask[8][16] PROGMEM = {
  {32 , 0  , 0  , 0  , 0  , 0  , 0  , 32 , 32 , 0  , 0  , 0  , 0  , 0  , 0  , 32 },
  {64 , 0  , 0  , 0  , 0  , 0  , 0  , 64 , 64 , 0  , 0  , 0  , 0  , 0  , 0  , 64 },
  {96 , 32 , 0  , 0  , 0  , 0  , 32 , 96 , 96 , 32 , 0  , 0  , 0  , 0  , 32 , 96 },
  {128, 64 , 32 , 0  , 0  , 32 , 64 , 128, 128, 64 , 32 , 0  , 0  , 32 , 64 , 128},
  {160, 96 , 64 , 32 , 32 , 64 , 96 , 160, 160, 96 , 64 , 32 , 32 , 64 , 96 , 160},
  {192, 128, 96 , 64 , 64 , 96 , 128, 192, 192, 128, 96 , 64 , 64 , 96 , 128, 192},
  {255, 160, 128, 96 , 96 , 128, 160, 255, 255, 160, 128, 96 , 96 , 128, 160, 255},
  {255, 192, 160, 128, 128, 160, 192, 255, 255, 192, 160, 128, 128, 160, 192, 255}
};

//these are the hues for the fire,
//should be between 0 (red) to about 25 (yellow)
const unsigned char hueMask[8][16] PROGMEM = {
  {1 , 11, 19, 25, 25, 22, 11, 1 , 1 , 11, 19, 25, 25, 22, 11, 1 },
  {1 , 8 , 13, 19, 25, 19, 8 , 1 , 1 , 8 , 13, 19, 25, 19, 8 , 1 },
  {1 , 8 , 13, 16, 19, 16, 8 , 1 , 1 , 8 , 13, 16, 19, 16, 8 , 1 },
  {1 , 5 , 11, 13, 13, 13, 5 , 1 , 1 , 5 , 11, 13, 13, 13, 5 , 1 },
  {1 , 5 , 11, 11, 11, 11, 5 , 1 , 1 , 5 , 11, 11, 11, 11, 5 , 1 },
  {0 , 1 , 5 , 8 , 8 , 5 , 1 , 0 , 0 , 1 , 5 , 8 , 8 , 5 , 1 , 0 },
  {0 , 0 , 1 , 5 , 5 , 1 , 0 , 0 , 0 , 0 , 1 , 5 , 5 , 1 , 0 , 0 },
  {0 , 0 , 0 , 1 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 1 , 0 , 0 , 0 }
};

void fireRoutine() {
  if (loadingFlag) {
    loadingFlag = false;
    //FastLED.clear();
    generateLine();
  }
  if (pcnt >= 100) {
    shiftUp();
    generateLine();
    pcnt = 0;
  }
  drawFrame(pcnt);
  pcnt += 30;
}

// Случайным образом генерирует следующую линию (matrix row)

void generateLine() {
  for (uint8_t x = 0; x < WIDTH; x++) {
    line[x] = random(128, 255);
  }
}

void shiftUp() {
  for (uint8_t y = HEIGHT - 1; y > 0; y--) {
    for (uint8_t x = 0; x < WIDTH; x++) {
      uint8_t newX = x;
      if (x > 15) newX = x - 15;
      if (y > 7) continue;
      matrixValue[y][newX] = matrixValue[y - 1][newX];
    }
  }

  for (uint8_t x = 0; x < WIDTH; x++) {
    uint8_t newX = x;
    if (x > 15) newX = x - 15;
    matrixValue[0][newX] = line[newX];
  }
}

// рисует кадр, интерполируя между 2 "ключевых кадров"
// параметр pcnt - процент интерполяции

void drawFrame(int pcnt) {
  int nextv;

  //each row interpolates with the one before it
  for (unsigned char y = HEIGHT - 1; y > 0; y--) {
    for (unsigned char x = 0; x < WIDTH; x++) {
      uint8_t newX = x;
      if (x > 15) newX = x - 15;
      if (y < 8) {
        nextv =
          (((100.0 - pcnt) * matrixValue[y][newX]
            + pcnt * matrixValue[y - 1][newX]) / 100.0)
          - pgm_read_byte(&(valueMask[y][newX]));

        CRGB color = CHSV(
                       modes[1].scale * 2.5 + pgm_read_byte(&(hueMask[y][newX])), // H
                       255, // S
                       (uint8_t)max(0, nextv) // V
                     );

        leds[getPixelNumber(x, y)] = color;
      } else if (y == 8 && SPARKLES) {
        if (random(0, 20) == 0 && getPixColorXY(x, y - 1) != 0) drawPixelXY(x, y, getPixColorXY(x, y - 1));
        else drawPixelXY(x, y, 0);
      } else if (SPARKLES) {

        // старая версия для яркости
        if (getPixColorXY(x, y - 1) > 0)
          drawPixelXY(x, y, getPixColorXY(x, y - 1));
        else drawPixelXY(x, y, 0);

      }
    }
  }

  //Перавя стрка интерполируется со следующей "next" линией
  for (unsigned char x = 0; x < WIDTH; x++) {
    uint8_t newX = x;
    if (x > 15) newX = x - 15;
    CRGB color = CHSV(
                   modes[1].scale * 2.5 + pgm_read_byte(&(hueMask[0][newX])), // H
                   255,           // S
                   (uint8_t)(((100.0 - pcnt) * matrixValue[0][newX] + pcnt * line[newX]) / 100.0) // V
                 );
    leds[getPixelNumber(newX, 0)] = color;
  }
}

byte hue;
// ---------------------------------------- радуга ------------------------------------------
void rainbowVertical() {
  hue += 2;
  for (byte j = 0; j < HEIGHT; j++) {
    CHSV thisColor = CHSV((byte)(hue + j * modes[2].scale), 255, 255);
    for (byte i = 0; i < WIDTH; i++)
      drawPixelXY(i, j, thisColor);
  }
}
void rainbowHorizontal() {
  hue += 2;
  for (byte i = 0; i < WIDTH; i++) {
    CHSV thisColor = CHSV((byte)(hue + i * modes[3].scale), 255, 255);
    for (byte j = 0; j < HEIGHT; j++)
      drawPixelXY(i, j, thisColor);   //leds[getPixelNumber(i, j)] = thisColor;
  }
}
void rainbowDiagonalRoutine()
{
  if (loadingFlag)
  {
    loadingFlag = false;
    FastLED.clear();
  }

  hue += 8;
  for (uint8_t i = 0U; i < WIDTH; i++)
  {
    for (uint8_t j = 0U; j < HEIGHT; j++)
    {
      float twirlFactor = 3.0F * (modes[4].scale / 100.0F);      // на сколько оборотов будет закручена матрица, [0..3]
      CRGB thisColor = CHSV((uint8_t)(hue + ((float)WIDTH / HEIGHT * i + j * twirlFactor) * ((float)255 / maxDim)), 255, 255);
      drawPixelXY(i, j, thisColor);
    }
  }
}
// ---------------------------------------- ЦВЕТА ------------------------------------------
void colorsRoutine() {
  hue += modes[16].scale;
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(hue, 255, 255);
  }
}

// --------------------------------- ЦВЕТ ------------------------------------
void colorRoutine() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(modes[15].scale * 2.5, 255, 255);
  }
}

// ------------------------------ снегопад 2.0 --------------------------------
void snowRoutine() {
  // сдвигаем всё вниз
  for (byte x = 0; x < WIDTH; x++) {
    for (byte y = 0; y < HEIGHT - 1; y++) {
      drawPixelXY(x, y, getPixColorXY(x, y + 1));
    }
  }

  for (byte x = 0; x < WIDTH; x++) {
    // заполняем случайно верхнюю строку
    // а также не даём двум блокам по вертикали вместе быть
    if (getPixColorXY(x, HEIGHT - 2) == 0 && (random(0, modes[19].scale) == 0))
      drawPixelXY(x, HEIGHT - 1, 0xE0FFFF - 0x101010 * random(0, 4));
    else
      drawPixelXY(x, HEIGHT - 1, 0x000000);
  }
}

// ------------------------------ МАТРИЦА ------------------------------
void matrixRoutine() {
  for (byte x = 0; x < WIDTH; x++) {
    // заполняем случайно верхнюю строку
    uint32_t thisColor = getPixColorXY(x, HEIGHT - 1);
    if (thisColor == 0)
      drawPixelXY(x, HEIGHT - 1, 0x00FF00 * (random(0, modes[17].scale) == 0));
    else if (thisColor < 0x002000)
      drawPixelXY(x, HEIGHT - 1, 0);
    else
      drawPixelXY(x, HEIGHT - 1, thisColor - 0x002000);
  }

  // сдвигаем всё вниз
  for (byte x = 0; x < WIDTH; x++) {
    for (byte y = 0; y < HEIGHT - 1; y++) {
      drawPixelXY(x, y, getPixColorXY(x, y + 1));
    }
  }
}

// ------------------------------ БЕЛАЯ ЛАМПА ------------------------------
void whiteLamp() {
  for (byte y = 0; y < (HEIGHT / 2); y++) {
    CHSV color = CHSV(100, 1, constrain(modes[17].brightness - (long)modes[18].speed * modes[17].brightness / 255 * y / 2, 1, 255));
    for (byte x = 0; x < WIDTH; x++) {
      drawPixelXY(x, y + 8, color);
      drawPixelXY(x, 7 - y, color);
    }
  }
}
//-------------Шторм,Метель-----------------

#define e_sns_DENSE (32U) // плотность снега - меньше = плотнее
void stormRoutine2(bool isColored)
{
  // заполняем головами комет
  uint8_t Saturation = 0U;    // цвет хвостов
  uint8_t e_TAIL_STEP = 127U; // длина хвоста
  if (isColored)
    Saturation = modes[currentMode].scale * 2.55;
  else
  {
    e_TAIL_STEP = 255U - modes[currentMode].scale * 2.55;
  }
  for (int8_t x = 0U; x < WIDTH - 1U; x++) // fix error i != 0U
  {
    if (!random8(e_sns_DENSE) &&
        !getPixColorXY(wrapX(x), HEIGHT - 1U) &&
        !getPixColorXY(wrapX(x + 1U), HEIGHT - 1U) &&
        !getPixColorXY(wrapX(x - 1U), HEIGHT - 1U))
    {
      drawPixelXY(x, HEIGHT - 1U, CHSV(random8(), Saturation, random8(64U, 255U)));
    }
  }

  // сдвигаем по диагонали
  for (int8_t y = 0U; y < HEIGHT - 1U; y++)
  {
    for (int8_t x = 0; x < WIDTH; x++)
    {
      drawPixelXY(wrapX(x + 1U), y, getPixColorXY(x, y + 1U));
    }
  }

  // уменьшаем яркость верхней линии, формируем "хвосты"
  for (int8_t i = 0U; i < WIDTH; i++)
  {
    fadePixel(i, HEIGHT - 1U, e_TAIL_STEP);
  }
}
//-----------------------------------------
void SinusoidRoutine()
{
  const uint8_t semiHeightMajor =  HEIGHT / 2 + (HEIGHT % 2);
  const uint8_t semiWidthMajor =  WIDTH / 2  + (WIDTH % 2) ;
  float e_s3_speed = 0.004 * modes[currentMode].speed + 0.015; // speed of the movement along the Lissajous curves
  float e_s3_size = 3 * (float)modes[currentMode].scale/100.0 + 2;    // amplitude of the curves

  float time_shift = float(millis()%(uint32_t)(30000*(1.0/((float)modes[currentMode].speed/255))));

  for (uint8_t y = 0; y < HEIGHT; y++) {
    for (uint8_t x = 0; x < WIDTH; x++) {
      CRGB color;

      float cx = y + float(e_s3_size * (sinf (float(e_s3_speed * 0.003 * time_shift)))) - semiHeightMajor;  // the 8 centers the middle on a 16x16
      float cy = x + float(e_s3_size * (cosf (float(e_s3_speed * 0.0022 * time_shift)))) - semiWidthMajor;
      float v = 127 * (1 + sinf ( sqrtf ( ((cx * cx) + (cy * cy)) ) ));
      color.r = v;

      cx = x + float(e_s3_size * (sinf (e_s3_speed * float(0.0021 * time_shift)))) - semiWidthMajor;
      cy = y + float(e_s3_size * (cosf (e_s3_speed * float(0.002 * time_shift)))) - semiHeightMajor;
      v = 127 * (1 + sinf ( sqrtf ( ((cx * cx) + (cy * cy)) ) ));
      color.b = v;

      cx = x + float(e_s3_size * (sinf (e_s3_speed * float(0.0041 * time_shift)))) - semiWidthMajor;
      cy = y + float(e_s3_size * (cosf (e_s3_speed * float(0.0052 * time_shift)))) - semiHeightMajor;
      v = 127 * (1 + sinf ( sqrtf ( ((cx * cx) + (cy * cy)) ) ));
      color.g = v;
      drawPixelXY(x, y, color);
    }
  }
}
//-------------
void MetaBallsRoutine() {
    if (loadingFlag)
    {
      loadingFlag = false;
      setCurrentPalette();
    }
      
  float speed = modes[currentMode].speed / 127.0;

  // get some 2 random moving points
  uint8_t x2 = inoise8(millis() * speed, 25355, 685 ) / WIDTH;
  uint8_t y2 = inoise8(millis() * speed, 355, 11685 ) / HEIGHT;

  uint8_t x3 = inoise8(millis() * speed, 55355, 6685 ) / WIDTH;
  uint8_t y3 = inoise8(millis() * speed, 25355, 22685 ) / HEIGHT;

  // and one Lissajou function
  uint8_t x1 = beatsin8(23 * speed, 0, 15);
  uint8_t y1 = beatsin8(28 * speed, 0, 15);

  for (uint8_t y = 0; y < HEIGHT; y++) {
    for (uint8_t x = 0; x < WIDTH; x++) {

      // calculate distances of the 3 points from actual pixel
      // and add them together with weightening
      uint8_t  dx =  abs(x - x1);
      uint8_t  dy =  abs(y - y1);
      uint8_t dist = 2 * sqrt((dx * dx) + (dy * dy));

      dx =  abs(x - x2);
      dy =  abs(y - y2);
      dist += sqrt((dx * dx) + (dy * dy));

      dx =  abs(x - x3);
      dy =  abs(y - y3);
      dist += sqrt((dx * dx) + (dy * dy));

      // inverse result
      //byte color = modes[currentMode].Speed * 10 / dist;
      byte color = 1000U / dist;

      // map color between thresholds
      if (color > 0 and color < 60) {
        if (modes[currentMode].scale == 100U)
          drawPixelXY(x, y, CHSV(color * 9, 255, 255));// это оригинальный цвет эффекта
        else
          drawPixelXY(x, y, ColorFromPalette(*curPalette, color * 9));
      } else {
        if (modes[currentMode].scale == 100U)
          drawPixelXY(x, y, CHSV(0, 255, 255)); // в оригинале центральный глаз почему-то крвсный
        else
          drawPixelXY(x, y, ColorFromPalette(*curPalette, 0U));
      }
      // show the 3 points, too
      drawPixelXY(x1, y1, CRGB(255, 255, 255));
      drawPixelXY(x2, y2, CRGB(255, 255, 255));
      drawPixelXY(x3, y3, CRGB(255, 255, 255));
    }
  }
}
//----------------------------------------
#define RANDOM_COLOR          (1U)                          // случайный цвет при отскоке
int16_t coordB[2U];
int8_t vectorB[2U];
CRGB ballColor;
//int8_t deltaValue; //ballSize;

void ballRoutine()
{
  if (loadingFlag)
  {
    loadingFlag = false;
    //FastLED.clear();

    for (uint8_t i = 0U; i < 2U; i++)
    {
      coordB[i] = WIDTH / 2 * 10;
      vectorB[i] = random(8, 20);
    }
    deltaValue = map(modes[currentMode].scale * 2.55, 0U, 255U, 2U, max((uint8_t)min(WIDTH, HEIGHT) / 3, 2));
    ballColor = CHSV(random(0, 9) * 28, 255U, 255U);
//    _pulse_color = CHSV(random(0, 9) * 28, 255U, 255U);
  }

//  if (!(modes[currentMode].Scale & 0x01))
//  {
//    hue += (modes[currentMode].Scale - 1U) % 11U * 8U + 1U;

//    ballColor = CHSV(hue, 255U, 255U);
//  }
 
  if ((modes[currentMode].scale & 0x01))
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
      if (RANDOM_COLOR) ballColor = CHSV(random(0, 9) * 28, 255U, 255U); // if (RANDOM_COLOR && (modes[currentMode].Scale & 0x01))
      //vectorB[i] += random(0, 6) - 3;
    }
  }
  if (coordB[0U] > (int16_t)((WIDTH - deltaValue) * 10))
  {
    coordB[0U] = (WIDTH - deltaValue) * 10;
    vectorB[0U] = -vectorB[0U];
    if (RANDOM_COLOR) ballColor = CHSV(random(0, 9) * 28, 255U, 255U);
    //vectorB[0] += random(0, 6) - 3;
  }
  if (coordB[1U] > (int16_t)((HEIGHT - deltaValue) * 10))
  {
    coordB[1U] = (HEIGHT - deltaValue) * 10;
    vectorB[1U] = -vectorB[1U];
    if (RANDOM_COLOR) ballColor = CHSV(random(0, 9) * 28, 255U, 255U);
    //vectorB[1] += random(0, 6) - 3;
  }
  
//  if (modes[currentMode].Scale & 0x01)
//    dimAll(135U);
//    dimAll(255U - (modes[currentMode].Scale - 1U) % 11U * 24U);
//  else
    FastLED.clear();
     
  for (uint8_t i = 0U; i < deltaValue; i++)
    for (uint8_t j = 0U; j < deltaValue; j++)
      leds[XY(coordB[0U] / 10 + i, coordB[1U] / 10 + j)] = ballColor;
}

//-----------------------------------------------
#define BALLS_AMOUNT          (3U)                          // количество "шариков"
#define CLEAR_PATH            (1U)                          // очищать путь
#define BALL_TRACK            (1U)                          // (0 / 1) - вкл/выкл следы шариков
#define TRACK_STEP            (70U)                         // длина хвоста шарика (чем больше цифра, тем хвост короче)
int16_t coord[BALLS_AMOUNT][2U];
int8_t vector[BALLS_AMOUNT][2U];
CRGB ballColors[BALLS_AMOUNT];
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
      ballColors[j] = CHSV((modes[currentMode].scale * (j + 1)) % 256U, 255U, 255U);
    }
  }

  if (!BALL_TRACK)                                          // режим без следов шариков
  {
    FastLED.clear();
  }
  else                                                      // режим со следами
  {
    fader(TRACK_STEP);
  }

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

//-------------------------------------------------
extern const TProgmemRGBPalette16 WaterfallColors4in1_p FL_PROGMEM = {
  CRGB::Black,
  CRGB::DarkSlateGray,
  CRGB::DimGray,
  CRGB::LightSlateGray,

  CRGB::DimGray,
  CRGB::DarkSlateGray,
  CRGB::Silver,
  CRGB::DarkCyan,

  CRGB::Lavender,
  CRGB::Silver,
  CRGB::Azure,
  CRGB::LightGrey,

  CRGB::GhostWhite,
  CRGB::Silver,
  CRGB::White,
  CRGB::RoyalBlue
};

#define COOLINGNEW 32
#define SPARKINGNEW 80 
void fire2012WithPalette4in1() {
  uint8_t rCOOLINGNEW = constrain((uint16_t)(modes[currentMode].scale % 16) * 32 / HEIGHT + 16, 1, 255) ;
  // Array of temperature readings at each simulation cell
  //static byte heat[WIDTH][HEIGHT]; будет noise3d[0][WIDTH][HEIGHT]

  for (uint8_t x = 0; x < WIDTH; x++) {
    // Step 1.  Cool down every cell a little
    for (unsigned int i = 0; i < HEIGHT; i++) {
      //noise3d[0][x][i] = qsub8(noise3d[0][x][i], random8(0, ((rCOOLINGNEW * 10) / HEIGHT) + 2));
      noise3d[0][x][i] = qsub8(noise3d[0][x][i], random8(0, rCOOLINGNEW));
    }

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for (int k = HEIGHT - 1; k >= 2; k--) {
      noise3d[0][x][k] = (noise3d[0][x][k - 1] + noise3d[0][x][k - 2] + noise3d[0][x][k - 2]) / 3;
    }

    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if (random8() < SPARKINGNEW) {
      int y = random8(2);
      noise3d[0][x][y] = qadd8(noise3d[0][x][y], random8(160, 255));
    }

    // Step 4.  Map from heat cells to LED colors
    for (unsigned int j = 0; j < HEIGHT; j++) {
      // Scale the heat value from 0-255 down to 0-240
      // for best results with color palettes.
      byte colorindex = scale8(noise3d[0][x][j], 240);
      if  (modes[currentMode].scale < 16) {            // Lavafall
        leds[XY(x, (HEIGHT - 1) - j)] = ColorFromPalette(LavaColors_p, colorindex);
      } else if (modes[currentMode].scale < 32) {      // Firefall
        leds[XY(x, (HEIGHT - 1) - j)] = ColorFromPalette(HeatColors_p, colorindex);
      } else if (modes[currentMode].scale < 48) {      // Waterfall
        leds[XY(x, (HEIGHT - 1) - j)] = ColorFromPalette(WaterfallColors4in1_p, colorindex);
      } else if (modes[currentMode].scale < 64) {      // Skyfall
        leds[XY(x, (HEIGHT - 1) - j)] = ColorFromPalette(CloudColors_p, colorindex);
      } else if (modes[currentMode].scale < 80) {      // Forestfall
        leds[XY(x, (HEIGHT - 1) - j)] = ColorFromPalette(ForestColors_p, colorindex);
      } else if (modes[currentMode].scale < 96) {      // Rainbowfall
        leds[XY(x, (HEIGHT - 1) - j)] = ColorFromPalette(RainbowColors_p, colorindex);
      } else {                      // Aurora
        leds[XY(x, (HEIGHT - 1) - j)] = ColorFromPalette(RainbowStripeColors_p, colorindex);
      }
    }
  }
}
//---------------------------------------------

#define TWINKLES_SPEEDS 4     // всего 4 варианта скоростей мерцания
#define TWINKLES_MULTIPLIER 6 // слишком медленно, если на самой медленной просто по единичке к яркости добавлять

void twinklesRoutine(){
    if (loadingFlag)
    {
      loadingFlag = false;
      setCurrentPalette();
      hue = 0U;
      deltaValue = (modes[currentMode].scale - 1U) % 11U + 1U;  // вероятность пикселя загореться от 1/1 до 1/11
      for (uint32_t idx=0; idx < NUM_LEDS; idx++) {
        if (random8(deltaValue) == 0){
          ledsbuff[idx].r = random8();                          // оттенок пикселя
          ledsbuff[idx].g = random8(1, TWINKLES_SPEEDS * 2 +1); // скорость и направление (нарастает 1-4 или угасает 5-8)
          ledsbuff[idx].b = random8();                          // яркость
        }
        else
          ledsbuff[idx] = 0;                                    // всё выкл
      }
    }
    for (uint32_t idx=0; idx < NUM_LEDS; idx++) {
      if (ledsbuff[idx].b == 0){
        if (random8(deltaValue) == 0 && hue > 0){  // если пиксель ещё не горит, зажигаем каждый ХЗй
          ledsbuff[idx].r = random8();                          // оттенок пикселя
          ledsbuff[idx].g = random8(1, TWINKLES_SPEEDS +1);     // скорость и направление (нарастает 1-4, но не угасает 5-8)
          ledsbuff[idx].b = ledsbuff[idx].g;                    // яркость
          hue--; // уменьшаем количество погасших пикселей
        }
      }
      else if (ledsbuff[idx].g <= TWINKLES_SPEEDS){             // если нарастание яркости
        if (ledsbuff[idx].b > 255U - ledsbuff[idx].g - TWINKLES_MULTIPLIER){            // если досигнут максимум
          ledsbuff[idx].b = 255U;
          ledsbuff[idx].g = ledsbuff[idx].g + TWINKLES_SPEEDS;
        }
        else
          ledsbuff[idx].b = ledsbuff[idx].b + ledsbuff[idx].g + TWINKLES_MULTIPLIER;
      }
      else {                                                    // если угасание яркости
        if (ledsbuff[idx].b <= ledsbuff[idx].g - TWINKLES_SPEEDS + TWINKLES_MULTIPLIER){// если досигнут минимум
          ledsbuff[idx].b = 0;                                  // всё выкл
          hue++; // считаем количество погасших пикселей
        }
        else
          ledsbuff[idx].b = ledsbuff[idx].b - ledsbuff[idx].g + TWINKLES_SPEEDS - TWINKLES_MULTIPLIER;
      }
      if (ledsbuff[idx].b == 0)
        leds[idx] = 0U;
      else
        leds[idx] = ColorFromPalette(*curPalette, ledsbuff[idx].r, ledsbuff[idx].b);
    }
}
//-----------------------------------------------------

void PrismataRoutine() {
  if (loadingFlag)
  {
    loadingFlag = false;
    setCurrentPalette();
  } 
  
  EVERY_N_MILLIS(33) {
    hue++; // используем переменную сдвига оттенка из функций радуги, чтобы не занимать память
  }
  blurScreen(20); // @Palpalych посоветовал делать размытие
  dimAll(255U - (modes[currentMode].scale - 1U) % 11U * 3U);

  for (int x = 0; x < WIDTH; x++)
  {
    //uint8_t y = beatsin8(x + 1, 0, HEIGHT-1); // это я попытался распотрошить данную функцию до исходного кода и вставить в неё регулятор скорости
    // вместо 28 в оригинале было 280, умножения на .Speed не было, а вместо >>17 было (<<8)>>24. короче, оригинальная скорость достигается при бегунке .Speed=20
    uint8_t beat = (GET_MILLIS() * (accum88(x + 1)) * 28 * modes[currentMode].speed) >> 17;
    uint8_t y = scale8(sin8(beat), HEIGHT-1);
    //и получилось!!!
    
    drawPixelXY(x, y, ColorFromPalette(*curPalette, x * 7 + hue));
  }
}
