
#include "creds.h"
#include "PIR.h"
#include "MicAdc.h"
#include "LTR-329.h"
#include "SHT21.h"
#include "LPS22HB.h"
#include "I2C_Utils.h"

#include <Arduino.h>
#include <ESP8266WiFi.h>

#define USE_SERIAL Serial

void setup()
{
  WiFi.mode(WIFI_OFF);
  
  I2C_Setup();
  
  PIR_Setup();
  MIC_Setup();
  ALS_Setup();
  SHT_Setup();
  LPS_Setup();

  USE_SERIAL.begin(115200);
  USE_SERIAL.setDebugOutput(true);

  delay(1000);
}

void loop()
{
  ALS_MeasureLight();
  SHT_MeasureTemperature();
  LPS_MeasurePressure();

  
  // Print results to console
  USE_SERIAL.printf("PIR M: %d, ", PIR_AnyMovementOccurred());

  char als_vis_str[9];
  dtostrf(ALS_GetAmbientLightLevel(), 0, 2, als_vis_str);
  char als_ir_str[9];
  dtostrf(ALS_GetIrLightLevel(), 0, 2, als_ir_str);
  USE_SERIAL.printf("ALS Vis: %s, ALS IR: %s, ", als_vis_str, als_ir_str);
  
  char sht_temp_str[6];
  dtostrf(SHT_GetTemperature_F(), 0, 2, sht_temp_str);
  char sht_rh_str[6];
  dtostrf(SHT_GetRelHumidity_pct(), 0, 2, sht_rh_str);
  USE_SERIAL.printf("SHT T: %s, SHT RH: %s, ", sht_temp_str, sht_rh_str);

  char lps_press_str[6];
  dtostrf(LPS_GetPressure_mbar(), 0, 2, lps_press_str);
  char lps_temp_str[6];
  dtostrf(LPS_GetTemperature_F(), 0, 2, lps_temp_str);
  USE_SERIAL.printf("LPS P: %s, LPS T: %s, ", lps_press_str, lps_temp_str);

  char snd_value_str[6];
  dtostrf(MIC_GetMicAvgLevel_V(), 0, 2, snd_value_str);
  USE_SERIAL.printf("MIC V: %s ", snd_value_str);
  
  
  USE_SERIAL.printf("\n");
  USE_SERIAL.flush();

  delay(500);
}



