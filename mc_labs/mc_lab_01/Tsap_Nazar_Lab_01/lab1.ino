#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>

#define BUTTON_PIN 13  
#define L1_PIN 0 //red led
#define L2_PIN 14 //blue led
#define L3_PIN 12 // green led
#define STEP_TIME 500 
#define HOLD_TIME 500

#define ON(pin)  ((pin) == 0 ? LOW : HIGH)
#define OFF(pin) ((pin) == 0 ? HIGH : LOW)

const char* ssid = "iPhone Nazar";
const char* password = "11111112";



AsyncWebServer server(80);


const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: Arial; text-align: center; margin: 0px auto; padding-top: 30px; }
    .button {
      padding: 15px 30px;
      font-size: 20px;
      color: white;
      background-color: #007bff; /* Синій колір */
      border: none;
      border-radius: 5px;
      cursor: pointer;
      user-select: none;
    }
    .button:active {
      background-color: #0056b3; /* Темніший синій при натисканні */
    }
  </style>
</head>
<body>
  <h1>ESP Pushbutton Web Server</h1>
  <button 
    class="button" 
    id="ledButton"
    onmousedown="sendRequest('press');"
    ontouchstart="sendRequest('press');" 
    onmouseup="sendRequest('release');"
    ontouchend="sendRequest('release');"
  >LED PUSHBUTTON</button>
  
  <script>
    function sendRequest(action) {
      var xhr = new XMLHttpRequest();
      xhr.open("GET", "/" + action, true);
      xhr.send();
    }
  </script>
</body>
</html>
)rawliteral";



typedef struct led_s{
  unsigned short pin;
  bool status;
  led_s* next;
} led_t;


led_t l1 = {L1_PIN, false, nullptr};
led_t l2 = {L2_PIN, false, nullptr};  
led_t l3 = {L3_PIN, false, nullptr}; 
unsigned short btnPressed = false;
unsigned short physicsBtnState = HIGH;
unsigned short webBtnState = HIGH;
unsigned short previousBtnState = HIGH;
uint32_t pressStartTime = 0;

bool firstStart = true;
uint32_t lastChangeTime = 0;
led_t* currentLed = nullptr;

led_t* firstPattern[] = {&l3, &l2, &l1, &l2, &l3};
uint8_t firstStep = 0;

void ledSetup() {
  l1.next = &l2;
  l2.next = &l3;
  l3.next = &l1;

  currentLed = &l3; 
  digitalWrite(l1.pin, HIGH); 
}


void pinSetup(){
  pinMode(BUTTON_PIN, INPUT);
  pinMode(l1.pin, OUTPUT);
  pinMode(l2.pin, OUTPUT);
  pinMode(l3.pin, OUTPUT);

}

void switchLed() {
  if (currentLed) {
    currentLed->status = OFF(currentLed->pin);
    digitalWrite(currentLed->pin, currentLed->status);
  }

  if (firstStart) {
    currentLed = firstPattern[firstStep++];
    firstStep %= 5;
  } else {
    currentLed = currentLed->next;
  }

  currentLed->status = ON(currentLed->pin);
  digitalWrite(currentLed->pin, currentLed->status);
}


void startEndpoint(AsyncWebServerRequest *request) {
  request->send_P(200, "text/html", index_html);
}

void pressEndpoint(AsyncWebServerRequest *request) {
  webBtnState = LOW;
  request->send(200, "text/plain", "ok");
}

void releaseEndpoint(AsyncWebServerRequest *request) {
  webBtnState = HIGH;
  request->send(200, "text/plain", "ok");
}




void wifiSetup(){
  Serial.print("Connecting to ");
  Serial.println(ssid);
  delay(1000);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED){
    delay(5000);
    Serial.print(".");
  }


  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, startEndpoint);

  server.on("/press", HTTP_GET, pressEndpoint);

  server.on("/release", HTTP_GET, releaseEndpoint);
  
  server.begin();
}


void setup() {
  Serial.begin(115200);
  pinSetup();
  ledSetup();
  wifiSetup();
}

void loop() {
  physicsBtnState = digitalRead(BUTTON_PIN);
  btnPressed = !(physicsBtnState && webBtnState);
  
  if (btnPressed && previousBtnState == HIGH) {
    previousBtnState = LOW;
    pressStartTime = millis();
  } 
  else if (btnPressed && previousBtnState == LOW && (millis() - pressStartTime >= HOLD_TIME)){
    if (millis() - lastChangeTime >= STEP_TIME) {
      switchLed();
      lastChangeTime = millis();
    }
  }

  if (!btnPressed && previousBtnState == LOW){
    previousBtnState = HIGH;
    currentLed->status = OFF(currentLed->pin);
    digitalWrite(currentLed->pin, currentLed->status);
    currentLed = &l3;
    firstStart = !firstStart;
    firstStep = 0;
  }
}
