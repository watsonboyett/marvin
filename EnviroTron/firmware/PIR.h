#pragma once

#include <Arduino.h>

#define MOTION_PIN (D5)

void PIR_Setup();

bool PIR_AnyMovementOccurred();
