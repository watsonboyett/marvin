#pragma once

#include <Arduino.h>

#define MIC_ADC_PIN (A0)

void MIC_Setup();

void MIC_MeasureLevel();

void MIC_EnableSampling(bool enable);

bool MIC_HasNewSample();

float MIC_GetInstLevel_V();

float MIC_GetMinLevel_V();

float MIC_GetMaxLevel_V();

float MIC_ResetMinMaxLevels();

float MIC_GetAvgLevel_V();

float MIC_VoltToSPL(float v);

float MIC_GetInstLevel_SPL();

float MIC_GetMinLevel_SPL();

float MIC_GetMaxLevel_SPL();

float MIC_GetAvgLevel_SPL();
