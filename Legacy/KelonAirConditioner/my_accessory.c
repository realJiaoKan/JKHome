#include <homekit/homekit.h>
#include <homekit/characteristics.h>

void my_accessory_identify(homekit_value_t _value)
{
    printf("accessory identify\n");
}

homekit_characteristic_t ACName = HOMEKIT_CHARACTERISTIC_(NAME, "JKHome Air Conditioner - Kelon");
homekit_characteristic_t currentHeatingCoolingState = HOMEKIT_CHARACTERISTIC_(CURRENT_HEATING_COOLING_STATE, 0);
homekit_characteristic_t targetHeatingCoolingState = HOMEKIT_CHARACTERISTIC_(TARGET_HEATING_COOLING_STATE, 0);
homekit_characteristic_t currentTemperature = HOMEKIT_CHARACTERISTIC_(CURRENT_TEMPERATURE, 26.0);
homekit_characteristic_t targetTemperature = HOMEKIT_CHARACTERISTIC_(TARGET_TEMPERATURE, 26.0);
homekit_characteristic_t temperatureDisplayUnit = HOMEKIT_CHARACTERISTIC_(TEMPERATURE_DISPLAY_UNITS, 0);
homekit_characteristic_t currentRelativeHumidity = HOMEKIT_CHARACTERISTIC_(CURRENT_RELATIVE_HUMIDITY, 50.0);

homekit_accessory_t *accessories[] = {
    HOMEKIT_ACCESSORY(.id = 1, .category = homekit_accessory_category_air_conditioner, .services = (homekit_service_t*[]) {
        HOMEKIT_SERVICE(ACCESSORY_INFORMATION, .characteristics = (homekit_characteristic_t*[]) {
            HOMEKIT_CHARACTERISTIC(NAME, "JKHome Air Conditioner - Kelon"),
            HOMEKIT_CHARACTERISTIC(MANUFACTURER, "JKhome"),
            HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER, "202108270124"),
            HOMEKIT_CHARACTERISTIC(MODEL, "Kelon"),
            HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, "1.1"),
            HOMEKIT_CHARACTERISTIC(IDENTIFY, my_accessory_identify),
            NULL
        }),//Define the accessory.
        HOMEKIT_SERVICE(THERMOSTAT, .primary = true, .characteristics = (homekit_characteristic_t*[]) {
            &ACName,
            &currentHeatingCoolingState,
            &targetHeatingCoolingState,
            &currentTemperature,
            &targetTemperature,
            &temperatureDisplayUnit,
            &currentRelativeHumidity,
            NULL
        }),//Add a thermostat to this accessory.
        NULL
    }),
    NULL
};

homekit_server_config_t accessory_config = {
    .accessories = accessories,
    .password = "200-00-907"
};
