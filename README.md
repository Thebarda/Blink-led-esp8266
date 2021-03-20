# Blink-led-esp8266
This a little project to learn ESP8266 wifi / Web Server micro-chip and LittleFS (flash filesystem).
The goal here is to configure the micro-chip to connect to a Wifi using a web form provided by the micro-chip and toggle status of a led in a UI and persist the wifi configuration

## Features
- It creates his own wifi when there is no wifi configuration
- Serves a HTML file to configure wifi configuration
- Receive ssid and password configuration
- Connect to the new wifi configuration
  -  When it fails, it reconnect to his previous configuration or re-create his Access Point
- Serve UI to toggle led status
- Click on a button that send an API request that invert the led status
- Display the current led status
