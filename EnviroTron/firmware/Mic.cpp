
#include "Mic.h"
#include "common.h"
#include "MCP3221.h"
#include <Ticker.h>
#include <Arduino.h>

Ticker MIC_timer;

const float MIC_voltage_offset = 1.65;  // in Volts
const float MIC_amp_gain = 60.84;   // in Volt/Volt
const float MIC_mic_sens = 125.89;  //  in Pascal/Volt (-42 dB -> 10^(-42/20) = 7.943 mV/Pa)
const float MIC_conv = MIC_mic_sens / MIC_amp_gain;  // conversion factor for Volts to Pascals

bool MIC_has_new_sample = false;

float MIC_voltage_inst = 0;
float MIC_voltage_min = 0;
float MIC_voltage_max = 0;

float MIC_voltage_avg = 0;
const float MIC_avg_a = 0.1;


void MIC_MeasureLevel()
{
  MCP3221_MeasureLevel();
  float mic_v = MCP3221_GetLevel_V();

  MIC_voltage_inst = mic_v - MIC_voltage_offset;
  MIC_voltage_avg = calc_exponential_avg(MIC_voltage_avg, MIC_voltage_inst, MIC_avg_a);

  float v_abs = abs(MIC_voltage_inst);
  MIC_voltage_max = max(v_abs, MIC_voltage_max);
  MIC_voltage_min = min(v_abs, MIC_voltage_min);

  
  char v_str[12];
  dtostrf(v_abs, 0, 5, v_str);
  char db_str[12];
  dtostrf(MIC_VoltToSPL(v_abs), 0, 5, db_str);
  //Serial.printf("V: %s, SPL: %s \n", v_str, db_str);

  
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
  MIC_voltage_min = MCP3221_VOLTAGE_MAX;
  MIC_voltage_max = MCP3221_VOLTAGE_MIN;
}

float MIC_GetAvgLevel_V()
{
  return MIC_voltage_avg;
}

#define MIC_MIN_PRESSURE_Pa (20e-6)  // min threshold of hearing is 20uPa (0 dB SPL)

float MIC_VoltToSPL(float v)
{
  // convert from volts to pascals (ensure pressure isn't 0 or the SPL will be inf)
  float p = abs(v) * MIC_conv;
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

