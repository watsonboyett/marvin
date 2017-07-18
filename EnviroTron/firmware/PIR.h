#pragma once

#include <Arduino.h>

#define MOTION_PIN (D6)

void PIR_Setup();

bool PIR_AnyMovementOccurred();
