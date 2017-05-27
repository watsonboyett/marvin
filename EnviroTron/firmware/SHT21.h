#pragma once

#include <Arduino.h>

void SHT_Setup();

float SHT_GetTemperature_C();

float SHT_GetTemperature_F();

float SHT_GetRelHumidity_pct();

void SHT_MeasureTemperature();
