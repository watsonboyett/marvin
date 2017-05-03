#pragma once

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

