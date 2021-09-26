#pragma once
#include <WebServer.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <SPIFFS.h>
#include "/home/gal/dev/SimpleWifiLamp/SimpleSmartLamp/include/SharedSettings.hpp"
#include "/home/gal/dev/SimpleWifiLamp/SimpleSmartLamp/include/LedHelpers.hpp"
WebServer server(80);

void reboot()
{
    ESP.restart();
}
/**
 * @brief Reset wifi credentials and reboot,
 * 
 */
void wifi_manager_reset()
{
    WiFiManager wm;
    wm.resetSettings();

    showBlink(CRGB::HTMLColorCode::Azure, 3, 250);
    reboot();
}

/**
 * @brief Stream file using server.streamfile
 * 
 * https://forum.arduino.cc/t/sending-client-a-web-page-stored-in-spiffs/598958
 * 
 */
void handle_OnConnect()
{
    // LED1status = LOW;
    // LED2status = LOW;
    // Serial.println("GPIO4 Status: OFF | GPIO5 Status: OFF");
    // server.send(200, "text/html", SendHTML(LED1status, LED2status));
    auto file = SPIFFS.open("/index.html", "r");
    server.streamFile(file, "text/html");
    file.close();
    // server.send(SPIFFS, "/index.html", String(), false);
}

void handle_notFound()
{
    server.send(404, "text/plain", "The content you are looking for was not found.");
}

void initServerStuff(SharedSettings &settings)
{
    server.on("/", handle_OnConnect);

    /**
     * @brief I usually stick to formal naming. Wanted to check something with the .css file though
     * 
     */
    server.on("/silly_unicorn.jpg", []()
              {
                  auto file = SPIFFS.open("/silly_unicorn.jpg", "r");
                  server.streamFile(file, "text/plain");
                  file.close();
              });

    server.on("/gals_best_style_ever.css", []()
              {
                  auto file = SPIFFS.open("/gals_best_style_ever.css", "r");
                  server.streamFile(file, "text/css");
                  file.close();
              });

    server.on("/wifiManager/reset", []()
              {
                  wifi_manager_reset();
                  reboot();
              });

    server.on("/color_selector.php", [&settings]()
              {
                  //   // Method 1
                  //   Serial.println(" - ");
                  //   for (int arg = 0; arg < server.args(); arg++)
                  //   {
                  //       Serial.print("Got:");
                  //       Serial.print(server.argName(arg));
                  //       Serial.print(" - ");
                  //       Serial.println(server.arg(arg));
                  //   }
                  //   Serial.println(" - ");

                  String val;

                  // Method 2
                  if (server.hasArg("timeofday"))
                  {
                      //   auto val = server.arg(1);
                      Serial.println("Got: timeofday");
                      settings.ColorByTimeOfDay = true;
                  }
                  else
                      settings.ColorByTimeOfDay = false;

                  if (server.hasArg("favcolor"))
                  {
                      Serial.println("Got: favcolor");
                      auto arg = server.arg(0);
                      Serial.println(arg);

                    //   auto R = arg.substring(1, 3);
                    //   Serial.println(R);
                    //   Serial.println(atoi(R.c_str()));

                    //   unsigned char buf[6]{0};
                    //   val.getBytes(buf, 6);
                    //   for (int i = 0; i < 6; i++)
                    //   {
                    //       Serial.println(buf[i]);
                    //   }

                    //   Serial.println(atoi(arg.substring(1).c_str()));
                      setStableColor(0);
                  }

                  auto reply = "Got " + val;
                  server.send(200, "text/plain", "");
              });

    server.onNotFound(handle_notFound);

    /**
   * @brief Initialize SPIFFS
   * 
   */
    if (!SPIFFS.begin(true))
    {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }

    server.begin();
    Serial.println("HTTP server started");
}