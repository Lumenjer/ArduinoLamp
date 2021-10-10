#ifndef _UTILITY_H
#define _UTILITY_H

#include "main.h"
//Функции
void dimAll(uint8_t value);

float fmap(const float x, const float in_min, const float in_max, const float out_min, const float out_max);

// залить все
void fillAll(CRGB color);
// функция отрисовки точки по координатам X Y
void drawPixelXY(uint8_t x, uint8_t y, CRGB color);

// функция получения цвета пикселя по его номеру
uint32_t getPixColor(uint32_t thisSegm);
// функция получения цвета пикселя в матрице по его координатам
uint32_t getPixColorXY(uint8_t x, uint8_t y);

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
uint16_t XY(uint8_t x, uint8_t y);
#else
uint16_t XY(uint8_t x, uint8_t y);
#endif
// оставлено для совместимости со эффектами из старых прошивок
uint16_t getPixelNumber(uint8_t x, uint8_t y);
//Субпиксель по Y
void drawPixelXYF_Y(uint16_t x, float y, const CRGB &color);
#endif // _UTILITY_H