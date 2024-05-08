#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "config.h"

// Enable SSL?
#define SSLEnable

// Hareware configuration
#define onoffPin 2

#ifdef SSLEnable
WiFiClientSecure wifiClient;
#else
WiFiClient wifiClient;
#endif
PubSubClient mqttClient(wifiClient);

void callback(char *topic, byte *_payload, unsigned int length);

void setup()
{
    // Hareware setup
    pinMode(onoffPin, OUTPUT);
    Serial.begin(115200);
    Serial.println("");

    // WiFi setup
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
    mqttClient.setCallback(callback);
}

void loop()
{
    // MQTT loop
    while (!mqttClient.connected())
    {
        Serial.println("[MQTT] Not connected to MQTT server! Establishing connection... | State: " + String(mqttClient.state()));
        if (mqttClient.connect(mqttID, mqttUsername, mqttPassword))
        {
            Serial.println("[MQTT] Connected");
            mqttClient.subscribe(mqttID);
        }
        else
            delay(5000);
    }
    mqttClient.loop();
}

void callback(char *topic, byte *_payload, unsigned int length)
{
    // Print the message and translate the payload
    String payload = "";
    for (int i = 0; i < length; i++)
        payload += (char)_payload[i];
    Serial.println("[MQTT] Message arrived - Topic:" + String(topic) + "   Payload:" + payload);

    // Cope with the message
    if (payload == "on")
        digitalWrite(onoffPin, HIGH);
    else
        digitalWrite(onoffPin, LOW);
}