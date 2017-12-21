
#include "Sensors.h"
#include "PIR.h"
#include "Mic.h"
#include "LTR-329.h"
#include "SHT21.h"
#include "LPS22HB.h"
#include "I2C_Utils.h"
#include "common.h"

#include <Arduino.h>


void SENSORS_Init()
{
  I2C_Setup();
  PIR_Setup();
  MIC_Setup();
  ALS_Setup();
  SHT_Setup();
  LPS_Setup();
}

void SENSORS_EnableInterrupts()
{
  PIR_EnableInterrupt(true);
  MIC_EnableSampling(true);
  interrupts();
}

void SENSORS_DisableInterrupts()
{
  PIR_EnableInterrupt(false);
  MIC_EnableSampling(false);
  noInterrupts();
}

const float avg_a = 0.1;

SensorData_t sd;

//bool motion_inst = false;
//bool motion_avg = false;
//
//float als_visible_inst = 0;
//float als_visible_avg = 0;
//float als_infrared_inst = 0;
//
//float sht_temp_inst = 0;
//float sht_temp_avg = 0;
//float sht_rh_inst = 0;
//float sht_rh_avg = 0;
//
//float lps_pres_inst = 0;
//float lps_pres_avg = 0;
//float lps_temp_inst = 0;
//float lps_temp_avg = 0;
//
//float mic_level_inst = 0;
//float mic_level_avg = 0;
//float mic_level_max = 0;

uint32_t sample_count = 0;

void update_sensor_averages()
{
  if (sample_count > 1)
  {
    sd.motion_avg = sd.motion_avg || sd.motion_inst;
    sd.light_avg = calc_exponential_avg(sd.light_avg, sd.light_inst, avg_a);
    sd.ir_avg = calc_exponential_avg(sd.ir_avg, sd.ir_inst, avg_a);
    sd.temp_avg = calc_exponential_avg(sd.temp_avg, sd.temp_inst, avg_a);
    sd.rel_hum_avg = calc_exponential_avg(sd.rel_hum_avg, sd.rel_hum_inst, avg_a);
    sd.pres_avg = calc_exponential_avg(sd.pres_avg, sd.pres_inst, avg_a);
    sd.temp_alt_avg = calc_exponential_avg(sd.temp_alt_avg, sd.temp_alt_inst, avg_a);
    sd.sound_avg = calc_exponential_avg(sd.sound_avg, sd.sound_inst, avg_a);
  }
  else
  {
    // initialize average values
    sd.light_avg = sd.light_inst;
    sd.ir_avg = sd.ir_inst;
    sd.temp_avg = sd.temp_inst;
    sd.rel_hum_avg = sd.rel_hum_inst;
    sd.pres_avg = sd.pres_inst;
    sd.temp_alt_avg = sd.temp_alt_inst;
    sd.sound_avg = sd.sound_inst;
  }
}

void SENSORS_Update()
{
  sample_count++;

  sd.motion_inst = PIR_AnyMovementOccurred();

  ALS_MeasureLight();
  sd.ir_inst = ALS_GetIrLightLevel();
  sd.light_inst = ALS_GetAmbientLightLevel();

  SHT_MeasureTemperature();
  sd.temp_inst = SHT_GetTemperature_F();
  sd.rel_hum_inst = SHT_GetRelHumidity_pct();

  LPS_MeasurePressure();
  sd.pres_inst = LPS_GetPressure_mbar();
  sd.temp_alt_inst = LPS_GetTemperature_F();

  sd.sound_inst = MIC_GetAvgLevel_SPL();
  float mic_level_max_new = MIC_GetMaxLevel_SPL();
  sd.sound_max = max_val(sd.sound_max, mic_level_max_new);
  MIC_ResetMinMaxLevels();

  update_sensor_averages();
}

void SENSORS_ClearHoldValues()
{
  sd.motion_avg = false;
  sd.sound_max = 0;
}

SensorData_t SENSORS_GetSensorData()
{
  return sd;
}

char inst_str[256];
char * SENSORS_GetInstString()
{
  char temp_str[8];
  dtostrf(sd.temp_inst, 0, 2, temp_str);
  char rh_str[8];
  dtostrf(sd.rel_hum_inst, 0, 2, rh_str);

  char press_str[8];
  dtostrf(sd.pres_inst, 0, 2, press_str);
  char temp_alt_str[8];
  dtostrf(sd.temp_alt_inst, 0, 2, temp_alt_str);

  char light_str[9];
  dtostrf(sd.light_inst, 0, 2, light_str);
  char ir_str[9];
  dtostrf(sd.ir_inst, 0, 2, ir_str);

  char spl_str[8];
  dtostrf(sd.sound_inst, 0, 2, spl_str);
  char spl_max_str[8];
  dtostrf(sd.sound_max, 0, 2, spl_max_str);

  sprintf(inst_str, "sample=%u,temp=%s,humidity=%s,pressure=%s,temp_alt=%s,light=%s,ir=%s,motion=%d,sound=%s,sound_max=%s",
          sample_count, temp_str, rh_str, press_str, temp_alt_str, light_str, ir_str, sd.motion_inst, spl_str, spl_max_str);

  return inst_str;
}

char avg_str[256];
char * SENSORS_GetAvgString()
{
  char temp_str[8];
  dtostrf(sd.temp_avg, 0, 2, temp_str);
  char rh_str[8];
  dtostrf(sd.rel_hum_avg, 0, 2, rh_str);

  char press_str[8];
  dtostrf(sd.pres_avg, 0, 2, press_str);
  char temp_alt_str[8];
  dtostrf(sd.temp_alt_avg, 0, 2, temp_alt_str);

  char light_str[9];
  dtostrf(sd.light_avg, 0, 2, light_str);

  char spl_str[8];
  dtostrf(sd.sound_avg, 0, 2, spl_str);
  char spl_max_str[8];
  dtostrf(sd.sound_max, 0, 2, spl_max_str);

  sprintf(avg_str, "sample=%u,temp=%s,humidity=%s,pressure=%s,temp_alt=%s,light=%s,motion=%d,sound_avg=%s,sound_max=%s",
          sample_count, temp_str, rh_str, press_str, temp_alt_str, light_str, sd.motion_avg, spl_str, spl_max_str);

  return avg_str;
}


