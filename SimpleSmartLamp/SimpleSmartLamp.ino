#include <ESPmDNS.h>

#include "/home/gal/dev/SimpleWifiLamp/SimpleSmartLamp/include/SharedSettings.hpp"
#include "/home/gal/dev/SimpleWifiLamp/SimpleSmartLamp/include/ServerHelper.hpp"

String hostname = "lamp";
SharedSettings settings;
void setup()
{
    WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
    WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
    WiFi.setHostname(hostname.c_str()); //define hostname
    // it is a good practice to make sure your code sets wifi mode how you want it.

    // put your setup code here, to run once:
    Serial.begin(115200);

    initLeds(settings);

    //WiFiManager, Local initialization. Once its business is done, there is no need to keep it around
    WiFiManager wm;

    // reset settings - wipe stored credentials for testing
    // these are stored by the esp library
    // wm.resetSettings();

    // Automatically connect using saved credentials,
    // if connection fails, it starts an access point with the specified name ( "AutoConnectAP"),
    // if empty will auto generate SSID, if password is blank it will be anonymous AP (wm.autoConnect())
    // then goes into a blocking loop awaiting configuration and will return success result

    bool res;
    res = wm.autoConnect(hostname.c_str()); // anonymous ap

    initServerStuff(settings);

    if (!res)
    {
        Serial.println("Failed to connect");
        showBlink(CRGB::Red, 5, 500);
        // ESP.restart();
    }
    else
    {
        //if you get here you have connected to the WiFi
        Serial.println("connected...yeey :)");
        showBlink(CRGB::Green, 2, 500);

        //init and get the time
        configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
        printLocalTime();
    }

    if (!MDNS.begin(hostname.c_str())) //In browser, write: http://lamp.local/
    {
        Serial.println("Error starting mDNS");
        showBlink(CRGB::Red, 5, 500);
        return;
    }

    if (!settings.preferences.begin("ledSettings", false))
    {
        Serial.println("Error initiating preferences::ledSettings");
        showBlink(CRGB::Red, 6, 500);
        return;
    }
    else
    {
        // Load stored brightness to memory
        settings.current_brightness = settings.preferences.getUChar("brightness");
        setBrightness(settings.current_brightness);

        // Load stored working mode to memory
        settings.mode = static_cast<workingMode>(settings.preferences.getUInt("mode", 0));
        switch (settings.mode)
        {
        case workingMode::stable:
            Serial.printf("Got mode from preference: stable\n");
            settings.ColorByTimeOfDay = false;
            setStableColor(settings.preferences.getULong("color", 13209363));
            break;
        case workingMode::colorByTime:
            Serial.printf("Got mode from preference: colorByTime\n");
            settings.ColorByTimeOfDay = true;
            break;
        default:
            Serial.printf("Could not infer current working mode\n");
            showBlink(CRGB::Red, 7, 500);
            break;
        }
    }

    delay(100);
}

void loop()
{
    server.handleClient();
    // put your main code here, to run repeatedly:
}
