#pragma once

#include <Arduino.h>

#define MOTION_PIN (D6)

void PIR_Setup();

void PIR_EnableInterrupt(bool enable);

bool PIR_AnyMovementOccurred();
