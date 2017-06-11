#pragma once

#include "PIR.h"
#include <Arduino.h>

bool movement_detected = false;
bool movement_read = true;

void PIR_StateChanged()
{
  // Get PIR sensor output state
  movement_detected = digitalRead(MOTION_PIN);
  movement_read = false;
}

void PIR_Setup()
{
  pinMode(MOTION_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(MOTION_PIN), PIR_StateChanged, RISING);
}

bool PIR_AnyMovementOccurred()
{
  movement_read = true;
  if (movement_detected)
  {
    movement_detected = false;
    return true;
  }
  return false;
}



