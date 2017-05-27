#pragma once

#include <Arduino.h>

#define MIC_ADC_PIN (A0)

void MIC_Setup();

float MIC_GetMicInstLevel_V();

float MIC_GetMicAvgLevel_V();
