#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

// Import required libraries
#ifdef ESP32
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#else
#include <Arduino.h>
#include "ESP8266WiFi.h"
// #include <Hash.h>
// #include <ESPAsyncTCP.h>
#include "ESPAsyncWebServer.h"
#include "btn_handle.h"
#endif
// #include <OneWire.h>
#define ESP_WIFI_MODE 2 // WIFI_STA // WIFI_AP //WIFI_AP_STA
#define WIFI_PORT 80

// Replace with your network credentials
const char *ssid = "WEMOS_ESP8266";
const char *password = "PASSWORD";

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: Arial; text-align: center; margin:0px auto; padding-top: 30px;}
    
    .topnav {
      overflow: hidden;
      background-color: #143642;
    }

    .content {
      padding: 30px;
      max-width: 600px;
      margin: 0 auto;
    }


    .card {
    background-color: #F8F7F9;;
    box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);
    padding-top:10px;
    padding-bottom:20px;
    }
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

      ".greenButton {background-color: green; font-size: 64px;}"
      ".greenButton:hover {background-color: darkgreen; font-size: 64px;}"
      
      ".redButton {background-color: red; font-size: 64px;}"
      ".redButton:hover {background-color: darkred; font-size: 64px;}"
      
      ".blueBox {"
                  "background-color: blue;"
                          "color: white;"
                          "width: 350px;" 
                          "padding: 20px;"
                          "text-align: center;"
                          "font-size: 50px;"
                          "font-family: arial;"
                          "margin: 20px 200px;"
                        "}"
  </style>
</head>

<body>
  <div class="topnav">
    <h1>ESP Pushbutton Web Server</h1>
  </div>
  <div class="content">
    <div class="card">
      <h2>Output - state/h2>
      <p class="state">state: <span id="state">%STATE%</span></p>
      <p><button id="button" class="button">Toggle</button></p>
    </div>
  </div>
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

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

// Create AsyncWebServer object on port 80
AsyncWebServer server(WIFI_PORT);

void initWiFi()
{

#if (ESP_WIFI_MODE == 1)
  WiFi.mode(WIFI_STA);
  // Connect to Wi-Fi network with SSID and password
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println();
  Serial.print("ESP IP Address: http://");
  Serial.println(WiFi.localIP());

  Serial.print("RRSI: ");
  Serial.println(WiFi.RSSI());
#else // (ESP_WIFI_MODE == WIFI_AP)
  WiFi.mode(WIFI_AP);
  Serial.println("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  // WiFi.softAP(ssid, password);
  WiFi.softAP(ssid, NULL);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
#endif

  // Send web page to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", index_html); });

  // Receive an HTTP GET request
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    btnChangedFlag = true;
    btnPressed = true;
    request->send(200, "text/plain", "ok"); });

  // Receive an HTTP GET request
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    btnChangedFlag = true;
    btnReleased = true;
    request->send(200, "text/plain", "ok"); });

  server.onNotFound(notFound);
  server.begin();
}

#endif //* WIFI_CONFIG_H