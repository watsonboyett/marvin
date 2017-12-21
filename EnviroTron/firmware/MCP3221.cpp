
#include "MCP3221.h"
#include "common.h"
#include "I2C_Utils.h"

#define MCP3221_ADDR (0b01001101)

uint16_t MCP3221_value_raw = 0;
float MCP3221_value_v = 0;

void MCP3221_MeasureLevel()
{
  //int sensorValue = analogRead(MIC_ADC_PIN);
  uint8_t rd_bytes[2] = {0};
  bool success = I2C_ReadAddr(MCP3221_ADDR, rd_bytes, 2);
  MCP3221_value_raw = ((int)rd_bytes[0] << 8 | rd_bytes[1]);

  MCP3221_value_v = MCP3221_value_raw * (MCP3221_VOLTAGE_MAX / MCP3221_RAW_MAX);
}

uint16_t MCP3221_GetLevel_Raw()
{
  return MCP3221_value_raw;
}

float MCP3221_GetLevel_V()
{
  return MCP3221_value_v;
}

