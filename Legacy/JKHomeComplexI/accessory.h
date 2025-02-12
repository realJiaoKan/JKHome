#include <Arduino.h>
#include <homekit/homekit.h>
#include <homekit/characteristics.h>
#include <esp_task_wdt.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Kelon.h>
#include <dht11.h>
#include <Adafruit_NeoPixel.h>

extern "C" homekit_server_config_t accessory_config;
extern "C" homekit_characteristic_t socketState;
extern "C" homekit_characteristic_t currentHeatingCoolingState;
extern "C" homekit_characteristic_t targetHeatingCoolingState;
extern "C" homekit_characteristic_t currentTemperature;
extern "C" homekit_characteristic_t targetTemperature;
extern "C" homekit_characteristic_t temperatureDisplayUnit;
extern "C" homekit_characteristic_t currentRelativeHumidity;
extern "C" homekit_characteristic_t RGBStripState;
extern "C" homekit_characteristic_t RGBStripBrightness;
extern "C" homekit_characteristic_t RGBStripSat;
extern "C" homekit_characteristic_t RGBStripHue;
const int IR_pin = 0;
const int DHT_pin = 4;
const int WS2812_pin = 12;
const int lednum = 120;
bool received_sat = false;
bool received_hue = false;
bool is_on = false;
float current_brightness = 100.0;
float current_sat = 0.0;
float current_hue = 0.0;
int rgb_colors[3];

IRKelonAc AC(IR_pin);
dht11 DHTSensor(DHT_pin);
Adafruit_NeoPixel WS2812 = Adafruit_NeoPixel(lednum, WS2812_pin, NEO_GRB + NEO_KHZ800);

void ACModeSetter(const homekit_value_t value);
void ACTemperatureSetter(const homekit_value_t value);
void temperatureDisplayUnitSetter(const homekit_value_t value);
void updateac();
void homekitNotify();
void setRGBStripOnOff(const homekit_value_t v);
void setRGBStripHue(const homekit_value_t v);
void setRGBStripSat(const homekit_value_t v);
void setRGBStripBrightness(const homekit_value_t v);
void updateColor();

void accessorySetupCpp()
{
    targetHeatingCoolingState.setter = ACModeSetter;
    targetTemperature.setter = ACTemperatureSetter;
    temperatureDisplayUnit.setter = temperatureDisplayUnitSetter;
    RGBStripState.setter = setRGBStripOnOff;
    RGBStripBrightness.setter = setRGBStripBrightness;
    RGBStripSat.setter = setRGBStripSat;
    RGBStripHue.setter = setRGBStripHue;
    WS2812.begin();
    WS2812.clear();
    WS2812.show();
    rgb_colors[0] = 128;
    rgb_colors[1] = 128;
    rgb_colors[2] = 128;
    updateColor();
    AC.begin();
    DHTSensor.begin();
}
void homekitNotify()
{
    homekit_characteristic_notify(&socketState, socketState.value);
    homekit_characteristic_notify(&RGBStripState, RGBStripState.value);
    homekit_characteristic_notify(&RGBStripBrightness, RGBStripBrightness.value);
    homekit_characteristic_notify(&RGBStripSat, RGBStripSat.value);
    homekit_characteristic_notify(&RGBStripHue, RGBStripHue.value);
    DHTSensor.read();
    currentTemperature.value.float_value = round(DHTSensor.temperature * 10.0) / 10.0;
    currentRelativeHumidity.value.float_value = round(DHTSensor.humidity) * 1.0;
    if (!(currentTemperature.value.float_value > 0 && currentTemperature.value.float_value < 100))
        currentTemperature.value.float_value = 26.0;
    if (!(currentRelativeHumidity.value.float_value > 0 || currentRelativeHumidity.value.float_value < 100))
        currentRelativeHumidity.value.float_value = 50.0;
    homekit_characteristic_notify(&currentHeatingCoolingState, currentHeatingCoolingState.value);
    homekit_characteristic_notify(&targetHeatingCoolingState, targetHeatingCoolingState.value);
    homekit_characteristic_notify(&currentTemperature, currentTemperature.value);
    homekit_characteristic_notify(&targetTemperature, targetTemperature.value);
    homekit_characteristic_notify(&temperatureDisplayUnit, temperatureDisplayUnit.value);
    homekit_characteristic_notify(&currentRelativeHumidity, currentRelativeHumidity.value);
}
// Air Conditioner
void ACModeSetter(const homekit_value_t value)
{
    currentHeatingCoolingState.value.uint8_value = value.uint8_value;
    targetHeatingCoolingState.value.uint8_value = value.uint8_value;
    updateac();
}
void ACTemperatureSetter(const homekit_value_t value)
{
    targetTemperature.value.float_value = value.float_value;
    updateac();
}
void temperatureDisplayUnitSetter(const homekit_value_t value)
{
    temperatureDisplayUnit.value.uint8_value = value.uint8_value;
    homekitNotify();
}
void updateac()
{
    switch (targetHeatingCoolingState.value.uint8_value)
    {
    case 0:
        AC.ensurePower(false);
        break;
    case 1:
        AC.ensurePower(true);
        AC.setMode(kKelonModeHeat);
        break;
    case 2:
        AC.ensurePower(true);
        AC.setMode(kKelonModeCool);
        break;
    case 3:
        if (currentTemperature.value.float_value > targetTemperature.value.float_value + 1.0)
            AC.ensurePower(true), AC.setMode(kKelonModeCool);
        else if (currentTemperature.value.float_value < targetTemperature.value.float_value - 1.0)
            AC.ensurePower(true), AC.setMode(kKelonModeHeat);
        else
            AC.ensurePower(false);
        break;
    }
    AC.setTemp((int)round(targetTemperature.value.float_value));
    AC.setFan(kKelonFanMax);
    AC.send();
    homekitNotify();
}
// RGB Strip
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
    (!(i & 1)) && (f = 1 - f);
    m = v * (1 - s);
    n = v * (1 - s * f);
    switch (i)
    {
    case 0:
    case 6:
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
void setRGBStripOnOff(const homekit_value_t v)
{
    bool on = v.bool_value;
    RGBStripState.value.bool_value = on;
    if (on)
        is_on = true;
    else
        is_on = false;
    updateColor();
}
void setRGBStripHue(const homekit_value_t v)
{
    float hue = v.float_value;
    RGBStripHue.value.float_value = hue;
    current_hue = hue;
    received_hue = true;
    updateColor();
}
void setRGBStripSat(const homekit_value_t v)
{
    float sat = v.float_value;
    RGBStripSat.value.float_value = sat;
    current_sat = sat;
    received_sat = true;
    updateColor();
}
void setRGBStripBrightness(const homekit_value_t v)
{
    int bright = v.int_value;
    RGBStripBrightness.value.int_value = bright;
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
        WS2812.setBrightness(b);
        for (int i = 0; i < lednum; i++)
            WS2812.setPixelColor(i, WS2812.Color(rgb_colors[0], rgb_colors[1], rgb_colors[2]));
        WS2812.show();
        delay(10);
    }
    else if (!is_on)
    {
        WS2812.setBrightness(0);
        WS2812.clear();
        WS2812.show();
        delay(10);
    }
    homekitNotify();
}