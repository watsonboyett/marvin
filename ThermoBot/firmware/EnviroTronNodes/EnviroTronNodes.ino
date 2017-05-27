#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

#include "creds.h"

#define USE_SERIAL Serial

ESP8266WiFiMulti WiFiMulti;

void setup()
{

  USE_SERIAL.begin(115200);
  USE_SERIAL.setDebugOutput(true);

  USE_SERIAL.println();
  USE_SERIAL.println();
  USE_SERIAL.println();

  for (uint8_t t = 3; t > 0; t--)
  {
    USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
    USE_SERIAL.flush();
    delay(1000);
  }

  WiFiMulti.addAP(WIFI_SSID, WIFI_PASS);

}

int cycles_since_last_write = 0;

void loop()
{


  // write sensor data to database (ensure WiFi is connected)
  if (cycles_since_last_write >= 30 && (WiFiMulti.run() == WL_CONNECTED))
  {
    USE_SERIAL.println();

    char temp_str[7];
    dtostrf(temp_f, 0, 2, temp_str);

    char humidity_str[7];
    dtostrf(humidity_f, 0, 2, humidity_str);

    char heatindex_str[7];
    dtostrf(heatindex_f, 0, 2, heatindex_str);
    

    String post_payload =
      String("sensor,node=0 ") +
      String("humidity=") + String(humidity_str) +
      String(",temp=") + String(temp_str);
    USE_SERIAL.print(post_payload);

    HTTPClient http;
    USE_SERIAL.print("[HTTP] begin...\n");

    String write_url = String(INFLUXDB_URL) + "/write?db=" + INFLUXDB_DBNAME;
    http.begin(write_url);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    http.POST(post_payload);
    http.writeToStream(&Serial);

    http.end();
    USE_SERIAL.println();
    
    cycles_since_last_write = 0;
  }

  cycles_since_last_write++;
  delay(1000);
}

