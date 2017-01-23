#include <Arduino.h>
#include <Wire.h>

#include "creds.h"
#include "LTR-329.h"

#define USE_SERIAL Serial

#define MOTION_PIN (D0)

void writeByte(uint8_t slaveAddr, uint8_t regAddr, uint8_t cmd)
{
  Wire.beginTransmission(slaveAddr);
  Wire.write((regAddr));
  Wire.write((cmd));
  Wire.endTransmission();
  
  delay(80);
}

void readByte(uint8_t slaveAddr, uint8_t regAddr, uint8_t * data, uint8_t byteCount)
{
  Wire.beginTransmission(slaveAddr);
  Wire.write((regAddr));
  Wire.endTransmission();

  uint8_t bytes_rx = Wire.requestFrom(slaveAddr, byteCount);
  
  if (bytes_rx >= byteCount || Wire.available() >= byteCount) 
  { 
    for (int i = 0; i < byteCount; i++)
    {
      data[i] = Wire.read();
    }
  }

  delay(80);
}

void setup()
{
  pinMode(MOTION_PIN, INPUT);

  Wire.begin(SDA, SCL);
  
  USE_SERIAL.begin(115200);
  USE_SERIAL.setDebugOutput(true);

  delay(1000);
}


bool motion_detected = false;
bool als_configured = false;

ALS_CONTR_t als_control;
ALS_DATA_t als_ch0;
ALS_DATA_t als_ch1;

void loop()
{
  // Get PIR sensor output state
  motion_detected = digitalRead(MOTION_PIN);
  USE_SERIAL.printf("motion: %d \n", motion_detected);
  USE_SERIAL.flush();


  // Enable ALS
  if (!als_configured)
  {
    als_control.ALS_Gain = ALS_GAIN_48X;
    als_control.ALS_Mode = ALS_MODE_ACTIVE;
    writeByte(ALS_ADDR, ALS_CONTR_ADDR, als_control.reg);
    delay(100);
    als_configured = true;
  }

  // Read sensor data from ALS
  readByte(ALS_ADDR, ALS_DATA_CH1_LOW_ADDR, &als_ch1.byte_low, 1);
  readByte(ALS_ADDR, ALS_DATA_CH1_HIGH_ADDR, &als_ch1.byte_high, 1);
  readByte(ALS_ADDR, ALS_DATA_CH0_LOW_ADDR, &als_ch0.byte_low, 1);
  readByte(ALS_ADDR, ALS_DATA_CH0_HIGH_ADDR, &als_ch0.byte_high, 1);

  USE_SERIAL.printf("als ch0: %d, als ch1: %d \n", als_ch0.value, als_ch1.value);
  USE_SERIAL.flush();


  delay(1000);
}



