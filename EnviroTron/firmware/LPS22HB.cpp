#pragma once

#include "LPS22HB.h"
#include "I2C_Utils.h"
#include <Arduino.h>

#define LPS_ADDR (0x5C)

#define LPS_INTERRUPT_CFG_ADDR (0x0B)

typedef union
{
  uint8_t reg;

  struct
  {
    bool PHE : 1;
    bool PLE : 1;
    bool LIR : 1;
    bool DIFF_EN : 1;
    bool RESET_AZ : 1;
    bool AUTOZERO : 1;
    bool RESET_ARP : 1;
    bool AUTOFRIP : 1;
  };
} LPS_INTERRUPT_CFG_t;


#define LPS_THS_P_L_ADDR  (0x0C)
#define LPS_THS_P_H_ADDR  (0x0D)

typedef union
{
  uint8_t bytes[2];
  uint16_t value;

  struct
  {
    uint8_t byte_low;
    uint8_t byte_high;
  };
} LPS_THS_P_t;


#define LPS_WHO_AM_I_ADDR  (0x0F)

typedef struct
{
  uint8_t reg;
} LPS_WHO_AM_I_T;


#define LPS_CTRL_REG1_ADDR (0x10)

typedef union
{
  uint8_t reg;

  struct
  {
    bool SIM : 1;
    bool BDU : 1;
    bool LPFP_CFG : 1;
    bool EN_LPFP : 1;
    uint8_t ODR : 3;
    bool zero : 1;
  };
} LPS_CTRL_REG1_t;


#define LPS_CTRL_REG2_ADDR (0x11)

typedef union
{
  uint8_t reg;

  struct
  {
    bool ONE_SHOT : 1;
    bool zero : 1;
    bool SWRESET : 1;
    bool I2C_DIS : 1;
    bool IF_ADD_INC : 1;
    bool STOP_ON_FTH : 1;
    bool FIFO_EN : 1;
    bool BOOT : 1;
  };
} LPS_CTRL_REG2_t;


#define LPS_CTRL_REG3_ADDR (0x12)

typedef union
{
  uint8_t reg;

  struct
  {
    bool INT_S1 : 1;
    bool INT_s2 : 1;
    bool DRDY : 1;
    bool F_OVR : 1;
    bool F_FTH : 1;
    bool F_FSS5 : 1;
    bool PP_OD : 1;
    bool INT_HL : 1;
  };
} LPS_CTRL_REG3_t;


#define LPS_FIFO_CTRL_ADDR (0x14)

typedef union
{
  uint8_t reg;

  struct
  {
    uint8_t WTM : 5;
    uint8_t F_MODE : 3;
  };
} LPS_FIFO_CTRL_t;


#define LPS_REF_P_XL_ADDR (0x15)
#define LPS_REF_P_L_ADDR (0x16)
#define LPS_REF_P_H_ADDR (0x17)

typedef union
{
  uint8_t bytes[4];
  uint32_t value;

  struct
  {
    uint8_t byte_XL;
    uint8_t byte_L;
    uint8_t byte_H;
  };
} LPS_REF_P_t;


#define LPS_RPDS_L_ADDR (0x18)
#define LPS_RPDS_H_ADDR (0x19)

typedef union
{
  uint8_t bytes[2];
  uint16_t value;

  struct
  {
    uint8_t byte_low;
    uint8_t byte_high;
  };
} LPS_RPDS_t;


#define LPS_RES_CONF_ADDR (0x1A)

typedef union
{
  uint8_t reg;

  struct
  {
    bool LC_EN : 1;
    bool reserved : 1;
    uint8_t zero : 6;
  };
} LPS_RES_CONF_t;


#define LPS_INT_SOURCE_ADDR (0x25)

typedef union
{
  uint8_t reg;

  struct
  {
    bool PH : 1;
    bool PL : 1;
    bool IA : 1;
    uint8_t zero : 4;
    bool BOOT_STATUS : 1;
  };
} LPS_INT_SOURCE_t;


#define LPS_FIFO_STATUS_ADDR (0x26)

typedef union
{
  uint8_t reg;

  struct
  {
    uint8_t FSS : 6;
    bool OVR : 1;
    bool FTH_FIFO : 1;
  };
} LPS_FIFO_STATUS_t;


#define LPS_STATUS_ADDR (0x27)

typedef union
{
  uint8_t reg;

  struct
  {
    bool P_DA : 1;
    bool T_DA : 1;
    uint8_t DNC1 : 2;
    bool P_OR : 1;
    bool T_OR : 1;
    uint8_t DNC2 : 2;
  };
} LPS_STATUS_t;


#define LPS_PRESS_OUT_XL_ADDR (0x28)
#define LPS_PRESS_OUT_L_ADDR (0x29)
#define LPS_PRESS_OUT_H_ADDR (0x2A)

typedef union
{
  uint8_t bytes[4];
  uint32_t value;

  struct
  {
    uint8_t byte_XL;
    uint8_t byte_L;
    uint8_t byte_H;
  };
} LPS_PRESS_OUT_t;


#define LPS_TEMP_OUT_L_ADDR (0x2B)
#define LPS_TEMP_OUT_H_ADDR (0x2C)

typedef union
{
  uint8_t bytes[2];
  uint16_t value;

  struct
  {
    uint8_t byte_L;
    uint8_t byte_H;
  };
} LPS_TEMP_OUT_t;


#define LPS_LPFP_REF_ADDR (0x33)

typedef struct
{
  uint8_t reg;
} LPS_LPFP_REF_t;


LPS_CTRL_REG1_t LPS_ctrl_reg1 = {0};
bool LPS_configured = false;

void LPS_Setup()
{
  // Enable LPS22HB
  if (!LPS_configured)
  {
    LPS_ctrl_reg1.ODR = 0b001;
    LPS_ctrl_reg1.EN_LPFP = 1;
    LPS_ctrl_reg1.BDU = 1;  // must read PRESS_OUT_H register last when BDU is enabled
    I2C_WriteByte(LPS_ADDR, LPS_CTRL_REG1_ADDR, LPS_ctrl_reg1.reg);
    delay(10);

    LPS_configured = true;
  }
}

// Barometric pressure sensor data
LPS_PRESS_OUT_t LPS_press = {0};
LPS_TEMP_OUT_t LPS_temp = {0};

float LPS_Press_mbar = 0;
float LPS_GetPressure_mbar()
{
  return LPS_Press_mbar;
}

float LPS_Temp_C = 0;
float LPS_GetTemperature_C()
{
  return LPS_Temp_C;
}

float LPS_Temp_F = 0;
float LPS_GetTemperature_F()
{
  return LPS_Temp_F;
}

float LPS_PressureOffset_mbar = 0;
float LPS_TemperatureOffset_C = -5.2;

void LPS_MeasurePressure()
{
  // Read pressure data from pressure sensor
  I2C_ReadByte(LPS_ADDR, LPS_PRESS_OUT_XL_ADDR, LPS_press.bytes, 3);
  LPS_Press_mbar = (float) LPS_press.value / 4096 + LPS_PressureOffset_mbar;

  delay(1);

  // Read temperature data from pressure sensor
  I2C_ReadByte(LPS_ADDR, LPS_TEMP_OUT_L_ADDR, LPS_temp.bytes, 2);
  LPS_Temp_C = (float) LPS_temp.value / 100 + LPS_TemperatureOffset_C;
  LPS_Temp_F = LPS_Temp_C * (9.0 / 5.0) + 32;
}

