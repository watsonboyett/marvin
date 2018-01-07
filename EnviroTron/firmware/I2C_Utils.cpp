#pragma once

#include "I2C_Utils.h"

#include <Arduino.h>
#include <Wire.h>

void I2C_Setup()
{
  Wire.begin(SDA, SCL);
}

// return True if ACK is retruned from slave, false otherwise
uint8_t I2C_WriteAddr(uint8_t slaveAddr)
{
  Wire.beginTransmission(slaveAddr);
  uint8_t result = Wire.endTransmission();
  return result;
}

uint8_t I2C_WriteCmd(uint8_t slaveAddr, uint8_t cmd)
{
  Wire.beginTransmission(slaveAddr);
  Wire.write(cmd);
  uint8_t result = Wire.endTransmission();
  return result;
}

uint8_t I2C_WriteByte(uint8_t slaveAddr, uint8_t regAddr, uint8_t cmd)
{
  Wire.beginTransmission(slaveAddr);
  Wire.write(regAddr);
  Wire.write(cmd);
  uint8_t result = Wire.endTransmission();
  return result;
}

// return True if ACK is retruned from slave, false otherwise
bool I2C_ReadAddr(uint8_t slaveAddr, uint8_t * data, uint8_t byteCount)
{
  uint8_t bytes_rx = Wire.requestFrom(slaveAddr, byteCount);
  //Serial.printf("received %d bytes\n", bytes_rx);

  if (bytes_rx < byteCount && Wire.available() < byteCount)
  {
    return false;
  }
  else
  {
    for (int i = 0; i < byteCount; i++)
    {
      data[i] = Wire.read();
    }
    return true;
  }
}

bool I2C_ReadByte(uint8_t slaveAddr, uint8_t regAddr, uint8_t * data, uint8_t byteCount)
{
  I2C_WriteCmd(slaveAddr, regAddr);
  delay(1);
  return I2C_ReadAddr(slaveAddr, data, byteCount);
}

