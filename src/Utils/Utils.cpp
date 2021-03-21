#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include "LittleFS.h"

#define GREEN_PIN 5
#define RED_PIN 4

void connectionStatus(int pin)
{
  digitalWrite(pin, HIGH);
  delay(500);
  digitalWrite(pin, LOW);
  delay(200);
  digitalWrite(pin, HIGH);
  delay(500);
  digitalWrite(pin, LOW);
  delay(200);
  digitalWrite(pin, HIGH);
  delay(500);
  digitalWrite(pin, LOW);
}

void createAccessPoint()
{
  WiFi.softAP("ESP_Soft_AP", "ocarina3131");
  Serial.println("");
  Serial.println("Access Point connected at: ");
  Serial.println(WiFi.softAPIP());
}

void connectToWifiOrCreateAP(bool blinkOnReady)
{
  File config = LittleFS.open("/config.json", "r");
  if (!config)
  {
    Serial.println("Failed to open data file");
  }
  DynamicJsonDocument configJson(1024);
  DeserializationError err = deserializeJson(configJson, config);
  if (err)
  {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(err.c_str());
  }
  config.close();

  const char *ssid = configJson["ssid"];
  const char *password = configJson["password"];

  if (configJson["ssid"].isNull() == 1 && configJson["password"].isNull() == 1)
  {
    createAccessPoint();
  }
  else
  {
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    int i = 0;
    while (WiFi.status() != WL_CONNECTED && i < 50)
    {
      i++;
      digitalWrite(LED_BUILTIN, LOW);
      delay(500);
      Serial.print(".");
      digitalWrite(LED_BUILTIN, HIGH);
      delay(500);
    }
    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println("Connection failed");
      connectionStatus(RED_PIN);

      createAccessPoint();
    }
    else
    {
      Serial.println("");
      Serial.println("WiFi connected at IP address:");
      String localIP = WiFi.localIP().toString();
      Serial.println(localIP);

      connectionStatus(GREEN_PIN);
    }
  }
  if (blinkOnReady)
  {
    connectionStatus(GREEN_PIN);
  }
}