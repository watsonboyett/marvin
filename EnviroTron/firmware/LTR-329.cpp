#pragma once

#include "I2C_Utils.h"
#include <Arduino.h>

#define ALS_ADDR (0x29)

#define ALS_GAIN_1X (0)
#define ALS_GAIN_2X (1)
#define ALS_GAIN_4X (2)
#define ALS_GAIN_8X (3)
#define ALS_GAIN_48X (6)
#define ALS_GAIN_96X (7)

#define ALS_MODE_STANDBY (0)
#define ALS_MODE_ACTIVE (1)

#define ALS_CONTR_ADDR (0x80)

typedef union 
{
  uint8_t reg;
  struct
  {
    bool ALS_Mode : 1;
    bool SW_Reset : 1;
    uint8_t ALS_Gain : 3;
    uint8_t reserved : 3;
  };
} ALS_CONTR_t;

#define ALS_INT_TIME_100ms (0)
#define ALS_INT_TIME_50ms (1)
#define ALS_INT_TIME_200ms (2)
#define ALS_INT_TIME_400ms (3)
#define ALS_INT_TIME_150ms (4)
#define ALS_INT_TIME_250ms (5)
#define ALS_INT_TIME_300ms (6)
#define ALS_INT_TIME_350ms (7)

#define ALS_MEAS_RATE_50ms (0)
#define ALS_MEAS_RATE_100ms (1)
#define ALS_MEAS_RATE_200ms (2)
#define ALS_MEAS_RATE_500ms (3)
#define ALS_MEAS_RATE_1000ms (4)
#define ALS_MEAS_RATE_2000ms (5)

#define ALS_MEAS_RATE_ADDR (0x85)

typedef union 
{
  uint8_t reg;
  struct
  {
    uint8_t ALS_Meas_Rate : 3;
    uint8_t ALS_Int_Time : 3;
    uint8_t reserved : 2;
  };
} ALS_MEAS_RATE_t;


#define ALS_DATA_CH1_LOW_ADDR (0x88)
#define ALS_DATA_CH1_HIGH_ADDR (0x89)
#define ALS_DATA_CH0_LOW_ADDR (0x8A)
#define ALS_DATA_CH0_HIGH_ADDR (0x8B)

typedef union
{
  uint16_t value;
  struct
  {
    uint8_t byte_low;
    uint8_t byte_high;
  };
} ALS_DATA_t;

#define ALS_PS_STATUS_ADDR (0x8C)

typedef union
{
  uint8_t reg;
  struct
  {
    uint8_t reserved : 2;
    bool ALS_Data_Status : 1;
    bool ALS_Int_status : 1;
    uint8_t ALS_Gain : 3;
    bool ALS_Data_Valid : 1;
  };
} ALS_PS_STATUS_t;



ALS_CONTR_t ALS_control = {0};
float ALS_GainFactor = 1;

float CalcGainFactor()
{
  switch(ALS_control.ALS_Gain)
  {
  case (ALS_GAIN_1X):
    ALS_GainFactor = 1 / 1.0;
    break;
  case (ALS_GAIN_2X):
    ALS_GainFactor = 1 / 2.0;
    break;
  case (ALS_GAIN_4X):
    ALS_GainFactor = 1 / 4.0;
    break;
  case (ALS_GAIN_8X):
    ALS_GainFactor = 1 / 8.0;
    break;
  case (ALS_GAIN_48X):
    ALS_GainFactor = 1 / 48.0;
    break;
  case (ALS_GAIN_96X):
    ALS_GainFactor = 1 / 96.0;
    break;
  }
}

bool ALS_configured = false;
ALS_MEAS_RATE_t ALS_meas = {0};

void ALS_Setup()
{
  if (!ALS_configured)
  {
    delay(1200);
    
    ALS_control.ALS_Gain = ALS_GAIN_8X;
    ALS_control.ALS_Mode = ALS_MODE_ACTIVE;
    I2C_WriteByte(ALS_ADDR, ALS_CONTR_ADDR, ALS_control.reg);
    
    delay(20);

    ALS_meas.ALS_Meas_Rate = ALS_MEAS_RATE_100ms;
    ALS_meas.ALS_Int_Time = ALS_INT_TIME_50ms;
    I2C_WriteByte(ALS_ADDR, ALS_MEAS_RATE_ADDR, ALS_meas.reg);
    
    ALS_configured = true;
  }

  CalcGainFactor();
}


ALS_DATA_t ALS_vis = {0};
ALS_DATA_t ALS_ir = {0};

const float ALS_VIS_CORRECTION_FACTOR = 18.0;

float ALS_GetAmbientLightLevel()
{
  float level = (float) ALS_vis.value * ALS_GainFactor * ALS_VIS_CORRECTION_FACTOR;
  return level;
}

const float ALS_IR_CORRECTION_FACTOR = 4.5;

float ALS_GetIrLightLevel()
{
  float level = (float) ALS_ir.value * ALS_GainFactor * ALS_IR_CORRECTION_FACTOR;
  return level;
}

ALS_PS_STATUS_t ALS_status;

void ALS_MeasureLight()
{
  // Read data from light sensor
  I2C_ReadByte(ALS_ADDR, ALS_PS_STATUS_ADDR, &ALS_status.reg, 1);
  
  if (ALS_status.ALS_Data_Status)
  {
    I2C_ReadByte(ALS_ADDR, ALS_DATA_CH1_LOW_ADDR, &ALS_ir.byte_low, 1);
    I2C_ReadByte(ALS_ADDR, ALS_DATA_CH1_HIGH_ADDR, &ALS_ir.byte_high, 1);
    I2C_ReadByte(ALS_ADDR, ALS_DATA_CH0_LOW_ADDR, &ALS_vis.byte_low, 1);
    I2C_ReadByte(ALS_ADDR, ALS_DATA_CH0_HIGH_ADDR, &ALS_vis.byte_high, 1);
  }
}
