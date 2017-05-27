#pragma once

#include "MS5607.h"
#include "I2C_Utils.h"
#include <Arduino.h>

// 1. send reset sequence (to make sure that calibration PROM gets loaded into internal registers)
// 2. read PROM data (to get calibration coefficients)
// 3. initiate conversion (D1 = uncompensated pressure, D2 = uncompensated temperature)
// 4. wait ~10ms for conversion to complete (incomplete conversion will return 0 as data result)
// 5. read from ADC_READ register for conversion result
// 6. repeat steps 3-5

// Temperature/Pressure compensation calculations
// dT = D2 - T_REF = D2 - C5 * 2^8
// TEMP (actual) = 20°C + dT * TEMPSENS = 2000 + dT * C6 / 2^23
// OFF = OFF_T1 + TCO * dT = C2 * 2^17 + (C4 * dT) / 2^5)
// SENS = SENS_T1 + TCS * dT = C1 * 2^16 + (C3 * dT) / 2^7
// P (actual) = D1 * SENS - OFF = (D1 * SENS / 2^21 - OFF) / 2^15

#define MS_ADDR (0x76)
#define MS_ALT_ADDR (0x77)

#define MS_RESET_ADDR (0x1E)

// pressure
#define MS_CONV_D1_OSR_256  (0x40)  // 0.130 mbar, Tc = 0.60 ms
#define MS_CONV_D1_OSR_512  (0x42)  // 0.084 mbar, Tc = 1.17 ms
#define MS_CONV_D1_OSR_1024 (0x44)  // 0.054 mbar, Tc = 2.28 ms
#define MS_CONV_D1_OSR_2048 (0x46)  // 0.036 mbar, Tc = 4.54 ms
#define MS_CONV_D1_OSR_4096 (0x48)  // 0.024 mbar, Tc = 9.04 ms

// temperature
#define MS_CONV_D2_OSR_256  (0x50)  // 0.012 °C, Tc = 0.60 ms
#define MS_CONV_D2_OSR_512  (0x52)  // 0.008 °C, Tc = 1.17 ms
#define MS_CONV_D2_OSR_1024 (0x54)  // 0.005 °C, Tc = 2.28 ms
#define MS_CONV_D2_OSR_2048 (0x56)  // 0.003 °C, Tc = 4.54 ms
#define MS_CONV_D2_OSR_4096 (0x58)  // 0.002 °C, Tc = 9.04 ms

#define MS_ADC_READ_ADDR (0x00)

typedef union
{
  uint8_t bytes[4];
  uint32_t data;

  struct 
  {
    uint8_t byte_1;
    uint8_t byte_2;
    uint8_t byte_3;
    uint8_t byte_4;
  };
} MS_ADC_READ_t;

#define MS_PROM_FACTORY_ADDR  (0xA0)
#define MS_PROM_SENS_T1_ADDR  (0xA2)
#define MS_PROM_OFF_T1_ADDR   (0xA4)
#define MS_PROM_TCS_ADDR      (0xA6)
#define MS_PROM_TCO_ADDR      (0xA8)
#define MS_PROM_T_REF_ADDR    (0xAA)
#define MS_PROM_TEMPSENS_ADDR (0xAC)
#define MS_PROM_CRC_ADDR      (0xAE)

typedef union
{
  uint8_t bytes[16];
  uint16_t data[8];

  struct
  {
    uint16_t FACTORY;
    uint16_t SENS_T1;   // (C1) pressure sensitivity
    uint16_t OFF_T1;    // (C2) pressure offset
    uint16_t TCS;       // (C3) temperature coefficient of pressure sensitivity
    uint16_t TCO;       // (C4) temperature coefficient of pressure offset
    uint16_t T_REF;     // (C5) reference temperature
    uint16_t TEMPSENS;  // (C6) temperature coefficient of the temperature
    uint16_t CRC;
  };

  struct
  {
    uint8_t FACTORY_l;
    uint8_t FACTORY_h;
    uint8_t SENS_T1_l;
    uint8_t SENS_T1_h;
    uint8_t OFF_T1_l;
    uint8_t OFF_T1_h;
    uint8_t TCS_l;
    uint8_t TCS_h;
    uint8_t TCO_l;
    uint8_t TCO_h;
    uint8_t T_REF_l;
    uint8_t T_REF_h;
    uint8_t TEMPSENS_l;
    uint8_t TEMPSENS_h;
    uint8_t CRC_l;
    uint8_t CRC_h;
  };
} MS_PROM_t;

