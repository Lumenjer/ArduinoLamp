#ifndef __CONTROL_H
#define __CONTROL_H
#include "main.h"


void changePower();

void GoToEffect(byte Mode);

void SaveSettings();

void DeleteSettings();

extern boolean runningFlag;
#ifdef USE_TEXT
extern String runningText;
#endif


#ifdef USE_BT

void debugPrint();
// ********************* ПРИНИМАЕМ ДАННЫЕ **********************
void parsing();
void BTTick();
#endif
//------------------------------------------------------------------------------
#ifdef USE_BUTTON
void ButtonTick();
#endif

// ----- IR REMOTE ----- from MusicColor v2 by AlexGyver
//--------------------------------------------------------------------------------------
#ifdef USE_IR
void IRTick();
#endif

void controlTick();

void SetUP();
#endif