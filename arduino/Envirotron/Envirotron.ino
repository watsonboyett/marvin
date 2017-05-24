
#include "creds.h"
#include "LTR-329.h"
#include "SHT21.h"
#include "MS5607.h"

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
ALS_CONTR_t als_control = {0};
ALS_MEAS_RATE_t als_meas_t = {0};
ALS_DATA_t als_ch0 = {0};
ALS_DATA_t als_ch1 = {0};

SHT21_ADC_CONV_t sht_temp = {0};
SHT21_ADC_CONV_t sht_rh = {0};

bool ms_configured = false;
MS_PROM_t ms_prom = {0};
MS_ADC_READ_t ms_press = {0};
MS_ADC_READ_t ms_temp = {0};

void loop()
{

  // Enable ALS
  if (!als_configured)
  {
    als_control.ALS_Gain = ALS_GAIN_96X;
    als_control.ALS_Mode = ALS_MODE_ACTIVE;
    writeByte(ALS_ADDR, ALS_CONTR_ADDR, als_control.reg);
    delay(100);

    als_meas_t.ALS_Meas_Rate = ALS_MEAS_RATE_500ms;
    als_meas_t.ALS_Int_Time = ALS_INT_TIME_200ms;
    writeByte(ALS_ADDR, ALS_MEAS_RATE_ADDR, als_meas_t.reg);
    
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


  // Enable MS5607
  if (!ms_configured)
  {
    // 1. send reset sequence (to make sure that calibration PROM gets loaded into internal registers)
    writeByte(MS_ALT_ADDR, MS_RESET_ADDR, 0);
    delay(10);
    
    // 2. read PROM data (to get calibration coefficients)
    readByte(MS_ALT_ADDR, MS_PROM_FACTORY_ADDR, &ms_prom.FACTORY_h, 2);
    readByte(MS_ALT_ADDR, MS_PROM_SENS_T1_ADDR, &ms_prom.SENS_T1_h, 2);
    readByte(MS_ALT_ADDR, MS_PROM_OFF_T1_ADDR, &ms_prom.OFF_T1_h, 2);
    readByte(MS_ALT_ADDR, MS_PROM_TCS_ADDR, &ms_prom.TCS_h, 2);
    readByte(MS_ALT_ADDR, MS_PROM_TCO_ADDR, &ms_prom.TCO_h, 2);
    readByte(MS_ALT_ADDR, MS_PROM_T_REF_ADDR, &ms_prom.T_REF_h, 2);
    readByte(MS_ALT_ADDR, MS_PROM_TEMPSENS_ADDR, &ms_prom.TEMPSENS_h, 2);

    delay(100);
    ms_configured = true;
  }

  // Read sensor data from PS
  writeByte(MS_ALT_ADDR, MS_CONV_D1_OSR_2048, 0);
  delay(12);
  readByte(MS_ALT_ADDR, MS_ADC_READ_ADDR, &ms_press.bytes[3], 3);
  uint32_t D1_press = ms_press.data & 0x00ffffff; // pressure
  USE_SERIAL.printf("MS P raw: %x \n", ms_press.data);

  writeByte(MS_ALT_ADDR, MS_CONV_D2_OSR_2048, 0);
  delay(12);
  readByte(MS_ALT_ADDR, MS_ADC_READ_ADDR, &ms_temp.bytes[3], 3);
  uint32_t D2_temp = ms_temp.data & 0x00ffffff; // temperature
  USE_SERIAL.printf("MS T raw: %x \n", ms_temp.data);

  int32_t dT = D2_temp - (ms_prom.T_REF << 8);
  int32_t TEMP = 2000 + dT * (ms_prom.TEMPSENS >> 23);
  int64_t OFF = (ms_prom.OFF_T1 << 17) + ((ms_prom.TCO * dT) >> 6);
  int64_t SENS = (ms_prom.SENS_T1 << 16) + ((ms_prom.TCS * dT) >> 7);
  int64_t PRESS = (D1_press * SENS >> 21 - OFF) >> 15;
  float ms_pres_f = PRESS;
  float ms_temp_f = TEMP;

  // Print results to console
  USE_SERIAL.printf("motion: %d, ", anyMovementOccurred());
  USE_SERIAL.printf("ALS ch0: %d, ALS ch1: %d, ", als_ch0.value, als_ch1.value);
  char temp_str[6];
  dtostrf(temp_F, 0, 2, temp_str);
  char rh_str[6];
  dtostrf(rh_pct, 0, 2, rh_str);
  USE_SERIAL.printf("SHT T: %s, SHT RH: %s, ", temp_str, rh_str);
  char press_str[6];
  dtostrf(ms_pres_f, 0, 2, press_str);
  char ms_temp_str[6];
  dtostrf(ms_temp_f, 0, 2, ms_temp_str);
  USE_SERIAL.printf("MS P: %s, MS T: %s, ", press_str, ms_temp_str);
  USE_SERIAL.printf("\n");
  USE_SERIAL.flush();

  delay(3000);
}



