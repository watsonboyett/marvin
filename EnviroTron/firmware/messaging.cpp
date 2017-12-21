
#include "messaging.h"
#include "creds.h"
#include "common.h"

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

extern "C" {
#include "user_interface.h"
}

#define USE_SERIAL Serial

WiFiClient espClient;
PubSubClient client(espClient);
IPAddress broker(192, 168, 0, 186);

int chip_id = 0;
char chip_id_str[7];

bool WIFI_IsConnected()
{
  return WiFi.status() == WL_CONNECTED;
}

void WIFI_TurnOff()
{
  WiFi.mode(WIFI_OFF);
}

void WIFI_Connect()
{
  chip_id = ESP.getChipId();
  sprintf(chip_id_str, "%#06x", chip_id);
  USE_SERIAL.printf("Chip ID: %s \n", chip_id_str);
  String core_version = String("Core Version: ") + ESP.getCoreVersion();
  USE_SERIAL.println(core_version);
  String sdk_version = String("SDK Version: ") + ESP.getSdkVersion();
  USE_SERIAL.println(sdk_version);

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



void MQTT_Callback(char* topic, byte* payload, unsigned int length)
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

void MQTT_Init()
{
  client.setServer(broker, 1883);
  client.setCallback(MQTT_Callback);
}

void MQTT_Reconnect()
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

void MQTT_DoClientLoop()
{
  if (!client.connected()) {
    MQTT_Reconnect();
  }
  client.loop();
}

bool MQTT_PublishMessages(MsgDict_t * md)
{
  bool success = 1;
  for (int i = 0; i < 3; i++)
  {
    char topic[38];
    sprintf(topic, "sensors/%#06x/%s", chip_id, md->u32_topics[i].key);
    char msg[32];
    sprintf(msg, "%u", md->u32_topics[i].value);
    success &= client.publish(topic, msg);
  }

  for (int i = 0; i < 7; i++)
  {
    char topic[38];
    sprintf(topic, "sensors/%#06x/%s", chip_id, md->float_topics[i].key);
    char msg[32];
    dtostrf(md->float_topics[i].value, 0, 2, msg);
    success &= client.publish(topic, msg);
  }

  return success;
}

