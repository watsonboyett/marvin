
#include "creds.h"
#include "LTR-329.h"
#include "SHT21.h"

#include <Arduino.h>
#include <Wire.h>

#define USE_SERIAL Serial

#define byteswap(x) (((x & 0x00FF) << 8) | ((x & 0xFF00) >> 8))

#define MOTION_PIN (D8)  // move to D8 pin

void writeByte(uint8_t slaveAddr, uint8_t regAddr, uint8_t cmd)
{
  Wire.beginTransmission(slaveAddr);
  Wire.write((regAddr));
  Wire.write((cmd));
  Wire.endTransmission();

  delay(10);
}

void readByte(uint8_t slaveAddr, uint8_t regAddr, uint8_t * data, uint8_t byteCount)
{
  Wire.beginTransmission(slaveAddr);
  Wire.write((regAddr));
  Wire.endTransmission();

  delay(100);

  uint8_t bytes_rx = Wire.requestFrom(slaveAddr, byteCount);
  //USE_SERIAL.printf("received %d bytes\n", bytes_rx);

  if (bytes_rx >= byteCount || Wire.available() >= byteCount)
  {
    for (int i = 0; i < byteCount; i++)
    {
      data[i] = Wire.read();
    }
  }

  delay(10);
}

volatile bool pir_active = false;
bool movement_detected = false;
bool movement_ack = true;

void pirStateChanged()
{
  // Get PIR sensor output state
  pir_active = digitalRead(MOTION_PIN);
  movement_detected = pir_active || !movement_ack;
  movement_ack = !movement_detected;
}

bool anyMovementOccurred()
{
  movement_ack = true;
  if (movement_detected)
  {
    movement_detected = false;
    return true;
  }
  return false;
}

void setup()
{
  pinMode(MOTION_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(MOTION_PIN), pirStateChanged, CHANGE);

  Wire.begin(SDA, SCL);

  USE_SERIAL.begin(115200);
  USE_SERIAL.setDebugOutput(true);

  delay(1000);
}



bool als_configured = false;

ALS_CONTR_t als_control;
ALS_DATA_t als_ch0;
ALS_DATA_t als_ch1;

SHT21_ADC_CONV_t sht_temp;
SHT21_ADC_CONV_t sht_rh;

void loop()
{

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

  // Read sensor data from SHT21
  readByte(SHT21_ADDR, SHT21_MEAS_T_NOHOLD_ADDR, sht_temp.bytes, 3);
  float temp_sens = (byteswap(sht_temp.data) & 0xFFFC);
  float temp_C = -46.85 + 175.72 * (temp_sens / 65536);
  float temp_F = temp_C * (9.0 / 5.0) + 32;

  readByte(SHT21_ADDR, SHT21_MEAS_RH_NOHOLD_ADDR, sht_rh.bytes, 3);
  float rh_sens = (byteswap(sht_rh.data) & 0xFFFC);
  float rh_pct = -6 + 125 * (rh_sens / 65536);

  // Read sensor data from PS



  // Print results to console
  USE_SERIAL.printf("motion: %d, ", anyMovementOccurred());
  USE_SERIAL.printf("ALS ch0: %d, ALS ch1: %d, ", als_ch0.value, als_ch1.value);
  char temp_str[6];
  dtostrf(temp_F, 0, 2, temp_str);
  char rh_str[6];
  dtostrf(rh_pct, 0, 2, rh_str);
  USE_SERIAL.printf("SHT T: %s, SHT RH: %s, ", temp_str, rh_str);
  USE_SERIAL.printf("\n");
  USE_SERIAL.flush();

  delay(3000);
}



