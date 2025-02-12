#include <Arduino.h>
#include <homekit/homekit.h>
#include <homekit/characteristics.h>

const int socket_pin = 2;

void my_accessory_identify(homekit_value_t _value);
void accessorySetup();
void setSocketOnOff(const homekit_value_t v);

homekit_characteristic_t socketState = HOMEKIT_CHARACTERISTIC_(ON, false, .setter = setSocketOnOff);
homekit_characteristic_t currentHeatingCoolingState = HOMEKIT_CHARACTERISTIC_(CURRENT_HEATING_COOLING_STATE, 0);
homekit_characteristic_t targetHeatingCoolingState = HOMEKIT_CHARACTERISTIC_(TARGET_HEATING_COOLING_STATE, 0);
homekit_characteristic_t currentTemperature = HOMEKIT_CHARACTERISTIC_(CURRENT_TEMPERATURE, 26.0);
homekit_characteristic_t targetTemperature = HOMEKIT_CHARACTERISTIC_(TARGET_TEMPERATURE, 26.0);
homekit_characteristic_t temperatureDisplayUnit = HOMEKIT_CHARACTERISTIC_(TEMPERATURE_DISPLAY_UNITS, 0);
homekit_characteristic_t currentRelativeHumidity = HOMEKIT_CHARACTERISTIC_(CURRENT_RELATIVE_HUMIDITY, 50.0);
homekit_characteristic_t RGBStripState = HOMEKIT_CHARACTERISTIC_(ON, false);
homekit_characteristic_t RGBStripBrightness = HOMEKIT_CHARACTERISTIC_(BRIGHTNESS, 100);
homekit_characteristic_t RGBStripSat = HOMEKIT_CHARACTERISTIC_(SATURATION, (float)0);
homekit_characteristic_t RGBStripHue = HOMEKIT_CHARACTERISTIC_(HUE, (float)180);
/*
注意此处函数最后有一个下划线,HOMEKIT_CHARACTERISTIC和HOMEKIT_CHARACTERISTIC_可以理解成一个东西,只是为了满足语法而设
在下方accessories中,直接定义,不需要在外部再完善定义的可以直接使用HOMEKIT_CHARACTERISTIC,
若是需要attach一个setter上去的,则需要参考socketState来书写
*/
homekit_accessory_t *accessories[] = {
    HOMEKIT_ACCESSORY(
            .id = 1,
            .category = homekit_accessory_category_bridge,
            .services =
                (homekit_service_t *[]){
                    HOMEKIT_SERVICE(
                        ACCESSORY_INFORMATION,
                        .characteristics =
                            (homekit_characteristic_t *[]){
                                HOMEKIT_CHARACTERISTIC(NAME, "JKHome Complex I"),
                                HOMEKIT_CHARACTERISTIC(MANUFACTURER, "JKhome"),
                                HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER, "202301111549"),
                                HOMEKIT_CHARACTERISTIC(MODEL, "JKHomeComplexI"),
                                HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, "1.0"),
                                HOMEKIT_CHARACTERISTIC(IDENTIFY, my_accessory_identify),
                                NULL}),
                    NULL}),
    HOMEKIT_ACCESSORY(
            .id = 2,
            .category = homekit_accessory_category_lightbulb,
            .services =
                (homekit_service_t *[]){
                    HOMEKIT_SERVICE(
                        ACCESSORY_INFORMATION,
                        .characteristics =
                            (homekit_characteristic_t *[]){
                                HOMEKIT_CHARACTERISTIC(NAME, "JKHome Smart Socket"),
                                HOMEKIT_CHARACTERISTIC(IDENTIFY, my_accessory_identify),
                                NULL}),
                    HOMEKIT_SERVICE(
                        LIGHTBULB,
                        .primary = true,
                        .characteristics =
                            (homekit_characteristic_t *[]){
                                HOMEKIT_CHARACTERISTIC(NAME, "JKHome Smart Socket"),
                                &socketState,
                                NULL}),
                    NULL}),
    HOMEKIT_ACCESSORY(
            .id = 3,
            .category = homekit_accessory_category_air_conditioner,
            .services =
                (homekit_service_t *[]){
                    HOMEKIT_SERVICE(
                        ACCESSORY_INFORMATION,
                        .characteristics =
                            (homekit_characteristic_t *[]){
                                HOMEKIT_CHARACTERISTIC(NAME, "JKHome Air Conditioner"),
                                HOMEKIT_CHARACTERISTIC(IDENTIFY, my_accessory_identify),
                                NULL}), // Define the accessory.
                    HOMEKIT_SERVICE(
                        THERMOSTAT,
                        .primary = true,
                        .characteristics =
                            (homekit_characteristic_t *[]){
                                HOMEKIT_CHARACTERISTIC(NAME, "JKHome Air Conditioner"),
                                &currentHeatingCoolingState,
                                &targetHeatingCoolingState,
                                &currentTemperature,
                                &targetTemperature,
                                &temperatureDisplayUnit,
                                &currentRelativeHumidity,
                                NULL}), // Add a thermostat to this accessory.
                    NULL}),
    HOMEKIT_ACCESSORY(
            .id = 4,
            .category = homekit_accessory_category_lightbulb,
            .services =
                (homekit_service_t *[]){
                    HOMEKIT_SERVICE(
                        ACCESSORY_INFORMATION,
                        .characteristics =
                            (homekit_characteristic_t *[]){
                                HOMEKIT_CHARACTERISTIC(NAME, "JKHome RGB Strip"),
                                HOMEKIT_CHARACTERISTIC(IDENTIFY, my_accessory_identify),
                                NULL}),
                    HOMEKIT_SERVICE(
                        LIGHTBULB,
                        .primary = true,
                        .characteristics =
                            (homekit_characteristic_t *[]){
                                HOMEKIT_CHARACTERISTIC(NAME, "JKHome RGB Strip"),
                                &RGBStripState,
                                &RGBStripBrightness,
                                &RGBStripSat,
                                &RGBStripHue,
                                NULL}),
                    NULL}),
    NULL};
homekit_server_config_t accessory_config = {
    .accessories = accessories,
    .password = "200-00-907"};

void my_accessory_identify(homekit_value_t _value)
{
    printf("accessory identify\n");
}
void accessorySetupC()
{
    pinMode(socket_pin, OUTPUT);
    digitalWrite(socket_pin, LOW);
}
// Socket
void setSocketOnOff(const homekit_value_t v)
{
    socketState.value.bool_value = v.bool_value;
    digitalWrite(socket_pin, socketState.value.bool_value ? LOW : HIGH);
}