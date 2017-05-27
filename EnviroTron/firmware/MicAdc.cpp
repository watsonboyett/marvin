#pragma once

#include "MicAdc.h"
#include <Ticker.h>
#include <Arduino.h>

Ticker MIC_timer;

float MIC_voltage_inst = 0;
float MIC_voltage_avg = 0;
const float MIC_avg_a = 0.1;
const float MIC_avg_a1 = (1 - MIC_avg_a);

void MIC_MeasureMic()
{
  int sensorValue = analogRead(MIC_ADC_PIN);

  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 3.2V):
  MIC_voltage_inst = sensorValue * (3.2 / 1023.0);
  MIC_voltage_avg = (MIC_avg_a * MIC_voltage_inst) + (MIC_avg_a1 * MIC_voltage_avg);
}

void MIC_Setup()
{
  MIC_timer.attach_ms(10, MIC_MeasureMic);
}

float MIC_GetMicInstLevel_V()
{
  return MIC_voltage_inst;
}

float MIC_GetMicAvgLevel_V()
{
  return MIC_voltage_avg;
}
