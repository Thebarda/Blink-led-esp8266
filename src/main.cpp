#include <Arduino.h>
#include "./Server/Server.h"
#include "./Utils/Utils.h"
#include "LittleFS.h"

#define GREEN_PIN 5
#define RED_PIN 4

void setup()
{
  Serial.begin(115200);

  if (!LittleFS.begin())
  {
    // Serious problem
    Serial.println("LittleFS Mount failed");
  }
  else
  {
    Serial.println("LittleFS Mount succesfull");
  }

  Serial.println("");

  pinMode(GREEN_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  connectToWifiOrCreateAP(true);

  startServer();
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop()
{
  handleClient();
}