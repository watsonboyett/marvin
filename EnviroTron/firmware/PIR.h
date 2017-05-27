#pragma once

#include <Arduino.h>

#define MOTION_PIN (D8)

void PIR_Setup();

bool PIR_AnyMovementOccurred();
