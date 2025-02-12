#include <Arduino.h>
#include <WiFi.h>
#include <arduino_homekit_server.h>

#define LOG_D(fmt, ...)   printf_P(PSTR(fmt "\n") , ##__VA_ARGS__);

extern "C" homekit_server_config_t accessory_config;
extern "C" homekit_characteristic_t cha_on;
static long long next_heap_millis = 0;
const int switchPin = 0;//The control pin of the relay module (Using ESP-01).
const char *ssid = "DD-514";
const char *password = "Judgelight900826";

void homekitSetup()
{
    cha_on.setter = setOnOff;
    arduino_homekit_setup(&accessory_config);
}

void homekitLoop()
{
    arduino_homekit_loop();
    const uint32_t t = millis();
    if (t > next_heap_millis)
    {
        next_heap_millis = t + 5 * 1000;
        LOG_D("Free heap: %d, HomeKit clients: %d", ESP.getFreeHeap(), arduino_homekit_connected_clients_count());
    }
}

void setOnOff(const homekit_value_t v)
{
    cha_on.value.bool_value = v.bool_value;
    digitalWrite(switchPin, cha_on.value.bool_value ? LOW : HIGH);
}

void setup()
{
    pinMode(switchPin, OUTPUT);
    digitalWrite(switchPin, LOW);
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.persistent(false);
    WiFi.disconnect(false);
    WiFi.setAutoReconnect(true);
    WiFi.begin(ssid, password);
    WiFi.setAutoConnect(true);
    delay(1000);
    homekitSetup();
}

void loop()
{
    homekitLoop();
    delay(10);
}
