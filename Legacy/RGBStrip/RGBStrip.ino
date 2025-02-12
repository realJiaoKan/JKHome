#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <arduino_homekit_server.h>

#define LOG_D(fmt, ...)   printf_P(PSTR(fmt "\n") , ##__VA_ARGS__);

extern "C" homekit_server_config_t accessory_config;
extern "C" homekit_characteristic_t cha_on;
extern "C" homekit_characteristic_t cha_bright;
extern "C" homekit_characteristic_t cha_sat;
extern "C" homekit_characteristic_t cha_hue;
static uint32_t next_heap_millis = 0;
const int ws2812pin = 2;
const int lednum = 120;
const char *ssid = "DD-514";
const char *password = "Judgelight900826";
bool received_sat = false;
bool received_hue = false;
bool is_on = false;
float current_brightness =  100.0;
float current_sat = 0.0;
float current_hue = 0.0;
int rgb_colors[3];

Adafruit_NeoPixel ws2812 = Adafruit_NeoPixel(lednum, ws2812pin, NEO_GRB + NEO_KHZ800);

void homekitSetup()
{
    cha_on.setter = setOnOff;
    cha_bright.setter = setBright;
    cha_sat.setter = setSat;
    cha_hue.setter = setHue;
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
    bool on = v.bool_value;
    cha_on.value.bool_value = on;
    if (on)
    {
        is_on = true;
        Serial.println("On");
    }
    else
    {
        is_on = false;
        Serial.println("Off");
    }
    updateColor();
}

void setHue(const homekit_value_t v)
{
    Serial.println("setHue");
    float hue = v.float_value;
    cha_hue.value.float_value = hue;
    current_hue = hue;
    received_hue = true;
    updateColor();
}

void setSat(const homekit_value_t v)
{
    Serial.println("setSat");
    float sat = v.float_value;
    cha_sat.value.float_value = sat;
    current_sat = sat;
    received_sat = true;
    updateColor();
}

void setBright(const homekit_value_t v)
{
    Serial.println("setBright");
    int bright = v.int_value;
    cha_bright.value.int_value = bright;
    current_brightness = bright;
    updateColor();
}

void updateColor()
{
    if (is_on)
    {
        if (received_hue && received_sat)
        {
            HSV2RGB(current_hue, current_sat, current_brightness);
            received_hue = false;
            received_sat = false;
        }
        int b = map(current_brightness, 0, 100, 75, 255);
        Serial.println(b);
        ws2812.setBrightness(b);
        for (int i = 0; i < lednum; i++) ws2812.setPixelColor(i, ws2812.Color(rgb_colors[0], rgb_colors[1], rgb_colors[2]));
        ws2812.show();
    }
    else if (!is_on)
    {
        Serial.println("is_on == false");
        ws2812.setBrightness(0);
        ws2812.clear();
        ws2812.show();
    }
}

void HSV2RGB(float h, float s, float v)
{
    int i;
    float m, n, f;
    s /= 100;
    v /= 100;
    if (s == 0)
    {
        rgb_colors[0] = rgb_colors[1] = rgb_colors[2] = round(v * 255);
        return;
    }
    h /= 60;
    i = floor(h);
    f = h - i;
    (!(i & 1))&&(f = 1 - f);
    m = v * (1 - s);
    n = v * (1 - s * f);
    switch (i)
    {
        case 0: case 6:
            rgb_colors[0] = round(v * 255);
            rgb_colors[1] = round(n * 255);
            rgb_colors[2] = round(m * 255);
            break;
        case 1:
            rgb_colors[0] = round(n * 255);
            rgb_colors[1] = round(v * 255);
            rgb_colors[2] = round(m * 255);
            break;
        case 2:
            rgb_colors[0] = round(m * 255);
            rgb_colors[1] = round(v * 255);
            rgb_colors[2] = round(n * 255);
            break;
        case 3:
            rgb_colors[0] = round(m * 255);
            rgb_colors[1] = round(n * 255);
            rgb_colors[2] = round(v * 255);
            break;
        case 4:
            rgb_colors[0] = round(n * 255);
            rgb_colors[1] = round(m * 255);
            rgb_colors[2] = round(v * 255);
            break;
        case 5:
            rgb_colors[0] = round(v * 255);
            rgb_colors[1] = round(m * 255);
            rgb_colors[2] = round(n * 255);
            break;
    }
}

void setup()
{
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.persistent(false);
    WiFi.disconnect(false);
    WiFi.setAutoReconnect(true);
    WiFi.begin(ssid, password);
    WiFi.setAutoConnect(true);
    ws2812.begin();
    ws2812.clear();
    ws2812.show();
    delay(1000);
    rgb_colors[0] = 255;
    rgb_colors[1] = 255;
    rgb_colors[2] = 255;
    homekitSetup();
}

void loop()
{
    homekitLoop();
    delay(10);
}
