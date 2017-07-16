#pragma once

#include "SHT21.h"
#include "I2C_Utils.h"
#include <Arduino.h>

#define SHT21_ADDR (0x40)

// RH = -6 + 125 * (S_RH / 2^16)
// T = -46.85 + 175.72 * (S_T / 2^16)

#define SHT21_MEAS_T_HOLD_ADDR    (0xE3)
#define SHT21_MEAS_RH_HOLD_ADDR   (0xE5)
#define SHT21_MEAS_T_NOHOLD_ADDR  (0xF3)
#define SHT21_MEAS_RH_NOHOLD_ADDR (0xF5)
#define SHT21_USER_REG_ADDR       (0xE6)
#define SHT21_SOFT_RESET_ADDR     (0xFE)

typedef union
{
  uint8_t bytes[3];
  struct
  {
    uint16_t data;
    uint8_t checksum;
  };
} SHT21_ADC_CONV_t;

typedef union
{
  uint8_t reg;
  struct
  {
    bool T_Resolution : 1;
    bool Disable_OTP_Reload : 1;
    uint8_t Reserved : 3;
    bool End_of_Battery : 1;
    bool RH_Resolution : 1;
  };
} SHT21_USER_REG_t;


void SHT_Setup()
{
  delay(20);
  
  I2C_WriteCmd(SHT21_ADDR, SHT21_MEAS_T_NOHOLD_ADDR);
  I2C_WriteCmd(SHT21_ADDR, SHT21_MEAS_RH_NOHOLD_ADDR);
}

// Temperature/humidity sensor data
SHT21_ADC_CONV_t SHT_temp = {0};
SHT21_ADC_CONV_t SHT_rh = {0};

float SHT_Temp_C = 0;
float SHT_GetTemperature_C()
{
  return SHT_Temp_C;
}

float SHT_Temp_F = 0;
float SHT_GetTemperature_F()
{
  return SHT_Temp_F;
}

float SHT_RH_pct = 0;
float SHT_GetRelHumidity_pct()
{
  return SHT_RH_pct;
}

float SHT_TemperatureOffset_C = -4.7;
float SHT_RelHumidityOffset_pct = 0;

void SHT_MeasureTemperature()
{
  // Get temperature data from sensor
  I2C_WriteCmd(SHT21_ADDR, SHT21_MEAS_T_NOHOLD_ADDR);
  delay(100);  // wait for conversion time to end
  I2C_ReadAddr(SHT21_ADDR, SHT_temp.bytes, 3);
  
  float temp_sens = (byteswap(SHT_temp.data) & 0xFFFC);
  SHT_Temp_C = -46.85 + 175.72 * (temp_sens / 65536) + SHT_TemperatureOffset_C;
  SHT_Temp_F = SHT_Temp_C * (9.0 / 5.0) + 32;

  // Get humidity data from sensor
  I2C_WriteCmd(SHT21_ADDR, SHT21_MEAS_RH_NOHOLD_ADDR);
  delay(50);  // wait for conversion time to end
  I2C_ReadAddr(SHT21_ADDR, SHT_rh.bytes, 3);
  
  float rh_sens = (byteswap(SHT_rh.data) & 0xFFFC);
  SHT_RH_pct = -6 + 125 * (rh_sens / 65536) + SHT_RelHumidityOffset_pct;
}



