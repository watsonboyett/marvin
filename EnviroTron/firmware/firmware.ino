
#include "creds.h"
#include "PIR.h"
#include "MicAdc.h"
#include "LTR-329.h"
#include "SHT21.h"
#include "LPS22HB.h"
#include "I2C_Utils.h"

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define USE_SERIAL Serial
#define USE_WIFI (false)

void connect_wifi()
{
  if (!USE_WIFI)
  {
    WiFi.mode(WIFI_OFF);
  }
  else
  {
    USE_SERIAL.println("Initializing Wifi...");
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    
    unsigned long now = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - now < 10000) 
    {
      USE_SERIAL.print(".");
      delay(500);
    }
  
    USE_SERIAL.print("WiFi connected. IP address: ");  
    USE_SERIAL.println(WiFi.localIP());
  }
}

void setup()
{
  USE_SERIAL.begin(115200);
  USE_SERIAL.setDebugOutput(true);
  
  connect_wifi();
  
  I2C_Setup();
  PIR_Setup();
  MIC_Setup();
  ALS_Setup();
  SHT_Setup();
  LPS_Setup();

  delay(1000);
}


int cycles_since_last_write = 0;

bool motion_inst = false;
bool motion_avg = false;

const float avg_a = 0.1;
const float avg_a1 = (1 - avg_a);

float als_visible_inst = 0;
float als_visible_avg = 0;
float als_infrared_inst = 0;

float sht_temp_inst = 0;
float sht_temp_avg = 0;
float sht_rh_inst = 0;
float sht_rh_avg = 0;

float lps_pres_inst = 0;
float lps_pres_avg = 0;
float lps_temp_inst = 0;
float lps_temp_avg = 0;

float mic_level_inst = 0;
float mic_level_avg = 0;
float mic_level_max = 0;

uint32_t sample_count = 0;
void loop()
{
  sample_count++;
  
  // Get the latest sensor values
  motion_inst = PIR_AnyMovementOccurred();
  motion_avg = motion_avg || motion_inst;
  
  ALS_MeasureLight();
  als_infrared_inst = ALS_GetIrLightLevel();
  als_visible_inst = ALS_GetAmbientLightLevel();
  
  SHT_MeasureTemperature();
  sht_temp_inst = SHT_GetTemperature_F();
  sht_rh_inst = SHT_GetRelHumidity_pct();
  
  LPS_MeasurePressure();
  lps_pres_inst = LPS_GetPressure_mbar();
  lps_temp_inst = LPS_GetTemperature_F();

  mic_level_inst = MIC_GetAvgLevel_SPL();
  mic_level_max = MIC_GetMaxLevel_SPL();
  MIC_ResetMinMaxLevels();

  if (sample_count > 1)
  {
    als_visible_avg = (als_visible_inst * avg_a) + (als_visible_avg * avg_a1);
    sht_temp_avg = (sht_temp_inst * avg_a) + (sht_temp_avg * avg_a1);
    sht_rh_avg = (sht_rh_inst * avg_a) + (sht_rh_avg * avg_a1);
    lps_pres_avg = (lps_pres_inst * avg_a) + (lps_pres_avg * avg_a1);
    lps_temp_avg = (lps_temp_inst * avg_a) + (lps_temp_avg * avg_a1);
    mic_level_avg = (mic_level_inst * avg_a) + (mic_level_avg * avg_a1);
  }
  else
  {
    // initialize average values
    als_visible_avg = als_visible_inst;
    sht_temp_avg = sht_temp_inst;
    sht_rh_avg = sht_rh_inst;
    lps_pres_avg = lps_pres_inst;
    lps_temp_avg = lps_temp_inst;
    mic_level_avg = mic_level_inst;
  }

  
  // Print results to console
  char sample_count_str[9];
  sprintf(sample_count_str, "%u", sample_count);
  USE_SERIAL.printf("Sample: %s, ", sample_count_str);

  char sht_temp_str[6];
  dtostrf(sht_temp_inst, 0, 2, sht_temp_str);
  char sht_rh_str[6];
  dtostrf(sht_rh_inst, 0, 2, sht_rh_str);
  USE_SERIAL.printf("SHT T: %s, SHT RH: %s, ", sht_temp_str, sht_rh_str);

  char lps_press_str[6];
  dtostrf(lps_pres_inst, 0, 2, lps_press_str);
  char lps_temp_str[6];
  dtostrf(lps_temp_inst, 0, 2, lps_temp_str);
  USE_SERIAL.printf("LPS P: %s, LPS T: %s, ", lps_press_str, lps_temp_str);

  char als_vis_str[9];
  dtostrf(als_visible_inst, 0, 2, als_vis_str);
  char als_ir_str[9];
  dtostrf(als_infrared_inst, 0, 2, als_ir_str);
  USE_SERIAL.printf("ALS Vis: %s, ALS IR: %s, ", als_vis_str, als_ir_str);
  
  USE_SERIAL.printf("PIR M: %d, ", motion_inst);

  char mic_inst_str[6];
  dtostrf(mic_level_inst, 0, 2, mic_inst_str);
  USE_SERIAL.printf("MIC Va: %s ", mic_inst_str);
  char mic_max_str[6];
  dtostrf(mic_level_max, 0, 2, mic_max_str);
  USE_SERIAL.printf("MIC Vh: %s ", mic_max_str);
  
  USE_SERIAL.printf("\n");
  USE_SERIAL.flush();


  // write sensor data to database (ensure WiFi is connected)
  int db_write_rate = 30;
  if (USE_WIFI && cycles_since_last_write >= db_write_rate && (WiFi.status() == WL_CONNECTED))
  {
    String post_payload =
      String("sensor,node=0 ") +
      String("sample=") + String(sample_count_str) +
      String(",temp=") + String(sht_temp_avg) + 
      String(",humidity=") + String(sht_rh_avg) +
      String(",pressure=") + String(lps_pres_avg) +
      String(",temp_alt=") + String(lps_temp_avg) +
      String(",light=") + String(als_visible_avg) +
      String(",motion=") + String(motion_avg) +
      String(",sound=") + String(mic_level_avg);
    USE_SERIAL.println(post_payload);

    bool isHttpGet = false;
    String write_url = String(INFLUXDB_URL) + "/write?db=" + INFLUXDB_DBNAME;

    HTTPClient http;
    USE_SERIAL.print("[HTTP] begin... ");
    http.begin(write_url);
    int httpCode = -1;
    if (isHttpGet)
    {
      httpCode = http.GET();
    }
    else
    {
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      httpCode = http.POST(post_payload);
      http.writeToStream(&Serial);
    }
    http.end();
    USE_SERIAL.printf("[HTTP] end. HttpCode: %d \n", httpCode);

    if(httpCode < 1)
    {
      USE_SERIAL.printf("[HTTP] failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    else
    {
      if(isHttpGet && httpCode == HTTP_CODE_OK) 
      {
          String payload = http.getString();
          USE_SERIAL.println(payload);
      }  
    }
    
    motion_avg = false;
    cycles_since_last_write = 0;
  }

  cycles_since_last_write++;
  delay(1000);
}



