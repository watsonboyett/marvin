
#include "creds.h"
#include "LTR-329.h"
#include "SHT21.h"
#include "LPS22HB.h"


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


// Light sensor data
bool ALS_configured = false;
ALS_CONTR_t ALS_control = {0};
ALS_MEAS_RATE_t ALS_meas = {0};
ALS_DATA_t ALS_vis = {0};
ALS_DATA_t ALS_ir = {0};

void measure_ambient_light()
{
  // Enable light sensor
  if (!ALS_configured)
  {
    ALS_control.ALS_Gain = ALS_GAIN_96X;
    ALS_control.ALS_Mode = ALS_MODE_ACTIVE;
    writeByte(ALS_ADDR, ALS_CONTR_ADDR, ALS_control.reg);
    delay(100);

    ALS_meas.ALS_Meas_Rate = ALS_MEAS_RATE_500ms;
    ALS_meas.ALS_Int_Time = ALS_INT_TIME_200ms;
    writeByte(ALS_ADDR, ALS_MEAS_RATE_ADDR, ALS_meas.reg);
    
    ALS_configured = true;
  }

  // Read data from light sensor
  readByte(ALS_ADDR, ALS_DATA_CH1_LOW_ADDR, &ALS_ir.byte_low, 1);
  readByte(ALS_ADDR, ALS_DATA_CH1_HIGH_ADDR, &ALS_ir.byte_high, 1);
  readByte(ALS_ADDR, ALS_DATA_CH0_LOW_ADDR, &ALS_vis.byte_low, 1);
  readByte(ALS_ADDR, ALS_DATA_CH0_HIGH_ADDR, &ALS_vis.byte_high, 1);
}


// Temperature/humidity sensor data
SHT21_ADC_CONV_t SHT_temp = {0};
SHT21_ADC_CONV_t SHT_rh = {0};
float SHT_Temp_C = 0;
float SHT_Temp_F = 0;
float SHT_RH_pct = 0;

void measure_temperature()
{
  // Read data from temperature sensor
  readByte(SHT21_ADDR, SHT21_MEAS_T_NOHOLD_ADDR, SHT_temp.bytes, 3);
  float temp_sens = (byteswap(SHT_temp.data) & 0xFFFC);
  SHT_Temp_C = -46.85 + 175.72 * (temp_sens / 65536);
  SHT_Temp_F = SHT_Temp_C * (9.0 / 5.0) + 32;

  readByte(SHT21_ADDR, SHT21_MEAS_RH_NOHOLD_ADDR, SHT_rh.bytes, 3);
  float rh_sens = (byteswap(SHT_rh.data) & 0xFFFC);
  SHT_RH_pct = -6 + 125 * (rh_sens / 65536);
}


// Barometric pressure sensor data
bool LPS_configured = false;
LPS_CTRL_REG1_t LPS_ctrl_reg1 = {0};
LPS_PRESS_OUT_t LPS_press = {0};
LPS_TEMP_OUT_t LPS_temp = {0};
float LPS_Press_mbar = 0;
float LPS_Temp_C = 0;
float LPS_Temp_F = 0;

void measure_pressure()
{
  // Enable LPS22HB
  if (!LPS_configured)
  {
    LPS_ctrl_reg1.ODR = 0b001;
    LPS_ctrl_reg1.EN_LPFP = 1;
    LPS_ctrl_reg1.BDU = 1;  // must read PRESS_OUT_H register last when BDU is enabled    
    writeByte(LPS_ADDR, LPS_CTRL_REG1_ADDR, LPS_ctrl_reg1.reg);
    delay(10);
    
    LPS_configured = true;
  }

  // Read pressure data from pressure sensor
  readByte(LPS_ADDR, LPS_PRESS_OUT_XL_ADDR, LPS_press.bytes, 3);
  LPS_Press_mbar = (float) LPS_press.value / 4096;
  //USE_SERIAL.printf("LPS P raw: %x \n", lps_press.value);

  // Read temperature data from pressure sensor
  delay(10);
  readByte(LPS_ADDR, LPS_TEMP_OUT_L_ADDR, LPS_temp.bytes, 2);
  LPS_Temp_C = (float) LPS_temp.value / 100;
  LPS_Temp_F = LPS_Temp_C * (9.0 / 5.0) + 32;
  //USE_SERIAL.printf("LPS T raw: %x \n", lps_temp.value);
}


void loop()
{
  measure_ambient_light();

  measure_temperature();

  measure_pressure();
  

  // Print results to console
  USE_SERIAL.printf("motion: %d, ", anyMovementOccurred());
  USE_SERIAL.printf("ALS vis: %d, ALS ir: %d, ", ALS_vis.value, ALS_ir.value);
  
  char temp_str[6];
  dtostrf(SHT_Temp_F, 0, 2, temp_str);
  char rh_str[6];
  dtostrf(SHT_RH_pct, 0, 2, rh_str);
  USE_SERIAL.printf("SHT T: %s, SHT RH: %s, ", temp_str, rh_str);

  char lps_press_str[6];
  dtostrf(LPS_Press_mbar, 0, 2, lps_press_str);
  char lps_temp_str[6];
  dtostrf(LPS_Temp_F, 0, 2, lps_temp_str);
  USE_SERIAL.printf("LPS P: %s, LPS T: %s, ", lps_press_str, lps_temp_str);
  
  USE_SERIAL.printf("\n");
  USE_SERIAL.flush();

  delay(3000);
}



