#pragma once

#include <Arduino.h>

#define MCP3221_VOLTAGE_MIN (0.0)  // in Volts
#define MCP3221_VOLTAGE_MAX (3.3)  // in Volts
#define MCP3221_RAW_MAX   (4096.0)

void MCP3221_MeasureLevel();

uint16_t MCP3221_GetLevel_Raw();

float MCP3221_GetLevel_V();

