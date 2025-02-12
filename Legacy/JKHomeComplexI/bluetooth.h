#include <Arduino.h>
#include <SoftwareSerial.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <homekit/homekit.h>
#include <homekit/characteristics.h>

extern "C" homekit_characteristic_t socketState;
extern "C" homekit_characteristic_t targetHeatingCoolingState;
extern "C" homekit_characteristic_t currentTemperature;
extern "C" homekit_characteristic_t targetTemperature;
extern "C" homekit_characteristic_t currentRelativeHumidity;
extern "C" homekit_characteristic_t RGBStripState;
extern "C" homekit_characteristic_t RGBStripBrightness;
extern "C" homekit_characteristic_t RGBStripSat;
extern "C" homekit_characteristic_t RGBStripHue;
extern "C" long long next_notify_time;

extern "C" void setSocketOnOff(const homekit_value_t v);
extern void ACModeSetter(const homekit_value_t value);
extern void ACTemperatureSetter(const homekit_value_t value);
extern void setRGBStripOnOff(const homekit_value_t v);

void BTSetup()
{
    Serial1.begin(115200);
}
void BTNotify()
{
    Serial1.print("socketState:" + (String)socketState.value.bool_value + ";");
    Serial1.print("targetHeatingCoolingState:" + (String)targetHeatingCoolingState.value.uint8_value + ";");
    Serial1.print("currentTemperature:" + (String)currentTemperature.value.float_value + ";");
    Serial1.print("targetTemperature:" + (String)targetTemperature.value.float_value + ";");
    Serial1.print("currentRelativeHumidity:" + (String)currentRelativeHumidity.value.float_value + ";");
    Serial1.print("RGBStripState:" + (String)RGBStripState.value.bool_value + ";");
    Serial1.println("[END]");
}
void BTLoop()
{
    if (Serial1.available() && Serial1.peek() != -1)
    {
        String buffer = Serial1.readStringUntil(';');
        String item = buffer.substring(0, buffer.indexOf(":"));
        String value = buffer.substring(buffer.indexOf(":") + 1, buffer.length());
        homekit_value_t temp;
        if (item == "socketState")
            temp.bool_value = value == "1" ? true : false, setSocketOnOff(temp), homekitNotify();
        else if (item == "targetHeatingCoolingState")
            temp.uint8_value = value.toInt(), ACModeSetter(temp);
        else if (item == "targetTemperature")
            temp.float_value = value.toFloat(), ACTemperatureSetter(temp);
        else if (item == "RGBStripState")
            temp.bool_value = value == "1" ? true : false, setRGBStripOnOff(temp);
        else if (item == "reset")
            temp.bool_value = value == "1" ? true : false, ESP.restart();
        else
            return;
        next_notify_time = millis() + 3 * 1000;
        BTNotify();
    }
}