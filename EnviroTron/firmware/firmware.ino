
#include "creds.h"
#include "Sensors.h"
#include "common.h"

#include <Arduino.h>
#include <ESP8266WiFi.h>
//#include <ESP8266HTTPClient.h>
#include <PubSubClient.h>

extern "C" {
#include "user_interface.h"
}

#define USE_SERIAL Serial
#define USE_WIFI (true)

WiFiClient espClient;
PubSubClient client(espClient);
IPAddress broker(192, 168, 0, 186);

void callback(char* topic, byte* payload, unsigned int length)
{
  USE_SERIAL.print("Message arrived [");
  USE_SERIAL.print(topic);
  USE_SERIAL.print("] ");
  for (int i = 0; i < length; i++)
  {
    USE_SERIAL.print((char)payload[i]);
  }
  USE_SERIAL.println();
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    USE_SERIAL.print("Attempting MQTT connection...");
    if (client.connect("ESP8266 Client"))
    {
      USE_SERIAL.println("connected");
      // ... and subscribe to topic
      client.subscribe("ledStatus");
    }
    else
    {
      USE_SERIAL.print("failed, rc=");
      USE_SERIAL.print(client.state());
      USE_SERIAL.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

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

    client.setServer(broker, 1883);
    client.setCallback(callback);

    USE_SERIAL.print("WiFi connected. IP address: ");
    USE_SERIAL.println(WiFi.localIP());
  }
}

int chip_id = 0;
char chip_id_str[7];



void setup()
{
  USE_SERIAL.begin(115200);
  USE_SERIAL.setDebugOutput(true);
  USE_SERIAL.println();

  chip_id = ESP.getChipId();
  sprintf(chip_id_str, "%#06x", chip_id);
  USE_SERIAL.printf("Chip ID: %s \n", chip_id_str);
  String core_version = String("Core Version: ") + ESP.getCoreVersion();
  USE_SERIAL.println(core_version);
  String sdk_version = String("SDK Version: ") + ESP.getSdkVersion();
  USE_SERIAL.println(sdk_version);

  connect_wifi();
  // NOTE: prevent ESP from sleeping because it causes lots of ADC noise
  wifi_set_sleep_type(NONE_SLEEP_T);

  SENSORS_Init();
  delay(1000);
  SENSORS_EnableInterrupts();
}


bool motion_avg;
float light_avg;
float ir_avg;
float temp_avg = 0;
float rel_hum_avg = 0;
float pres_avg = 0;
float sound_avg = 0;
float sound_max = 0;

bool publish_sensor_data(SensorData_t sd)
{
  bool success = 1;
  for (int i = 0; i < 8; i++)
  {
    char topic[32];
    char msg[32];
    char subtopic[12];

    // TODO: this is super hacky, so figure out a better way to do this
    switch (i)
    {
      case 0:
        {
          sprintf(subtopic, "motion");
          sprintf(msg, "%d", sd.motion_avg);
          break;
        }
      case 1:
        {
          sprintf(subtopic, "light");
          dtostrf(sd.light_avg, 0, 2, msg);
          break;
        }
      case 2:
        {
          sprintf(subtopic, "ir");
          dtostrf(sd.ir_avg, 0, 2, msg);
          break;
        }
      case 3:
        {
          sprintf(subtopic, "temp");
          dtostrf(sd.temp_avg, 0, 2, msg);
          break;
        }
      case 4:
        {
          sprintf(subtopic, "rhum");
          dtostrf(sd.rel_hum_avg, 0, 2, msg);
          break;
        }
      case 5:
        {
          sprintf(subtopic, "pressure");
          dtostrf(sd.pres_avg, 0, 2, msg);
          break;
        }
      case 6:
        {
          sprintf(subtopic, "sound_avg");
          dtostrf(sd.sound_avg, 0, 2, msg);
          break;
        }
      case 7:
        {
          sprintf(subtopic, "sound_max");
          dtostrf(sd.sound_max, 0, 2, msg);
          break;
        }
    }

    sprintf(topic, "sensors/%#06x/%s", chip_id, subtopic);
    success &= client.publish(topic, msg);
  }
  return success;
}

/*
  void write_msg_to_db(char * msg)
  {
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

  if (httpCode < 1)
  {
    USE_SERIAL.printf("[HTTP] failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  else if (httpCode == HTTP_CODE_OK && isHttpGet)
  {
    String payload = http.getString();
    USE_SERIAL.println(payload);
  }
  }
*/

uint32_t previous_time = 0;
uint32_t sample_interval = 1000;

uint16_t samples_since_last_write = 0;
uint16_t db_write_rate = 2;

void loop()
{
  uint32_t current_time = millis();

  if (current_time - previous_time >= sample_interval)
  {
    previous_time = current_time;
    SENSORS_Update();

    // Print results to console
    USE_SERIAL.print("Sensors: ");
    char * inst_str = SENSORS_GetInstString();
    USE_SERIAL.println(inst_str);

    // write sensor data to database (ensure WiFi is connected)
    if (samples_since_last_write >= db_write_rate)
    {
      char * avg_str = SENSORS_GetAvgString();
      USE_SERIAL.println(avg_str);

      SensorData_t sd = SENSORS_GetSensorData();

      if (USE_WIFI && (WiFi.status() == WL_CONNECTED))
      {
        // NOTE: interrupts during WiFi write seem to cause ESP8266 module to reboot
        SENSORS_DisableInterrupts();

        if (!client.connected()) {
          reconnect();
        }
        client.loop();

        //write_data_to_db(avg_str);
        bool success = publish_sensor_data(sd);
        if (success)
        {
          USE_SERIAL.println("Publish Succeeded.");
        }
        else
        {
          USE_SERIAL.println("Publish Failed.");
        }

        SENSORS_EnableInterrupts();
      }

      SENSORS_ClearHoldValues();
      samples_since_last_write = 0;
    }

    samples_since_last_write++;
  }

  delay(100);
}



