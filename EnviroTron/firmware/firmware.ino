
#include "Sensors.h"
#include "messaging.h"
#include "common.h"

#include <Arduino.h>

extern "C" {
#include "user_interface.h"
}

#define USE_SERIAL Serial
#define USE_WIFI (true)

MsgDict_t md;

bool define_topics(MsgDict_t * md)
{
  strcpy(md->uptime.key, "uptime");
  strcpy(md->sample_count.key, "sample_count");
  strcpy(md->motion.key, "motion");

  strcpy(md->light.key, "light");
  strcpy(md->ir.key, "ir");
  strcpy(md->temp.key, "temp");
  strcpy(md->rhum.key, "rhum");
  strcpy(md->pressure.key, "pressure");
  strcpy(md->sound_avg.key, "sound_avg");
  strcpy(md->sound_max.key, "sound_max");

  return true;
}

void setup()
{
  USE_SERIAL.begin(115200);
  USE_SERIAL.setDebugOutput(true);
  USE_SERIAL.println();

  define_topics(&md);

  if (!USE_WIFI)
  {
    WIFI_TurnOff();
  }
  else
  {
    WIFI_Connect();
    MQTT_Init();
  }

  // NOTE: prevent ESP from sleeping because it causes lots of ADC noise
  wifi_set_sleep_type(NONE_SLEEP_T);

  SENSORS_Init();
  delay(1000);
  SENSORS_EnableInterrupts();
}

uint32_t previous_time = 0;
uint32_t sample_interval = 1000;
uint32_t uptime_sec = 0;

uint16_t samples_since_last_write = 0;
uint16_t publish_interval = 2;

void loop()
{
  uint32_t current_time = millis();

  if (current_time - previous_time >= sample_interval)
  {
    uptime_sec += (current_time - previous_time) / 1000;
    previous_time = current_time;
    SENSORS_Update();

    // Print latest sensor samples to console
    USE_SERIAL.print("Inst: ");
    char * inst_str = SENSORS_GetInstString();
    USE_SERIAL.println(inst_str);

    // wait until we hit the publish interval to send the averaged data
    if (samples_since_last_write >= publish_interval)
    {
      USE_SERIAL.print("Avg: ");
      char * avg_str = SENSORS_GetAvgString();
      USE_SERIAL.println(avg_str);

      // convert sensor data structure to topic data structure
      SensorData_t sd = SENSORS_GetSensorData();
      md.uptime.value = uptime_sec;
      md.sample_count.value = sd.sample_count;
      md.motion.value = sd.motion_avg;
      md.light.value = sd.light_avg;
      md.ir.value = sd.ir_avg;
      md.temp.value = sd.temp_avg;
      md.rhum.value = sd.rel_hum_avg;
      md.pressure.value = sd.pres_avg;
      md.sound_avg.value = sd.sound_avg;
      md.sound_max.value = sd.sound_max;


      if (USE_WIFI && WIFI_IsConnected())
      {
        // NOTE: interrupts during WiFi write seem to cause ESP8266 module to reboot
        SENSORS_DisableInterrupts();

        MQTT_DoClientLoop();

        bool success = MQTT_PublishMessages(&md);
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

