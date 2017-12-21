#pragma once

#include <Arduino.h>

typedef struct
{
  char key[64];
  uint32_t value;
} KvUint32_t;

typedef struct
{
  char key[64];
  float value;
} KvFloat_t;

typedef union
{
  struct
  {
    KvUint32_t u32_topics[3];
    KvFloat_t float_topics[7];
  };
  struct
  {
    KvUint32_t uptime;
    KvUint32_t sample_count;
    KvUint32_t motion;

    KvFloat_t light;
    KvFloat_t ir;
    KvFloat_t temp;
    KvFloat_t rhum;
    KvFloat_t pressure;
    KvFloat_t sound_avg;
    KvFloat_t sound_max;
  };
} MsgDict_t;


bool WIFI_IsConnected();

void WIFI_TurnOff();

void WIFI_Connect();

void MQTT_Init();

void MQTT_DoClientLoop();

bool MQTT_PublishMessages(MsgDict_t * md);

