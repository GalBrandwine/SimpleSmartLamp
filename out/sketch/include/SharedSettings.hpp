#pragma once
#include <time.h>
#include <Preferences.h>

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 2 * 60 * 60;
const int daylightOffset_sec = 3600;

enum workingMode
{
    stable,
    colorByTime
};

struct SharedSettings
{
    Preferences preferences;
    workingMode mode{workingMode::stable};
    bool ColorByTimeOfDay = false;
    unsigned char current_brightness{0};
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