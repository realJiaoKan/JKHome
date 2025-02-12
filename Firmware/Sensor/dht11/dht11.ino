#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <dht11.h>
#include "config.h"

// Enable SSL?
#define SSLEnable

// Hareware configuration
#define dht11Pin 2
dht11 DHT11;

#ifdef SSLEnable
WiFiClientSecure wifiClient;
#else
WiFiClient wifiClient;
#endif
PubSubClient mqttClient(wifiClient);

void update();

void setup()
{
    // Hareware setup
    pinMode(dht11Pin, INPUT);
    Serial.begin(115200);
    Serial.println("");

    // WiFi setup
    Serial.print("[WiFi] Connecting WiFi with ssid: ");
    Serial.print(ssid);
    Serial.print(" Password: ");
    Serial.print(password);
    Serial.println();
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
#ifdef SSLEnable
    while (!wifiClient.setFingerprint(fingerprint))
    {
        Serial.println("[WiFi] SSL Fatal Error...");
        delay(5000);
    }
#endif
    Serial.print("[WiFi] Connecting WiFi...");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.println("[WiFi] WiFi connected");
    Serial.println("[WiFi] IP address: " + WiFi.localIP().toString());
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);

    // MQTT setup
    mqttClient.setServer(mqttDomain, mqttPort);
}

void loop()
{
    // MQTT loop
    while (!mqttClient.connected())
    {
        Serial.println("[MQTT] Not connected to MQTT server! Establishing connection... | State: " + String(mqttClient.state()));
        if (mqttClient.connect(mqttID[0], mqttUsername, mqttPassword))
        {
            Serial.println("[MQTT] Connected");
            for (int i = 0; i < sizeof(mqttID) / sizeof(mqttID[0]); i++)
                mqttClient.subscribe(mqttID[i]);
        }
        else
            delay(5000);
    }
    update();
    delay(2000);
    mqttClient.loop();
}

void update()
{
    DHT11.read(dht11Pin);
    char humidity[10], temperature[10];
    sprintf(humidity, "%f", DHT11.humidity), sprintf(temperature, "%f", DHT11.temperature);
    mqttClient.publish(mqttID[0], humidity);
    mqttClient.publish(mqttID[1], temperature);
}