#ifndef _MISURE_H
#define _MISURE_H

#include <Arduino.h>

typedef struct {
    float BME280Temperature;
    float BME280Humidity;
    float RTDResistance;
    float RTDTemperature;
    float LM35Temperature;
    uint32_t WindPeriod;
    float WindSpeed;
    float BatteryLevel;
} TMisura;

#endif