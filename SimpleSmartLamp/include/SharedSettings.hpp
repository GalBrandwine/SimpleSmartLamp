#pragma once
#include "time.h"
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 2 * 60 * 60;
const int daylightOffset_sec = 3600;

struct SharedSettings
{
    bool ColorByTimeOfDay = false;
};

/**
 * @brief My implementation of Arduino's map()
 * I needed a more accurate mapping 
 * 
 * @param x 
 * @param in_min 
 * @param in_max 
 * @param out_min 
 * @param out_max 
 * @return double 
 */
double mapd(double x, double in_min, double in_max, double out_min, double out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void printLocalTime()
{
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        Serial.println("Failed to obtain time");
        return;
    }
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}