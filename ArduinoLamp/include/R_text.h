#include "main.h"
#ifndef _R_TEXT_H
#define _R_TEXT_H

#ifdef USE_TEXT

// интерпретатор кода символа в массиве fontHEX (для Arduino IDE 1.8.* и выше)
uint8_t getFont(uint8_t font, uint8_t row);

void drawLetter(uint8_t index, uint8_t letter, int16_t offset, uint32_t color);

// ------------- СЛУЖЕБНЫЕ ФУНКЦИИ --------------


void fillString(String text, uint32_t color);
#endif
#endif // _R_TEXT_H