#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

// Import required libraries
#include <Arduino.h>
#include "ESP8266WiFi.h"
#include "ESPAsyncWebServer.h"
#include "btn_handle.h"

#define ESP_WIFI_MODE   2  //WIFI_STA // WIFI_AP //WIFI_AP_STA
#define WIFI_PORT 80

// Replace with your network credentials
const char* ssid = "WEMOS_ESP8266";
const char* password = "PASSWORD";

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: Arial; text-align: center; margin:0px auto; padding-top: 30px;}
      .button {
        padding: 10px 20px;
        font-size: 24px;
        text-align: center;
        outline: none;
        color: #fff;
        background-color: #2f4468;
        border: none;
        border-radius: 5px;
        box-shadow: 0 6px #999;
        cursor: pointer;
        -webkit-touch-callout: none;
        -webkit-user-select: none;
        -khtml-user-select: none;
        -moz-user-select: none;
        -ms-user-select: none;
        user-select: none;
        -webkit-tap-highlight-color: rgba(0,0,0,0);
      }  
      .button:hover {background-color: #1f2e45}
      .button:active {
        background-color: #1f2e45;
        box-shadow: 0 4px #666;
        transform: translateY(2px);
      }
  </style>
</head>
<body>
  <h1>ESP Pushbutton Web Server</h1>
  <button class="button" onmousedown="toggleCheckbox('on');" ontouchstart="toggleCheckbox('on');" onmouseup="toggleCheckbox('off');" ontouchend="toggleCheckbox('off');">LED PUSHBUTTON</button>
    <script>
      function toggleCheckbox(x) {
        var xhr = new XMLHttpRequest();
        xhr.open("GET", "/" + x, true);
        xhr.send();
      }
    </script>
  </body>
</html>
)rawliteral";


void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

// Create AsyncWebServer object on port 80
AsyncWebServer server(WIFI_PORT);

void initWiFi()
{
 
#if(ESP_WIFI_MODE == 1)
  WiFi.mode(WIFI_STA);
  // Connect to Wi-Fi network with SSID and password
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println();
  Serial.print("ESP IP Address: http://");
  Serial.println(WiFi.localIP());

  Serial.print("RRSI: ");
  Serial.println(WiFi.RSSI());
#else// (ESP_WIFI_MODE == WIFI_AP)
  WiFi.mode(WIFI_AP);
  Serial.println("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  //WiFi.softAP(ssid, password);
  WiFi.softAP(ssid, NULL);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
#endif

  // Send web page to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  // Receive an HTTP GET request
  server.on("/on", HTTP_GET, [] (AsyncWebServerRequest *request) {
    btnChangedFlag = true;
    btnPressed = true;
    request->send(200, "text/plain", "ok");
  });

  // Receive an HTTP GET request
  server.on("/off", HTTP_GET, [] (AsyncWebServerRequest *request) {
    btnChangedFlag = true;
    btnReleased = true;
    request->send(200, "text/plain", "ok");
  });
  
  server.onNotFound(notFound);
  server.begin();
}

#endif //* WIFI_CONFIG_H