

#include <Arduino.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

#include "creds.h"

#define USE_SERIAL Serial

#define DHTPIN D4     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);

ESP8266WiFiMulti WiFiMulti;


void setup() {

  USE_SERIAL.begin(115200);
  // USE_SERIAL.setDebugOutput(true);

  USE_SERIAL.println();
  USE_SERIAL.println();
  USE_SERIAL.println();

  for (uint8_t t = 4; t > 0; t--) {
    USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
    USE_SERIAL.flush();
    delay(1000);
  }

  WiFiMulti.addAP(WIFI_SSID, WIFI_PASS);

}

float temp_f = 0;
float humidity_f = 0;
void loop() {

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float humidity_f = dht.readHumidity();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float temp_f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity_f) || isnan(temp_f)) {
    USE_SERIAL.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(temp_f, humidity_f);

  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    char temp_str[7];
    char humidity_str[7];
    dtostrf(temp_f, 5, 2, temp_str);
    dtostrf(humidity_f, 5, 2, humidity_str);
    String post_payload = String("sensor,node=8 humidity=") + String(humidity_str)
                          + String(",temp=") + String(temp_str);
    USE_SERIAL.print(post_payload);

    HTTPClient http;
    USE_SERIAL.print("[HTTP] begin...\n");
    http.begin("http://192.168.0.10:8086/write?db=telegraf");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    http.POST(post_payload);
    http.writeToStream(&Serial);

    http.end();
  }

  delay(30000);
}
