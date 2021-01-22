// ************* НАСТРОЙКИ *************
// The 16 bit version of our coordinates
static uint16_t x;
static uint16_t y;
static uint16_t z;

uint8_t speed = 20;                                        // speed is set dynamically once we've started up
uint8_t scale = 30;                                        // scale is set dynamically once we've started up

// This is the array that we keep our computed noise values in
#define MAX_DIMENSION (max(WIDTH, HEIGHT))
#if (WIDTH > HEIGHT)
uint8_t noise[WIDTH][WIDTH];
#else
uint8_t noise[HEIGHT][HEIGHT];
#endif


//------------Шум 3Д------------------
void Noise3D()
{
  if (loadingFlag)
  { setCurrentPalette(map(palette,1,255,1,20));
    loadingFlag = false;
    scale = map(modes[currentMode].Scale,1,10,1,100);
    speed = map(modes[currentMode].Scale,1,10,1,155);
    currentPalette = *curPalette;
  }
  if (palette<=9)
  fillNoiseLED();
  else{
    fillnoise8();
  for (uint8_t i = 0; i < WIDTH; i++)
  {
    for (uint8_t j = 0; j < HEIGHT; j++)
    {
      drawPixelXY(i, j, CHSV(noise[j][i], 255, noise[i][j]));
    }
  }
  hue += 1;
}}

// ************* СЛУЖЕБНЫЕ *************
void fillNoiseLED()
{
  uint8_t dataSmoothing = 0;
  if (speed < 50)
  {
    dataSmoothing = 200 - (speed * 4);
  }
  for (uint8_t i = 0; i < MAX_DIMENSION; i++)
  {
    int32_t ioffset = scale * i;
    for (uint8_t j = 0; j < MAX_DIMENSION; j++)
    {
      int32_t joffset = scale * j;

      uint8_t data = inoise8(x + ioffset, y + joffset, z);

      data = qsub8(data, 16);
      data = qadd8(data, scale8(data, 39));

      if (dataSmoothing)
      {
        uint8_t olddata = noise[i][j];
        uint8_t newdata = scale8( olddata, dataSmoothing) + scale8( data, 256 - dataSmoothing);
        data = newdata;
      }

      noise[i][j] = data;
    }
  }
  z += speed;
  x += speed / 8;
  y -= speed / 16;

  for (uint8_t i = 0; i < WIDTH; i++)
  {
    for (uint8_t j = 0; j < HEIGHT; j++)
    {
      uint8_t index = noise[j][i];
      uint8_t bri =   noise[i][j];
      // if this palette is a 'loop', add a slowly-changing base value
      if (modes[currentMode].Speed % 2)
      {
        index += hue;
      }
      // brighten up, as the color palette itself often contains the
      // light/dark dynamic range desired
      if ( bri > 127 )
      {
        bri = 255;
      }
      else
      {
        bri = dim8_raw( bri * 2);
      }
      CRGB color = ColorFromPalette( currentPalette, index, bri);
      drawPixelXY(i, j, color);
    }
  }
  hue += 1;
}

void fillnoise8()
{
  for (uint8_t i = 0; i < MAX_DIMENSION; i++)
  {
    int32_t ioffset = scale * i;
    for (uint8_t j = 0; j < MAX_DIMENSION; j++)
    {
      int32_t joffset = scale * j;
      noise[i][j] = inoise8(x + ioffset, y + joffset, z);
    }
  }
  z -= speed;
}
