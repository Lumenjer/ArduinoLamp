//Функции
void dimAll(uint8_t value) {
  fadeToBlackBy (leds, NUM_LEDS, 255U - value);
}

static float fmap(const float x, const float in_min, const float in_max, const float out_min, const float out_max) {
  return (out_max - out_min) * (x - in_min) / (in_max - in_min) + out_min;
}

// залить все
void fillAll(CRGB color) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = color;
  }
}
// функция отрисовки точки по координатам X Y
void drawPixelXY(uint8_t x, uint8_t y, CRGB color)
{
  if (x < 0 || x > (WIDTH - 1) || y < 0 || y > (HEIGHT - 1)) return;
  uint32_t thisPixel = XY((uint8_t)x, (uint8_t)y) * SEGMENTS;
  for (uint8_t i = 0; i < SEGMENTS; i++)
  {
    leds[thisPixel + i] = color;
  }
}

// функция получения цвета пикселя по его номеру
uint32_t getPixColor(uint32_t thisSegm)
{
  uint32_t thisPixel = thisSegm * SEGMENTS;
  if (thisPixel > NUM_LEDS - 1) return 0;
  return (((uint32_t)leds[thisPixel].r << 16) | ((uint32_t)leds[thisPixel].g << 8 ) | (uint32_t)leds[thisPixel].b);
}
// функция получения цвета пикселя в матрице по его координатам
uint32_t getPixColorXY(uint8_t x, uint8_t y)
{
  return getPixColor(XY(x, y));
}

// =================== НАСТРОЙКА МАТРИЦЫ ==================
#if (CONNECTION_ANGLE == 0 && STRIP_DIRECTION == 0)
#define _WIDTH WIDTH
#define THIS_X x
#define THIS_Y y

#elif (CONNECTION_ANGLE == 0 && STRIP_DIRECTION == 1)
#define _WIDTH HEIGHT
#define THIS_X y
#define THIS_Y x

#elif (CONNECTION_ANGLE == 1 && STRIP_DIRECTION == 0)
#define _WIDTH WIDTH
#define THIS_X x
#define THIS_Y (HEIGHT - y - 1)

#elif (CONNECTION_ANGLE == 1 && STRIP_DIRECTION == 3)
#define _WIDTH HEIGHT
#define THIS_X (HEIGHT - y - 1)
#define THIS_Y x

#elif (CONNECTION_ANGLE == 2 && STRIP_DIRECTION == 2)
#define _WIDTH WIDTH
#define THIS_X (WIDTH - x - 1)
#define THIS_Y (HEIGHT - y - 1)

#elif (CONNECTION_ANGLE == 2 && STRIP_DIRECTION == 3)
#define _WIDTH HEIGHT
#define THIS_X (HEIGHT - y - 1)
#define THIS_Y (WIDTH - x - 1)

#elif (CONNECTION_ANGLE == 3 && STRIP_DIRECTION == 2)
#define _WIDTH WIDTH
#define THIS_X (WIDTH - x - 1)
#define THIS_Y y

#elif (CONNECTION_ANGLE == 3 && STRIP_DIRECTION == 1)
#define _WIDTH HEIGHT
#define THIS_X y
#define THIS_Y (WIDTH - x - 1)

#else
#define _WIDTH WIDTH
#define THIS_X x
#define THIS_Y y
#pragma message "Wrong matrix parameters! Set to default"

#endif

#ifdef XY_MAPPING
uint16_t XY(uint8_t x, uint8_t y) {// получить номер пикселя в ленте по координатам
#define LAST_VISIBLE_LED 127
  if ( (x >= WIDTH) || (y >= HEIGHT) ) {
    return (LAST_VISIBLE_LED + 1);
  }

const uint8_t XYTable[] = {
   135, 134, 133, 132,   7,   6,   5,   4,   3,   2,   1,   0, 131, 130, 129, 128,
   143, 142, 141, 140,  15,  14,  13,  12,  11,  10,   9,   8, 139, 138, 137, 136,
   151, 150, 149, 148,  23,  22,  21,  20,  19,  18,  17,  16, 147, 146, 145, 144,
   159, 158, 157, 156,  31,  30,  29,  28,  27,  26,  25,  24, 155, 154, 153, 152,
   167, 166, 165, 164,  39,  38,  37,  36,  35,  34,  33,  32, 163, 162, 161, 160,
   175, 174, 173, 172,  47,  46,  45,  44,  43,  42,  41,  40, 171, 170, 169, 168,
   183, 182, 181, 180,  55,  54,  53,  52,  51,  50,  49,  48, 179, 178, 177, 176,
   191, 190, 189, 188,  63,  62,  61,  60,  59,  58,  57,  56, 187, 186, 185, 184,
   199, 198, 197, 196,  71,  70,  69,  68,  67,  66,  65,  64, 195, 194, 193, 192,
   207, 206, 205, 204,  79,  78,  77,  76,  75,  74,  73,  72, 203, 202, 201, 200,
   215, 214, 213, 212,  87,  86,  85,  84,  83,  82,  81,  80, 211, 210, 209, 208,
   223, 222, 221, 220,  95,  94,  93,  92,  91,  90,  89,  88, 219, 218, 217, 216,
   231, 230, 229, 228, 103, 102, 101, 100,  99,  98,  97,  96, 227, 226, 225, 224,
   239, 238, 237, 236, 111, 110, 109, 108, 107, 106, 105, 104, 235, 234, 233, 232,
   247, 246, 245, 244, 119, 118, 117, 116, 115, 114, 113, 112, 243, 242, 241, 240,
   255, 254, 253, 252, 127, 126, 125, 124, 123, 122, 121, 120, 251, 250, 249, 248
  };
  uint8_t i = (y * WIDTH) + x;
  uint8_t j = XYTable[i];
  return j;
}
#else
uint16_t XY(uint8_t x, uint8_t y)
{
  if (!(THIS_Y & 0x01) || MATRIX_TYPE)               // Even rows run forwards
    return (THIS_Y * _WIDTH + THIS_X);
  else
    return (THIS_Y * _WIDTH + _WIDTH - THIS_X - 1);  // Odd rows run backwards
}
#endif
// оставлено для совместимости со эффектами из старых прошивок
uint16_t getPixelNumber(uint8_t x, uint8_t y)
{
  return XY(x, y);
}
//Субпиксель по Y
void drawPixelXYF_Y(uint16_t x, float y, const CRGB &color)
  {
  //if (x<0 || y<0 || x>((float)WIDTH) || y>((float)HEIGHT)) return;

  // extract the fractional parts and derive their inverses
  uint8_t yy = (y - (int)y) * 255, iy = 255 - yy;
  // calculate the intensities for each affected pixel
  uint8_t wu[2] = {iy, yy};
  // multiply the intensities by the colour, and saturating-add them to the pixels
  for (int8_t i = 1; i >= 0; i--) {
      int16_t yn = y + (i & 1);
      CRGB clr = leds[XY(x, yn)];
      if(yn>0 && yn<(int)HEIGHT-1){
        clr.r = qadd8(clr.r, (color.r * wu[i]) >> 8);
        clr.g = qadd8(clr.g, (color.g * wu[i]) >> 8);
        clr.b = qadd8(clr.b, (color.b * wu[i]) >> 8);
      } else if(yn==0 || yn==(int)HEIGHT-1) {
        clr.r = qadd8(clr.r, (color.r * 85) >> 8);
        clr.g = qadd8(clr.g, (color.g * 85) >> 8);
        clr.b = qadd8(clr.b, (color.b * 85) >> 8);
      }
      leds[XY(x, yn)]= clr;
  }
  }
