#pragma once
#include <FastLED.h>
#include <math.h>
#define DATA_PIN 23
#define LED_TYPE WS2812
#define COLOR_ORDER GRB

#define NUM_OF_LEDS 8
CRGB leds[NUM_OF_LEDS];

/**
 * @brief switches off all LEDs
 * 
 * @param delayTime 
 */
void showProgramCleanUp(long delayTime)
{
    for (int i = 0; i < NUM_OF_LEDS; ++i)
    {
        leds[i] = CRGB::Black;
    }
    FastLED.show();
    FastLED.delay(delayTime);
}

void setStableColor(int color)
{
    Serial.println(__FUNCTION__);
    Serial.println(color);

    for (int i = 0; i < NUM_OF_LEDS; ++i)
    {
        leds[i] = color;
    }
    FastLED.show();
}

void setBrightness(char brightness)
{
    FastLED.setBrightness(brightness);
    FastLED.show();
}
/**
 * @brief Bli
 * 
 * @param color 
 * @param num_of_blinks 
 * @param delay_between_blink 
 */
void showBlink(CRGB::HTMLColorCode color, int num_of_blinks, int delay_between_blink)
{
    for (int blink = 0; blink < num_of_blinks; blink++)
    {
        for (int i = 0; i < NUM_OF_LEDS; ++i)
        {
            leds[i] = color;
        }
        FastLED.delay(delay_between_blink);
        FastLED.show();

        for (int i = 0; i < NUM_OF_LEDS; ++i)
        {
            leds[i] = CRGB::Black;
        }
        FastLED.delay(delay_between_blink);
        FastLED.show();
    }
}
/**
 * @brief Change CHSV angle around given color
 * 
 * @param current_color 
 * @param angle_width [0-360]
 */
void medusaEffect(int current_color, int angle_width, int delay = 150)
{
    auto angle = -angle_width;
    for (; angle < angle_width; angle++)
    {
        for (int i = 0; i < NUM_OF_LEDS; ++i)
        {
            leds[i].setHue(current_color + angle);
        }
        FastLED.show();
        FastLED.delay(delay);
    }
    for (; angle > -angle_width; angle--)
    {
        for (int i = 0; i < NUM_OF_LEDS; ++i)
        {
            leds[i].setHue(current_color + angle);
        }
        FastLED.show();
        FastLED.delay(delay);
    }
}

/**
 * @brief Map time of day to color
 * 
 * Black is 0x000000
 * White is 0xFFFFFF ~ 16777215
 * 
 * I want to map time of day so that Ill get a curve with maximum value at FFFFFF
 * 
 * Using reversed curved bell function (Gaussian)
 * I can get the desired mapping from time to HTML color codes.
 * 
 * current_color = MAX_COLOR* (1/STD*sqrt(2pi))*e^((-1/2)*((current_minute- MEAN) / STD)^2)
 * 
 * @param time 
 * @return const CRGB::HTMLColorCode 
 */
const double timeToHue()
{
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        Serial.println("Failed to obtain time");
        return CRGB::HTMLColorCode::YellowGreen;
    }
    // const auto MAX_COLOR = 16777215; // White color - this is the peak of the curve bell
    const auto HRS_IN_DAY = 24;
    const auto MAX_HUE = 1.0; // White color - this is the peak of the curve bell
    const auto STD = 0.4;     // Spread the distribution over 60 minutes
    const auto MEAN = 0;      // Shift the distribution from 0 to 60
    auto current_hr = timeinfo.tm_hour;
    Serial.println("Current hr: ");
    Serial.println(current_hr);

    auto mapped = mapd(current_hr, 0, HRS_IN_DAY, -1.0, 1.0);
    Serial.println("Current mapped [-1,1]: ");
    Serial.println(mapped);

    // Get normalized hue relevant to current time of day using gaussian's curved bell equation.
    double normalized_hue = MAX_HUE * (1.0 / (STD * sqrt(2 * PI))) * exp((-1.0 / 2.0) * pow(((mapped - MEAN) / STD), 2.0));

    Serial.println("Current normalized_hue [0,360]: ");
    Serial.println(normalized_hue);

    return normalized_hue;
}

void ColorByTimeOfDayTask(void *pvParameters)
{
    int TICKS = 100;
    Serial.println(__FUNCTION__);
    for (;;)
    {
        if ((*(SharedSettings *)pvParameters).ColorByTimeOfDay)
        {
            auto current_hue = timeToHue();
            Serial.print("Current current_hue: ");
            Serial.println(current_hue * 360);
            medusaEffect(current_hue * 360, random(0, 60));
        }
        else
        {
            // Serial.println("No ColorByTimeOfDay");
            vTaskDelay(pdMS_TO_TICKS(TICKS * 10));
        }
    }
    vTaskDelete(NULL);
}

/**
 * @brief Init the FastLED driver.
 * 
 */
void initLeds(SharedSettings &settings)
{
    delay(500);                                                                                         // initial delay of a few seconds is recommended
    FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_OF_LEDS).setCorrection(TypicalLEDStrip); // initializes LED strip
    FastLED.setBrightness(255);                                                                         // global brightness
    showProgramCleanUp(100);
    setStableColor(CRGB::HTMLColorCode::Orange);

    randomSeed(analogRead(0));

    xTaskCreate(
        ColorByTimeOfDayTask, // Function that should be called
        "ColorByTimeOfDay",   // Name of the task (for debugging)
        10000,                // Stack size (bytes)
        (void *)&settings,    // Parameter to pass
        1,                    // Task priority
        NULL                  // Task handle
    );

    // medusaEffect(60, 15);
}