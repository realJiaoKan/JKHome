#include <Arduino.h>
#include <WiFi.h>
#include <arduino_homekit_server.h>
#include "accessory.h"
#include "bluetooth.h"

extern "C" homekit_server_config_t accessory_config;
extern "C" void accessorySetupC();
/*
 * Because of the "magic" compiler of Arduino,
 * .h and .ino file here will be treated as cpp file,
 * however, it also support .c file,
 * which will be treated as c file.
 * In this project,
 * because the libraries are based on different language,
 * some are based on c while others are on cpp.
 * Therefore, I have to divide some functions into two parts,
 * one for c-based ones, another for cpp-based ones.
 * F**K.
 */
const char *ssid = "DD-514";
const char *password = "Judgelight900826";
long long next_notify_time = 0;

void setup()
{
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.persistent(false);
    WiFi.disconnect(false);
    WiFi.setAutoReconnect(true);
    WiFi.begin(ssid, password);
    WiFi.setAutoConnect(true);
    accessorySetupC();
    accessorySetupCpp();
    delay(1000);
    arduino_homekit_setup(&accessory_config);
    BTSetup();
}
void loop()
{
    arduino_homekit_loop();
    BTLoop();
    const uint32_t timer = millis();
    if (timer > next_notify_time)
    {
        next_notify_time = timer + 3 * 1000;
        homekitNotify();
        BTNotify();
    }
    delay(100);
}