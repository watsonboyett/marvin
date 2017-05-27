#pragma once

#include <Arduino.h>

#define byteswap(x) (((x & 0x00FF) << 8) | ((x & 0xFF00) >> 8))

void I2C_Setup();

uint8_t I2C_WriteAddr(uint8_t slaveAddr);

uint8_t I2C_WriteCmd(uint8_t slaveAddr, uint8_t cmd);

uint8_t I2C_WriteByte(uint8_t slaveAddr, uint8_t regAddr, uint8_t cmd);

bool I2C_ReadAddr(uint8_t slaveAddr, uint8_t * data, uint8_t byteCount);

bool I2C_ReadByte(uint8_t slaveAddr, uint8_t regAddr, uint8_t * data, uint8_t byteCount);

