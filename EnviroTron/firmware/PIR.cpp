#pragma once

#include "PIR.h"
#include <Arduino.h>

volatile bool pir_active = false;
bool movement_detected = false;
bool movement_ack = true;

void PIR_StateChanged()
{
  // Get PIR sensor output state
  pir_active = digitalRead(MOTION_PIN);
  movement_detected = pir_active || !movement_ack;
  movement_ack = !movement_detected;
}

void PIR_Setup()
{
  pinMode(MOTION_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(MOTION_PIN), PIR_StateChanged, CHANGE);
}

bool PIR_AnyMovementOccurred()
{
  movement_ack = true;
  if (movement_detected)
  {
    movement_detected = false;
    return true;
  }
  return false;
}



