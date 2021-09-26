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

    delay(100);
}

void loop()
{
    server.handleClient();
    // put your main code here, to run repeatedly:
}
