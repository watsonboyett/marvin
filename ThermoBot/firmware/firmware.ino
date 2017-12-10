
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

#include "creds.h"

#define USE_SERIAL Serial

#define RELAY1_PIN (D1)
#define RELAY2_PIN (D2)
#define RELAY3_PIN (D3)
#define RELAY4_PIN (D0)

#define FAN_CTRL (RELAY1_PIN)
#define HEATING_CTRL (RELAY2_PIN)
#define COOLING_CTRL (RELAY3_PIN)
#define SYSTEM_CTRL (RELAY4_PIN)

ESP8266WiFiMulti WiFiMulti;

void setup() {

  USE_SERIAL.begin(115200);
  // USE_SERIAL.setDebugOutput(true);

  USE_SERIAL.println();
  USE_SERIAL.println();
  USE_SERIAL.println();

  pinMode(RELAY1_PIN, OUTPUT);
  digitalWrite(RELAY1_PIN, LOW);
  pinMode(RELAY2_PIN, OUTPUT);
  digitalWrite(RELAY2_PIN, LOW);
  pinMode(RELAY3_PIN, OUTPUT);
  digitalWrite(RELAY3_PIN, LOW);
  pinMode(RELAY4_PIN, OUTPUT);
  digitalWrite(RELAY4_PIN, LOW);

  for (uint8_t t = 4; t > 0; t--) {
    USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
    USE_SERIAL.flush();
    delay(1000);
  }

  WiFiMulti.addAP(WIFI_SSID, WIFI_PASS);
}

static int loop_counter = 0;

const int NUM_RELAYS = 4;
static int relay_pins[] = {SYSTEM_CTRL, FAN_CTRL, HEATING_CTRL, COOLING_CTRL};
static bool relay_state[NUM_RELAYS] = {0};

void loop() {
  loop_counter++;

  bool use_wifi = true;
  bool use_relays = false;

  if (use_relays)
  {
    for (int i = 0; i < NUM_RELAYS; i++)
    {
      digitalWrite(relay_pins[i], LOW);
      relay_state[i] = 0;
    }

    int relay_cur = (loop_counter - 1) % NUM_RELAYS;
    digitalWrite(relay_pins[relay_cur], HIGH);
    relay_state[relay_cur] = 1;
  }
  
  // wait for WiFi connection
  if (use_wifi && (WiFiMulti.run() == WL_CONNECTED))
  {

    String system_str = "off";
    String post_payload = String("thermostat system=") + String(relay_state[0]) +
                          String(",fan=") + String(relay_state[1]) +
                          String(",heating=") + String(relay_state[2]) +
                          String(",cooling=") + String(relay_state[3]);
    USE_SERIAL.print(post_payload);

    HTTPClient http;
    USE_SERIAL.print("[HTTP] begin...\n");
    http.begin("http://192.168.0.10:8086/write?db=telegraf");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    http.POST(post_payload);
    http.writeToStream(&Serial);

    http.end();
  }

  digitalWrite(BUILTIN_LED, LOW);
  delay(5000);
}

