#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <arduino_homekit_server.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Kelon.h>
#include <DHT.h>

extern "C" homekit_server_config_t accessory_config;
extern "C" homekit_characteristic_t currentHeatingCoolingState;
extern "C" homekit_characteristic_t targetHeatingCoolingState;
extern "C" homekit_characteristic_t currentTemperature;
extern "C" homekit_characteristic_t targetTemperature;
extern "C" homekit_characteristic_t temperatureDisplayUnit;
extern "C" homekit_characteristic_t currentRelativeHumidity;
static long long nextNotifyTime = 0;
const int IRPin = 3;//RX on ESP-01.
const int DHTPin = 2;//IO2 on ESP-01.
const char *ssid = "DD-514";//Your WiFi SSID.
const char *password = "Judgelight900826";//Your WiFi Password, just ignore the encrypt method.

IRKelonAc AC(IRPin);
DHT DHTSensor(DHTPin, DHT11);//You can replace "DHT11" with "DHT21" and "DHT22", depend on your sensor.

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
            if (currentTemperature.value.float_value > targetTemperature.value.float_value + 1.0) AC.ensurePower(true), AC.setMode(kKelonModeCool);
            else if (currentTemperature.value.float_value < targetTemperature.value.float_value - 1.0) AC.ensurePower(true), AC.setMode(kKelonModeHeat);
            else AC.ensurePower(false);
            break;
    }
    AC.setTemp((int)round(targetTemperature.value.float_value));
    AC.setFan(kKelonFanMax);
    AC.send();
    homekitNotify();
}

void homekitNotify()
{
    currentTemperature.value.float_value = round(DHTSensor.readTemperature() * 10.0) / 10.0;
    currentRelativeHumidity.value.float_value = round(DHTSensor.readHumidity()) * 1.0;
    if (!(currentTemperature.value.float_value > 0 && currentTemperature.value.float_value < 100)) currentTemperature.value.float_value = 26.0;
    if (!(currentRelativeHumidity.value.float_value > 0 || currentRelativeHumidity.value.float_value < 100)) currentRelativeHumidity.value.float_value = 50.0;
    //Serial.print("[TEST]");
    //Serial.println(currentTemperature.value.float_value);
    homekit_characteristic_notify(&currentHeatingCoolingState, currentHeatingCoolingState.value);
    homekit_characteristic_notify(&targetHeatingCoolingState, targetHeatingCoolingState.value);
    homekit_characteristic_notify(&currentTemperature, currentTemperature.value);
    homekit_characteristic_notify(&targetTemperature, targetTemperature.value);
    homekit_characteristic_notify(&temperatureDisplayUnit, temperatureDisplayUnit.value);
    homekit_characteristic_notify(&currentRelativeHumidity, currentRelativeHumidity.value);
}

void homekitSetup()
{
    targetHeatingCoolingState.setter = ACModeSetter;
    targetTemperature.setter = ACTemperatureSetter;
    temperatureDisplayUnit.setter = temperatureDisplayUnitSetter;
    arduino_homekit_setup(&accessory_config);
}

void homekitLoop()
{
    arduino_homekit_loop();
    const uint32_t timer = millis();
    if (timer > nextNotifyTime)
    {
        nextNotifyTime = timer + 2 * 1000;
        homekitNotify();
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
    AC.begin();
    DHTSensor.begin();
    delay(1000);
    homekitSetup();
}

void loop()
{
    homekitLoop();
    delay(10);
}
