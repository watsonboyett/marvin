#include <Arduino.h>

#define ALS_ADDR (0x29)

#define ALS_GAIN_1X (0)
#define ALS_GAIN_2X (1)
#define ALS_GAIN_4X (2)
#define ALS_GAIN_8X (3)
#define ALS_GAIN_48X (4)
#define ALS_GAIN_96X (5)

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

