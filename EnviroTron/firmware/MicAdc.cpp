#pragma once

#include "MicAdc.h"
#include "common.h"
#include "I2C_Utils.h"
#include <Ticker.h>
#include <Arduino.h>

Ticker MIC_timer;

const float MIC_adc_voltage_min = 0.0;  // in Volts
const float MIC_adc_voltage_max = 3.3;  // in Volts
const float MIC_adc_value_max = 4096.0;

const float MIC_voltage_offset = 1.65;  // in Volts
const float MIC_amp_gain = 46.24;   // in Volt / Volt
const float MIC_mic_sens = 125.89;  //  in Volt / Pascal
const float MIC_conv = MIC_mic_sens / MIC_amp_gain;  // conversion factor for Volts to Pascals

bool MIC_has_new_sample = false;

float MIC_voltage_inst = 0;
float MIC_voltage_min = 0;
float MIC_voltage_max = 0;

float MIC_voltage_avg = 0;
const float MIC_avg_a = 0.1;
const float MIC_avg_a1 = (1 - MIC_avg_a);

#define MCP3221_ADDR (0b01001101)

void MIC_MeasureLevel()
{
  //int sensorValue = analogRead(MIC_ADC_PIN);
  uint8_t rd_bytes[2] = {0};
  bool success = I2C_ReadAddr(MCP3221_ADDR, rd_bytes, 2);
  int sensorValue = ((int)rd_bytes[0] << 8 | rd_bytes[1]);

  // Convert the analog reading (which goes from 0 - 4096) to a voltage (0 - 3.3V):
  MIC_voltage_inst = sensorValue * (MIC_adc_voltage_max / MIC_adc_value_max);
  MIC_voltage_avg = calc_exponential_avg(MIC_voltage_avg, MIC_voltage_inst, MIC_avg_a);

  MIC_voltage_max = max(MIC_voltage_inst, MIC_voltage_max);
  MIC_voltage_min = min(MIC_voltage_inst, MIC_voltage_min);

  MIC_has_new_sample = true;
}

void MIC_EnableSampling(bool enable)
{
  if (enable)
  {
    MIC_timer.attach_ms(10, MIC_MeasureLevel);
  }
  else
  {
    MIC_timer.detach();
  }
}


void MIC_Setup()
{
  // initialize the average value (to improve "ramp-up" time)
  MIC_MeasureLevel();
  MIC_voltage_avg = MIC_voltage_inst;
  MIC_ResetMinMaxLevels();
}

bool MIC_HasNewSample()
{
  return MIC_has_new_sample;
}

float MIC_GetInstLevel_V()
{
  MIC_has_new_sample = false;
  return MIC_voltage_inst;
}

float MIC_GetMinLevel_V()
{
  MIC_has_new_sample = false;
  return MIC_voltage_min;
}

float MIC_GetMaxLevel_V()
{
  MIC_has_new_sample = false;
  return MIC_voltage_max;
}

float MIC_ResetMinMaxLevels()
{
  MIC_voltage_min = MIC_adc_voltage_max;
  MIC_voltage_max = MIC_adc_voltage_min;
}

float MIC_GetAvgLevel_V()
{
  return MIC_voltage_avg;
}

#define MIC_MIN_PRESSURE_Pa (1e-5)

float MIC_VoltToSPL(float v)
{
  // convert from volts to pascals (ensure pressure isn't 0 or the SPL will be inf)
  float p = abs(v - MIC_voltage_offset) * MIC_conv;
  p = max(p, MIC_MIN_PRESSURE_Pa);

  // convert from pascals to SPL (SPL is referenced to 20uPa, which is where the 50e3 factor comes from)
  float spl = 20 * log10(p * 50e3);
  return spl;
}

float MIC_GetInstLevel_SPL()
{
  return MIC_VoltToSPL(MIC_GetInstLevel_V());
}

float MIC_GetMinLevel_SPL()
{
  return MIC_VoltToSPL(MIC_GetMinLevel_V());
}

float MIC_GetMaxLevel_SPL()
{
  return MIC_VoltToSPL(MIC_GetMaxLevel_V());
}

float MIC_GetAvgLevel_SPL()
{
  return MIC_VoltToSPL(MIC_GetAvgLevel_V());
}

