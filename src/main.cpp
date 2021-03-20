#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include "LittleFS.h"

#define GREEN_PIN 5
#define RED_PIN 4
#define HTTP_REST_PORT 80

ESP8266WebServer server(HTTP_REST_PORT);

int status = LOW;

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
    WiFi.softAP("ESP_Soft_AP", "ocarina3131");
    Serial.println("");
    Serial.println("Access Point connected at: ");
    Serial.println(WiFi.softAPIP());
  }
  else
  {
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected at IP address:");
    Serial.println(WiFi.localIP());
  }
  if (blinkOnReady)
  {
    connectionStatus(GREEN_PIN);
  }
}

void getLedStatus()
{
  if (status == HIGH)
  {
    server.send(200, "application/json", "{\"status\": true}");
  }
  else
  {

    server.send(200, "application/json", "{\"status\": false}");
  }
}

void toggleLed()
{
  if (status == LOW)
  {
    status = HIGH;
  }
  else
  {
    status = LOW;
  }
  digitalWrite(GREEN_PIN, status);
  getLedStatus();
}

void home()
{
  File config = LittleFS.open("/config.json", "r");
  if (!config)
  {
    server.sendHeader("Location", "/setup", true);
    server.send(302, "text/plane", "");
    return;
  }

  DynamicJsonDocument configJson(1024);
  deserializeJson(configJson, config.readString());

  if (configJson["ssid"].isNull() == 1 && configJson["password"].isNull() == 1)
  {
    server.sendHeader("Location", "/setup", true);
    server.send(302, "text/plane", "");
    config.close();
    return;
  }

  config.close();

  File index = LittleFS.open("/index.html", "r");

  server.send(200, "text/html", index.readString());
  index.close();
}

void configureWifi()
{
  String postBody = server.arg("plain");
  DynamicJsonDocument newConfig(1024);
  deserializeJson(newConfig, postBody);

  const char *ssid = newConfig["ssid"];
  const char *password = newConfig["password"];

  Serial.println("Configuring new WiFi...");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.softAPdisconnect(true);
  WiFi.disconnect();
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

    connectToWifiOrCreateAP(false);

    server.send(400, "application/json", "{\"success\": false}");
  }
  else
  {
    Serial.println("");
    Serial.println("WiFi connected at IP address:");
    Serial.println(WiFi.localIP());

    File config = LittleFS.open("/config.json", "w");

    serializeJson(newConfig, config);

    config.close();

    connectionStatus(GREEN_PIN);

    server.send(200, "application/json", "{\"success\": true}");
  }
}

void restServerRouting()
{
  server.on(F("/toggleLed"), HTTP_GET, toggleLed);
  server.on(F("/ledStatus"), HTTP_GET, getLedStatus);
  server.on(F("/configureWifi"), HTTP_POST, configureWifi);
  server.on(F("/"), HTTP_GET, home);
}

void handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

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

  // Set server routing
  restServerRouting();
  // Set not found response
  server.onNotFound(handleNotFound);
  server.serveStatic("/js", LittleFS, "/js");
  server.serveStatic("/setup", LittleFS, "/setup.html");
  // Start server
  server.begin();
  Serial.println("HTTP server started");
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop()
{
  server.handleClient();
}