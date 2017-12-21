#pragma once

#include <Arduino.h>

typedef struct
{
  bool motion_inst;
  bool motion_avg;
  float light_inst;
  float light_avg;
  float ir_inst;
  float ir_avg;
  float temp_inst = 0;
  float temp_avg = 0;
  float rel_hum_inst = 0;
  float rel_hum_avg = 0;
  float pres_inst = 0;
  float pres_avg = 0;
  float temp_alt_inst = 0;
  float temp_alt_avg = 0;
  float sound_inst = 0;
  float sound_avg = 0;
  float sound_max = 0;
} SensorData_t;

void SENSORS_Init();

void SENSORS_EnableInterrupts();

void SENSORS_DisableInterrupts();

void SENSORS_Update();

void SENSORS_ClearHoldValues();

SensorData_t SENSORS_GetSensorData();

char * SENSORS_GetInstString();

char * SENSORS_GetAvgString();

