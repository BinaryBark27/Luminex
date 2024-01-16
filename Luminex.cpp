#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "samsungA73";
const char* password = "harsh@1234";

const int ldrPin = A0;  
const int currentSensorPin = A1;  
const int redButtonPin = D1;  
const int webServerPort = 80;

ESP8266WebServer server(webServerPort);

bool pole1Fault = false;
bool pole2Fault = false;

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  pinMode(ldrPin, INPUT);
  pinMode(currentSensorPin, INPUT);
  pinMode(redButtonPin, OUTPUT);
  server.on("/", HTTP_GET, handleRoot);
  server.begin();
}

void loop() {
  server.handleClient();
  int ldrValue = analogRead(ldrPin);
  int currentSensorValue = analogRead(currentSensorPin);
  if (ldrValue < 100) {
    indicateFault();
  } else {
    digitalWrite(redButtonPin, LOW);
  }
  if (currentSensorValue > 800) {
    reportMainLineFault();
  }
}
void indicateFault() {
  digitalWrite(redButtonPin, HIGH);
}

void reportMainLineFault() {
    int currentFaultyPole = 1;
  String message = "Main line fault detected for Pole " + String(currentFaultyPole);
  sendFaultReport(message);
}

void sendFaultReport(String message) {
  String endpoint = "/report-fault";
  String url = "http://your-web-server-address" + endpoint + "?message=" + message;
  HTTPClient http;
  http.begin(url);
  int httpCode = http.GET();
  if (httpCode > 0) {
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}
void handleRoot() {
  String page = "<html><head><style>.button{display:inline-block;padding:10px 20px;font-size:16px;cursor:pointer;text-align:center;text-decoration:none;outline:none;color:#fff;background-color:#4CAF50;border:none;border-radius:5px;}.button.red{background-color:#FF0000;}#pole-info{margin-top:20px;}</style></head><body><h2>Street Pole Controls</h2>";


  page += "<button class='button" + (pole1Fault ? " red" : "") + "' onclick='toggleFault(1)'>Pole 1</button>";
  page += "<button class='button" + (pole2Fault ? " red" : "") + "' onclick='toggleFault(2)'>Pole 2</button>";

  page += "<div id='pole-info'>"; 
  page += "</div></body><script>function toggleFault(pole) { var xhttp = new XMLHttpRequest(); xhttp.onreadystatechange = function() { if (this.readyState == 4 && this.status == 200) { document.getElementById('pole-info').innerHTML = this.responseText; } }; xhttp.open('GET', '/toggle-fault?pole=' + pole, true); xhttp.send(); }</script></html>";

  server.send(200, "text/html", page);
}