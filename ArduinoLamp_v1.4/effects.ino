// ================================= ЭФФЕКТЫ ===========================================
// ------------------------------ Конфетти ---------------------------------------------
#define FADE_OUT_SPEED        (70U)                         // скорость затухания
//--------------------------------Шторм,Метель------------------------------------------
#define SNOW_DENSE (32U) // плотность снега - меньше = плотнее
//--------------------------------Светлячки со шлейфом-----------------------------------------
#define BALLS_AMOUNT          (3U)     // количество "шариков"
//#define CLEAR_PATH            (1U)     // очищать путь
#define TRACK_STEP            (70U)    // длина хвоста шарика (чем больше цифра, тем хвост короче)
int16_t coord[BALLS_AMOUNT][2U];
int8_t vector[BALLS_AMOUNT][2U];
CRGB ballColors[BALLS_AMOUNT];
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
static const TProgmemRGBPalette16 StepkosColors_p FL_PROGMEM = {0x0000ff, 0x0f00f0, 0x1e00e1, 0x2d00d2, 0x3c00c3, 0x4b00b4, 0x5a00a5, 0x690096, 0x780087, 0x870078, 0x9600cd, 0xa50050, 0xb40041, 0xc30032, 0xd20023, 0xe10014};
static const TProgmemRGBPalette16 ZeebraColors_p FL_PROGMEM = {CRGB::White, CRGB::Black, CRGB::Black, CRGB::Black, CRGB::White, CRGB::Black, CRGB::Black, CRGB::Black, CRGB::White, CRGB::Black, CRGB::Black, CRGB::Black, CRGB::White, CRGB::Black, CRGB::Black, CRGB::Black};
// добавлено изменение текущей палитры (используется во многих эффектах ниже для бегунка Масштаб)
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
    &StepkosColors_p };
const TProgmemRGBPalette16 *curPalette = palette_arr[0];
void setCurrentPalette(){
      if (modes[currentMode].Scale > 100U) modes[currentMode].Scale = 100U; // чтобы не было проблем при прошивке без очистки памяти
      curPalette = palette_arr[(uint8_t)(modes[currentMode].Scale/100.0F*((sizeof(palette_arr)/sizeof(TProgmemRGBPalette16 *))-0.01F))];
}
// --------------------------------- конфетти ------------------------------------
void sparklesRoutine()
{
  for (uint8_t i = 0; i < modes[currentMode].Scale; i++)
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

// радуги 2D
// ------------- радуга вертикальная/горизонтальная ----------------
void rainbowHorVertRoutine(bool isVertical)
{
  for (uint8_t i = 0U; i < (isVertical?WIDTH:HEIGHT); i++)
  {
    CHSV thisColor = CHSV((uint8_t)(hue + i * modes[currentMode].Scale%170), 255, 255);
    for (uint8_t j = 0U; j < (isVertical?HEIGHT:WIDTH); j++)
    {
      drawPixelXY((isVertical?i:j), (isVertical?j:i), thisColor);
    }
  }
}

// ------------- радуга диагональная -------------

void RainbowRoutine()
{
  // коэф. влияния замаплен на скорость, 4 ползунок нафиг не нужен
  hue += (6.0 * (modes[currentMode].Speed / 255.0) + 0.05 ); // скорость смещения цвета зависит от кривизны наклна линии, коэф. 6.0 и 0.05

  if(modes[currentMode].Scale<85){
    rainbowHorVertRoutine(false);
  } else if (modes[currentMode].Scale>170){
    rainbowHorVertRoutine(true);
  } else{

  for (uint8_t i = 0U; i < WIDTH; i++)
  {
    for (uint8_t j = 0U; j < HEIGHT; j++)
    {
      float twirlFactor = fmap((float)modes[currentMode].Scale, 85, 170, 8.3, 24);      // на сколько оборотов будет закручена матрица, [0..3]
      CRGB thisColor = CHSV((uint8_t)(hue + ((float)WIDTH / HEIGHT * i + j * twirlFactor) * ((float)255 / maxDim)), 255, 255);
      drawPixelXY(i, j, thisColor);}
    }
  }
}


// ---------------------------------------- ЦВЕТА -----------------------------
void colorsRoutine() {
  hue += modes[currentMode].Scale;
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(hue, 255, 255);
  }
}

// --------------------------------- ЦВЕТ ------------------------------------
//@stepko
void colorRoutine() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(modes[currentMode].Scale * 2.5, modes[currentMode].Speed * 2.5, 255);
  }
}

// ------------------------------ снегопад 2.0 --------------------------------
void snowRoutine() {
  shiftDown();

  for (byte x = 0; x < WIDTH; x++) {
    // заполняем случайно верхнюю строку
    // а также не даём двум блокам по вертикали вместе быть
    if (getPixColorXY(x, HEIGHT - 2) == 0 && (random(0, modes[currentMode].Scale) == 0))
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
      else if (thisColor == 0U && random((100 - modes[currentMode].Scale) * HEIGHT) == 0U)  // если наш пиксель ещё не горит, иногда зажигаем новые цепочки
        //else if (thisColor == 0U && random((100 - modes[currentMode].Scale) * HEIGHT*3) == 0U)  // для длинных хвостов
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
      if (random(100 - modes[currentMode].Scale) == 0U)
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
    
    uint8_t fullRows =  centerY / 100.0 * modes[currentMode].Scale;
    uint8_t iPol = (centerY / 100.0 * modes[currentMode].Scale - fullRows) * 255;
    
    for (int16_t y = centerY; y >= 0; y--)
    {
      CRGB color = CHSV(
                     45U,                                                                              // определяем тон
                     map(modes[currentMode].Speed, 0U, 255U, 0U, 170U),                                // определяем насыщенность
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
      leds[getPixelNumber(0, i)] = CHSV(random(0, 200), modes[currentMode].Scale, 255);
  }
  for (byte i = 0; i < WIDTH / 2; i++) {
    if (getPixColorXY(i, HEIGHT - 1) == 0
        && (random(0, SNOW_DENSE) == 0)
        && getPixColorXY(i + 1, HEIGHT - 1) == 0
        && getPixColorXY(i - 1, HEIGHT - 1) == 0)
      leds[getPixelNumber(i, HEIGHT - 1)] = CHSV(random(0, 200), modes[currentMode].Scale, 255);
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

//-------------------------Блуждающий кубик-----------------------
int16_t coordB[2U];
int8_t vectorB[2U];
CRGB ballColor;

void ballRoutine()
{
  if (loadingFlag)
  {
    loadingFlag = false;
  
  if (modes[currentMode].Scale <= 85) 
    deltaValue = map(modes[currentMode].Scale, 1, 85, 1U, max((uint8_t)min(WIDTH,HEIGHT) / 3, 1));
  else if (modes[currentMode].Scale > 85 and modes[currentMode].Scale <= 170)
    deltaValue = map(modes[currentMode].Scale, 170, 86, 1U, max((uint8_t)min(WIDTH,HEIGHT) / 3, 1));
  else
    deltaValue = map(modes[currentMode].Scale, 171, 255, 1U, max((uint8_t)min(WIDTH,HEIGHT) / 3, 1));
    
    for (uint8_t i = 0U; i < 2U; i++)
    {
      coordB[i] = WIDTH / 2 * 10;
      vectorB[i] = random(8, 20);
    }
    ballColor = CHSV(random(0, 9) * 28, 255U, 255U);
  }

 
  if ((modes[currentMode].Scale & 0x01))
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
  
if (modes[currentMode].Scale <= 85)  // при масштабе до 85 выводим кубик без шлейфа
    memset8( leds, 0, NUM_LEDS * 3);
  else if (modes[currentMode].Scale > 85 and modes[currentMode].Scale <= 170)
    fadeToBlackBy(leds, NUM_LEDS, 255 - (10 * (modes[currentMode].Speed) /255) + 30); // выводим кубик со шлейфом, длинна которого зависит от скорости.
  else
    fadeToBlackBy(leds, NUM_LEDS, 255 - (10 * (modes[currentMode].Speed) /255) + 15); // выводим кубик с длинным шлейфом, длинна которого зависит от скорости.

     
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
      ballColors[j] = CHSV((modes[currentMode].Scale * (j + 1)) % 256U, 255U, 255U);
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



void Fire2020() {
  //CRGBPalette16 myPal = firepal;
  uint8_t speedy = map(modes[currentMode].Speed, 1, 255, 255, 0);
  uint8_t _scale = modes[currentMode].Scale + 30;

  uint32_t a = millis();
  for (byte i = 0U; i < WIDTH; i++) {
    for (float j = 0.; j < HEIGHT; j++) {
      //leds[XY((LED_COLS - 1) - i, (LED_ROWS - 1) - j)] = ColorFromPalette(*curPalette/*myPal*/, qsub8(inoise8(i * scale, j * scale + a, a / speed), abs8(j - (LED_ROWS - 1)) * 255 / (LED_ROWS - 1)), 255);
//      if(curPalette!=palettes.at(10))
        drawPixelXY((WIDTH - 1) - i, (HEIGHT - 1) - j, ColorFromPalette(HeatColors_p/*myPal*/, qsub8(inoise8(i * _scale, j * _scale + a, a / speedy), abs8(j - (HEIGHT - 1)) * 255 / (HEIGHT - 1)), 255));
//      else
//        myLamp.drawPixelXYF_Y((LED_COLS - 1) - i, (float)(LED_ROWS - 1) - j, ColorFromPalette(HeatColors2_p/*myPal*/, qsub8(inoise8(i * _scale, j * _scale + a, a / speedy), abs8(j - (LED_ROWS - 1)) * 255 / (LED_ROWS - 1)), 255));
    }
  }
}

// ---- Эффект "Узоры"
// https://github.com/vvip-68/GyverPanelWiFi/blob/master/firmware/GyverPanelWiFi_v1.02/patterns.ino
    uint8_t patternIdx = -1;
    int8_t lineIdx = 0;
    bool dir = true;


    CHSV colorMR[8] = {
        CHSV(0, 0, 0),              // 0 - Black
        CHSV(HUE_RED, 255, 255),    // 1 - Red
        CHSV(HUE_GREEN , 255, 255),  // 2 - Green
        CHSV(HUE_BLUE, 255, 255),   // 3 - Blue
        CHSV(HUE_YELLOW, 255, 255), // 4 - Yellow
        CHSV(0, 0, 220),            // 5 - White
        CHSV(0, 255, 255),              // 6 - плавно меняеться в цикле (фон)
        CHSV(0, 255, 255),              // 7 - цвет равен 6 но +64
    };



#define MAX_PATTERN 35
typedef uint8_t Pattern[10][10];

static const Pattern patterns[] PROGMEM = {
{// 0 зигзаг ********
{6,6,6,6,6,7,7,7,7,7},  
{7,6,6,6,6,6,7,7,7,7},
{7,7,6,6,6,6,6,7,7,7},
{7,7,7,6,6,6,6,6,7,7},
{7,7,7,7,6,6,6,6,6,7},
{7,7,7,7,7,6,6,6,6,6},
{7,7,7,7,6,6,6,6,6,7},
{7,7,7,6,6,6,6,6,7,7},
{7,7,6,6,6,6,6,7,7,7},
{7,6,6,6,6,6,7,7,7,7}, 
},
{// 1 ноты ********* белые на цветном фоне
{6,6,6,6,6,5,5,5,6,6},
{6,6,6,6,6,5,6,6,5,6},
{6,6,6,6,6,5,6,6,6,6},
{6,6,6,6,6,5,6,6,6,6},
{6,6,6,6,6,5,6,6,6,6},
{6,6,5,5,5,5,6,6,6,6},
{6,5,5,5,5,5,6,6,6,6},
{6,6,5,5,5,6,6,6,6,6},
{6,6,6,6,6,6,6,6,6,6},
{6,6,6,6,6,5,6,6,6,6},
},
{// 3 сердце *********
{6,6,6,6,6,6,6,6,6,6},
{6,6,1,1,6,6,6,1,1,6},
{6,1,1,1,1,6,1,1,1,1},
{6,1,1,1,1,1,1,1,1,1},
{6,1,1,1,1,1,1,1,1,1},
{6,6,1,1,1,1,1,1,1,6},
{6,6,1,1,1,1,1,1,1,6},
{6,6,6,1,1,1,1,1,6,6},
{6,6,6,6,1,1,1,6,6,6}, 
{6,6,6,6,6,1,6,6,6,6},
},
{// 4 Маска ********
{6,6,6,6,5,5,6,6,6,6},
{6,5,5,6,6,6,6,5,5,6},
{6,5,6,6,5,5,6,6,5,6},
{6,6,6,6,6,6,6,6,6,6},
{5,6,5,6,5,5,6,5,6,5},
{5,6,5,6,5,5,6,5,6,5},
{6,6,6,6,6,6,6,6,6,6},
{6,5,6,6,5,5,6,6,5,6},
{6,5,5,6,6,6,6,5,5,6},
{6,6,6,6,5,5,6,6,6,6},
},
{// 5 клеточка *********
{6,6,6,7,7,7,7,7,7,6},
{6,6,6,6,7,7,7,7,6,6},
{6,6,6,6,6,7,7,6,6,6},
{7,7,6,6,6,6,6,6,6,6},
{7,7,7,6,6,6,6,6,6,7},
{7,7,7,7,6,6,6,6,7,7},
{7,7,7,6,6,6,6,6,6,7},
{7,7,6,6,6,6,6,6,6,6},
{6,6,6,6,6,7,7,6,6,6},
{6,6,6,6,7,7,7,7,6,6}, 
},
{// 6 смайлик********* желтый на зеленом
{6,6,6,6,6,6,6,6,6,6},
{6,6,4,4,4,4,4,6,6,6},
{6,4,4,1,4,1,4,4,6,6},
{6,4,4,4,4,4,4,4,6,6},
{6,4,4,4,4,4,4,4,6,6},
{6,4,1,4,4,4,1,4,6,6},
{6,4,4,1,1,1,4,4,6,6},
{6,6,4,4,4,4,4,6,6,6},
{6,6,6,6,6,6,6,6,6,6},
{6,6,6,6,6,6,6,6,6,6}, 
},
{// 7 ********** зигзаг
{6,0,7,7,7,7,7,0,6,6},
{6,6,0,7,7,7,0,6,6,6},
{6,6,6,0,7,0,6,6,6,6},
{6,6,6,6,0,6,6,6,6,0},
{0,6,6,6,6,6,6,6,0,7},
{7,0,6,6,6,6,6,0,7,7},
{7,7,0,6,6,6,0,7,7,7},
{7,7,7,0,6,0,7,7,7,7},
{7,7,7,7,0,7,7,7,7,0},
{0,7,7,7,7,7,7,7,0,6},
},
{// 8 ********* полосы
{6,6,6,6,6,6,7,7,7,7},
{7,6,6,6,6,6,6,7,7,7},
{7,7,6,6,6,6,6,6,7,7},
{7,7,7,6,6,6,6,6,6,7},
{7,7,7,7,6,6,6,6,6,6},
{6,7,7,7,7,6,6,6,6,6},
{6,6,7,7,7,7,6,6,6,6},
{6,6,6,7,7,7,7,6,6,6},
{6,6,6,6,7,7,7,7,6,6},
{6,6,6,6,6,7,7,7,7,6},
},
{// 11 Вышиванка ********
{1,5,5,5,0,0,5,5,5,1},
{5,1,1,0,5,5,0,1,1,5},
{5,5,0,1,5,5,1,0,5,5},
{5,0,5,1,5,5,1,5,0,5},
{0,5,5,5,1,1,5,5,5,0},
{0,5,5,5,1,1,5,5,5,0},
{5,0,5,1,5,5,1,5,0,5},
{5,5,0,1,5,5,1,0,5,5},
{5,1,1,0,5,5,0,1,1,5},
{1,5,5,5,0,0,5,5,5,1},
},
{// 12 ********* плетёнка
{7,7,7,7,7,7,7,7,7,6},
{7,7,7,7,7,7,7,7,6,7},
{7,7,7,7,7,7,7,6,6,6},
{6,7,7,7,7,7,6,6,6,6},
{6,6,7,7,7,6,7,6,6,6},
{6,6,6,7,6,7,7,7,6,6},
{6,6,6,6,7,7,7,7,7,6},
{6,6,6,7,7,7,7,7,7,7},
{7,6,7,7,7,7,7,7,7,7},
{6,7,7,7,7,7,7,7,7,7},
},
{//14 Ромбы ********
{0,6,6,6,6,6,6,6,6,0},
{7,0,6,6,6,6,6,6,0,7},
{7,7,0,6,6,6,6,0,7,7},
{7,7,7,0,6,6,0,7,7,7},
{7,7,7,7,0,0,7,7,7,7},
{7,7,7,7,0,0,7,7,7,7},
{7,7,7,0,6,6,0,7,7,7},
{7,7,0,6,6,6,6,0,7,7},
{7,0,6,6,6,6,6,6,0,7},
{0,6,6,6,6,6,6,6,6,0},
},
{//15 Dont Know ********
{6,6,6,6,6,5,5,5,5,5},
{6,5,5,5,6,5,7,7,7,5},
{6,5,6,5,6,5,7,5,7,5},
{6,5,5,5,6,5,7,7,7,5},
{6,6,6,6,6,5,5,5,5,5},
{5,5,5,5,5,6,6,6,6,6},
{5,7,7,7,5,6,5,5,5,6},
{5,7,5,7,5,6,5,6,5,6},
{5,7,7,7,5,6,5,5,5,6},
{5,5,5,5,5,6,6,6,6,6},
},
{// 16 смайлик********* с очечями
{0,0,0,0,0,0,0,0,0,0},
{0,0,6,6,6,6,6,0,0,0},
{5,5,5,5,5,5,5,5,5,0},
{0,6,5,5,6,5,5,6,0,0},
{0,6,6,6,6,6,6,6,0,0},
{0,6,0,6,6,6,0,6,0,0},
{0,6,6,0,0,0,6,6,0,0},
{0,0,6,6,6,6,6,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
},
{// 17 Круги ********
{6,6,6,5,5,5,5,6,6,6},
{6,6,5,6,6,6,6,5,6,6},
{6,5,6,6,0,0,6,6,5,6},
{5,6,6,0,6,6,0,6,6,5},
{5,6,0,6,7,7,6,0,6,5},
{5,6,0,6,7,7,6,0,6,5},
{5,6,6,0,6,6,0,6,6,5},
{6,5,6,6,0,0,6,6,5,6},
{6,6,5,6,6,6,6,5,6,6},
{6,6,6,5,5,5,5,6,6,6},
},
{// 18 Маска ********
{7,7,7,7,7,7,7,7,6,7},
{6,6,6,6,6,6,6,6,6,7},
{7,6,7,7,7,7,6,7,6,7},
{7,6,6,6,6,6,6,7,6,7},
{7,6,7,6,7,7,6,7,6,7},
{7,6,7,6,7,7,6,7,6,7},
{7,6,7,6,6,6,6,6,6,7},
{7,6,7,6,7,7,7,7,6,7},
{7,6,6,6,6,6,6,6,6,6},
{7,6,7,7,7,7,7,7,7,7},
},
{// 21 Маска4 ********
{6,6,6,6,7,7,6,6,6,6},
{6,7,6,6,6,6,6,6,7,6},
{6,6,6,6,6,6,6,6,6,6},
{6,6,6,7,6,6,7,6,6,6},
{7,6,6,6,6,6,6,6,6,7},
{7,6,6,6,6,6,6,6,6,7},
{6,6,6,7,6,6,7,6,6,6},
{6,6,6,6,6,6,6,6,6,6},
{6,7,6,6,6,6,6,6,7,6},
{6,6,6,6,7,7,6,6,6,6},
},
{// 23 Маска6 ********
{7,6,6,7,6,6,7,6,6,7},
{6,7,7,6,7,7,6,7,7,6},
{6,7,6,6,6,6,6,6,7,6},
{7,6,6,7,6,6,7,6,6,7},
{6,7,6,6,6,6,6,6,7,6},
{6,7,6,6,6,6,6,6,7,6},
{7,6,6,7,6,6,7,6,6,7},
{6,7,6,6,6,6,6,6,7,6},
{6,7,7,6,7,7,6,7,7,6},
{7,6,6,7,6,6,7,6,6,7},
},
{// 26 Маска 9********
{9,0,0,9,0,0,9,0,0,9},
{0,0,0,0,9,9,0,0,0,0},
{0,0,0,0,9,9,0,0,0,0},
{9,0,0,9,0,0,9,0,0,9},
{0,9,9,0,0,0,0,9,9,0},
{0,9,9,0,0,0,0,9,9,0},
{9,0,0,9,0,0,9,0,0,9},
{0,0,0,0,9,9,0,0,0,0},
{0,0,0,0,9,9,0,0,0,0},
{9,0,0,9,0,0,9,0,0,9},
},
{// 27 Маска 10********
{0,7,7,0,0,0,0,7,7,0},
{7,7,7,0,7,7,0,7,7,7},
{7,7,7,0,7,7,0,7,7,7},
{0,0,0,7,7,7,7,0,0,0},
{0,7,7,7,0,0,7,7,7,0},
{0,7,7,7,0,0,7,7,7,0},
{0,0,0,7,7,7,7,0,0,0},
{7,7,7,0,7,7,0,7,7,7},
{7,7,7,0,7,7,0,7,7,7},
{0,7,7,0,0,0,0,7,7,0},
},
{// 28 Маска 11********
{7,7,7,6,6,6,6,7,7,7},
{7,6,6,6,6,6,6,6,6,7},
{7,6,7,7,6,6,7,7,6,7},
{6,6,7,7,7,7,7,7,6,6},
{6,6,6,7,6,6,7,6,6,6},
{6,6,6,7,6,6,7,6,6,6},
{6,6,7,7,7,7,7,7,6,6},
{7,6,7,7,6,6,7,7,6,7},
{7,6,6,6,6,6,6,6,6,7},
{7,7,7,6,6,6,6,7,7,7},
},
{// 30 Маска 13********
{7,7,6,6,7,7,6,6,7,7},
{7,7,6,6,7,7,6,6,7,7},
{6,6,6,6,6,6,6,6,6,6},
{6,6,6,6,6,6,6,6,6,6},
{7,7,6,6,7,7,6,6,7,7},
{7,7,6,6,7,7,6,6,7,7},
{6,6,6,6,6,6,6,6,6,6},
{6,6,6,6,6,6,6,6,6,6},
{7,7,6,6,7,7,6,6,7,7},
{7,7,6,6,7,7,6,6,7,7},
},
{// 31 Маска 14********
{7,7,6,6,7,7,6,6,7,7},
{7,6,6,6,7,7,6,6,6,7},
{6,6,7,6,7,7,6,7,6,6},
{6,6,6,6,7,7,6,6,6,6},
{7,7,7,7,6,6,7,7,7,7},
{7,7,7,7,6,6,7,7,7,7},
{6,6,6,6,7,7,6,6,6,6},
{6,6,7,6,7,7,6,7,6,6},
{7,6,6,6,7,7,6,6,6,7},
{7,7,6,6,7,7,6,6,7,7},
},
{// 32 Маска 15********
{7,7,7,7,7,7,7,7,7,7},
{7,6,7,6,6,6,6,7,6,7},
{7,7,7,7,7,7,7,7,7,7},
{7,6,7,6,6,6,6,7,6,7},
{7,6,7,6,7,7,6,7,6,7},
{7,6,7,6,7,7,6,7,6,7},
{7,6,7,6,6,6,6,7,6,7},
{7,7,7,7,7,7,7,7,7,7},
{7,6,7,6,6,6,6,7,6,7},
{7,7,7,7,7,7,7,7,7,7},
},
{// 33 Маска 16********
{6,7,6,6,6,6,6,6,7,6},
{7,6,7,7,7,7,7,7,6,7},
{6,7,6,6,6,6,6,6,7,6},
{6,7,6,7,7,7,7,6,7,6},
{6,7,6,7,6,6,7,6,7,6},
{6,7,6,7,6,6,7,6,7,6},
{6,7,6,7,7,7,7,6,7,6},
{6,7,6,6,6,6,6,6,7,6},
{7,6,7,7,7,7,7,7,6,7},
{6,7,6,6,6,6,6,6,7,6},
},
{// 35 Маска 18********
{7,6,6,7,6,6,7,6,6,7},
{6,7,6,6,6,6,6,6,7,6},
{6,6,7,6,6,6,6,7,6,6},
{7,6,6,7,6,6,7,6,6,7},
{6,6,6,6,7,7,6,6,6,6},
{6,6,6,6,7,7,6,6,6,6},
{7,6,6,7,6,6,7,6,6,7},
{6,6,7,6,6,6,6,7,6,6},
{6,7,6,6,6,6,6,6,7,6},
{7,6,6,7,6,6,7,6,6,7},
},
};
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
        drawPixelXY(X+x,Y+H-y, color2); 
      }
    }
  }
}

void patternsRoutine() {
  

  if (loadingFlag) {
    loadingFlag = false;
    patternIdx = map(modes[currentMode].Scale, 1U, MAX_PATTERN + 1, -1, MAX_PATTERN);  // мапим к ползунку масштаба
    if (patternIdx < 0) {
      patternIdx = random8(0U, MAX_PATTERN); 
    }
    //fadeToBlackBy(leds, NUM_LEDS, 25);
    if (dir) 
      lineIdx = 9;         // Картинка спускается сверху вниз - отрисовка с нижней строки паттерна (паттерн 10x10)
    else 
      lineIdx = 0;         // Картинка поднимается сверху вниз - отрисовка с верхней строки паттерна
    // Цвета с индексом 6 и 7 - случайные, определяются в момент настройки эффекта
    colorMR[6] = CHSV(random8(), 255U, 255U);
    colorMR[7].hue = colorMR[6].hue + 96; //(beatsin8(1, 0, 255, 0, 127), 255U, 255U);
      } 
  drawPattern(patternIdx, 0, 0, 10, 10, dir);
  EVERY_N_MILLIS((1005000U / modes[currentMode].Speed)){
    if (modes[currentMode].Scale == 1) 
    dir=!dir;
    loadingFlag = true;
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
      ball[i][2] = (float)random8(5, 11) / (257U - modes[currentMode].Speed) / 4.0;
      ball[i][0] = 0;
      ball[i][1] = i * 2U + random8(2);//random(0,WIDTH);
      if ( ball[i][2] == 0)
        ball[i][2] = 1;
      setCurrentPalette();
    }
    loadingFlag = false;
  }
  dimAll(100);
  blurScreen(20);
  // Bounce three balls around
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
      ball[i][2] = (float)random8(5, 11) / (257U - modes[currentMode].Speed) / 4.0;
      ball[i][0] = 0.01;
    }
    else if (ball[i][0] > HEIGHT - 1.01) {    // тоже на всякий пожарный
      ball[i][2] = (float)random8(5, 11) / (257U - modes[currentMode].Speed) / 4.0;
      ball[i][2] = -ball[i][2];
      ball[i][0] = HEIGHT - 1.01;
    }}}

    
