#ifndef _EFFECTS_H_
#define _EFFECTS_H_
#include "main.h"

void setCurrentPalette(uint8_t palIdx);
void rainbowHorVertRoutine(bool isVertical);
void sparklesRoutine();
void RainbowRoutine();
void FireRoutine();
void sendVoxels();
void Noise3D();
void whiteLampRoutine();
void colorRoutine();
void colorsRoutine();
void matrixRoutine();
void RainRoutine();
void stormRoutine();
void ballRoutine();
void ballsRoutine();
void Nexus();
void matrixRoutine();
void sendVoxels();
void N_Dot_Reload(byte id);
void FillNoise(bool ShiftX, bool ShiftY, bool ShiftZ, CRGBPalette16 palette, bool ShiftHue, bool BriNoise);

#endif // _EFFECTS_H_
