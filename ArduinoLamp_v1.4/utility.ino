
void dimAll(uint8_t value) {
  fadeToBlackBy (leds, NUM_LEDS, 255U - value);
}

static float fmap(const float x, const float in_min, const float in_max, const float out_min, const float out_max) {
  return (out_max - out_min) * (x - in_min) / (in_max - in_min) + out_min;
}

void shiftDown() {
  for (byte x = 0; x < WIDTH; x++) {
    for (byte y = 0; y < HEIGHT - 1; y++) {
      drawPixelXY(x, y, getPixColorXY(x, y + 1));
    }
  }
}
void shiftUp() {
  for (byte x = 0; x < WIDTH; x++) {
    for (byte y = HEIGHT; y > 0; y--) {
      drawPixelXY(x, y, getPixColorXY(x, y - 1));
    }
  }
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

// **************** НАСТРОЙКА МАТРИЦЫ ****************
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

uint16_t XY(uint8_t x, uint8_t y)
{
  if (!(THIS_Y & 0x01) || MATRIX_TYPE)               // Even rows run forwards
    return (THIS_Y * _WIDTH + THIS_X);
  else
    return (THIS_Y * _WIDTH + _WIDTH - THIS_X - 1);  // Odd rows run backwards
}

// оставлено для совместимости со эффектами из старых прошивок
uint16_t getPixelNumber(uint8_t x, uint8_t y)
{
  return XY(x, y);
}
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
/*void drawPixelXYF(float x, float y, const CRGB &color)
{
  if (x < 0 || y < 0 || x > ((float)WIDTH - 1) || y > ((float)HEIGHT - 1)) return;

  // extract the fractional parts and derive their inverses
  uint8_t xx = (x - (int)x) * 255, yy = (y - (int)y) * 255, ix = 255 - xx, iy = 255 - yy;
  // calculate the intensities for each affected pixel
#define WU_WEIGHT(a,b) ((uint8_t) (((a)*(b)+(a)+(b))>>8))
  uint8_t wu[4] = {WU_WEIGHT(ix, iy), WU_WEIGHT(xx, iy),
                   WU_WEIGHT(ix, yy), WU_WEIGHT(xx, yy)
                  };
  // multiply the intensities by the colour, and saturating-add them to the pixels
  for (uint8_t i = 0; i < 4; i++) {
    int16_t xn = x + (i & 1), yn = y + ((i >> 1) & 1);
    CRGB clr = getPixColorXY(xn, yn);
    if (xn < (int)WIDTH - 1 && yn < (int)HEIGHT - 1 && yn > 0 && xn > 0) {
      clr.r = qadd8(clr.r, (color.r * wu[i]) >> 8);
      clr.g = qadd8(clr.g, (color.g * wu[i]) >> 8);
      clr.b = qadd8(clr.b, (color.b * wu[i]) >> 8);
    } else {
      clr.r = qadd8(clr.r, (color.r * 85) >> 8);
      clr.g = qadd8(clr.g, (color.g * 85) >> 8);
      clr.b = qadd8(clr.b, (color.b * 85) >> 8);
    }
    drawPixelXY(xn, yn, clr);
  }
#undef WU_WEIGHT
}*/
