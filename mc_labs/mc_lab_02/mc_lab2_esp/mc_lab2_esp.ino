
// Import required libraries

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#define ESP_WIFI_MODE 2 // WIFI_STA // WIFI_AP //WIFI_AP_STA

// Replace with your network credentials
const char *ssid = "WEMOS_ESP8266";
const char *password = "IoT";

// Assign GPIO to the relay and button
const int redLedPin = D5;   // Replace with the desired GPIO pin
const int orangeLedPin = D6; // Replace with the desired GPIO pin
const int greenLedPin = D7; // Replace with the desired GPIO pin

const int btnGPIO = D8; // Replace with the desired GPIO pin

bool btnChangedFlag = false;
bool btnPressed = false;
bool btnReleased = false;
bool webChange = false;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body {
      font-family: Arial;
      text-align: center;
      margin: 0px auto;
      padding-top: 30px;
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
      -webkit-tap-highlight-color: rgba(0, 0, 0, 0);
    }

    .button:hover {
      background-color: #1f2e45
    }

    .button:active {
      background-color: #1f2e45;
      box-shadow: 0 4px #666;
      transform: translateY(2px);
    }

    .square {
      width: 100px;
      height: 100px;
      margin: 20px;
      display: inline-block;
      border: 5px solid black;
    }

    .container {
      text-align: center;
    }

    #square1 {
      background-color: white;
    }

  </style>
</head>
<body>
  <h1>ESP Pushbutton Web Server</h1>
  <button class="button" onmousedown="algorighm1('on');" ontouchstart="algorighm1('on');" onmouseup="algorighm1('off');" ontouchend="algorighm1('off');">Algorithm 1</button>
  <button class="button" onmousedown="algorighm2('on');" ontouchstart="algorighm2('on');" onmouseup="algorighm2('off');" ontouchend="algorighm2('off');">Algorithm 2</button>

  <div class="container">
    <div id="square1" class="square"></div>
    <div id="square2" class="square"></div>
    <div id="square3" class="square"></div>
  </div>

  <script>
    var square1 = document.getElementById('square1');
    var square2 = document.getElementById('square2');
    var square3 = document.getElementById('square3');

    function toggleCheckboxRed(element) {
        var xhr = new XMLHttpRequest();
        xhr.open("GET", "/status_red_led", true);
        xhr.onreadystatechange = function() {
        if (xhr.readyState === 4 && xhr.status === 200){
            var status_red_led = xhr.responseText;
            console.log("LedRed status:", status_red_led);

             if (status_red_led === '1') {
                square1.style.backgroundColor = 'red';
              } else {
                square1.style.backgroundColor = 'white';
              }
            }
          };
        xhr.send();
     } 

     function toggleCheckboxOrange(element) {
        var xhr = new XMLHttpRequest();
        xhr.open("GET", "/status_orange_led", true);
        xhr.onreadystatechange = function() {
        if (xhr.readyState === 4 && xhr.status === 200){
            var status_orange_led = xhr.responseText;
            console.log("LedRed status:", status_orange_led);

             if (status_orange_led === '1') {
                square2.style.backgroundColor = 'orange';
              } else {
                square2.style.backgroundColor = 'white';
              }
            }
          };
        xhr.send();
     } 

    setInterval(toggleCheckboxRed, 250);
    setInterval(toggleCheckboxOrange, 250);

    function algorighm1(x) {
      var xhr = new XMLHttpRequest();
      xhr.open("GET", "/" + x, true);
      xhr.send();
    }

    function algorighm2(x) {
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
    webChange = true;
    request->send(200, "text/plain", "ok"); });

  server.on("/status_red_led", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    request->send(200, "text/plain", String(digitalRead(redLedPin)).c_str());
    });

  server.on("/status_orange_led", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    request->send(200, "text/plain", String(digitalRead(orangeLedPin)).c_str());
     });

  // Receive an HTTP GET request
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    btnChangedFlag = true;
    btnReleased = true;
    webChange = true;
    request->send(200, "text/plain", "ok"); });

  server.onNotFound(notFound);
  server.begin();
}

IRAM_ATTR void btnIsrHigh()
{
  btnChangedFlag = true;
#if (BUTTON_PULLUP == true)
  btnReleased = true;
#else
  btnPressed = true;
#endif
}

IRAM_ATTR void btnIsrLow()
{
  btnChangedFlag = true;
#if (BUTTON_PULLUP == true)
  btnPressed = true;
#else
  btnReleased = true;
#endif
}

void setup()
{
  Serial.begin(115200);

  pinMode(btnGPIO, INPUT);
  // attachInterrupt(digitalPinToInterrupt(btnGPIO), btnIsrHigh, CHANGE);
 // attachInterrupt(digitalPinToInterrupt(btnGPIO), btnIsrHigh, RISING);
  attachInterrupt(digitalPinToInterrupt(btnGPIO), btnIsrLow, FALLING);

  pinMode(redLedPin, OUTPUT);
  pinMode(orangeLedPin, OUTPUT);
  digitalWrite(redLedPin, LOW);
  digitalWrite(orangeLedPin, LOW);

  initWiFi();

  digitalWrite(orangeLedPin, HIGH);
  digitalWrite(redLedPin, HIGH);
}

void loop()
{
  if (btnChangedFlag)
  {
    if (webChange)
    {
      if (btnPressed)
      {
        digitalWrite(orangeLedPin, LOW);
        digitalWrite(redLedPin, HIGH);
        btnPressed = false;
      }
      if (btnReleased)
      {
        digitalWrite(orangeLedPin, HIGH);
        digitalWrite(redLedPin, LOW);
        btnReleased = false;
      }
      webChange = false;
    }
    else
    {

      if (btnPressed)
      {
        digitalWrite(redLedPin, LOW);
        btnPressed = false;
      }
      if (btnReleased)
      {
        digitalWrite(redLedPin, HIGH);
        btnReleased = false;
      }
    }

    btnChangedFlag = false;
  }
}