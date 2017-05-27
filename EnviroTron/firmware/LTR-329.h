#pragma once

#include <Arduino.h>

void ALS_Setup();

void ALS_MeasureLight();

float ALS_GetAmbientLightLevel();

float ALS_GetIrLightLevel();

