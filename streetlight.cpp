#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

const char* ssid = "Hogwarts";
const char* password = "harry potter";

ESP8266WebServer server(80);

const int SENSOR_PIN_5 = D5;
const int SENSOR_PIN_6 = D6;
const int CURRENT_SENSOR_PIN = A0;

void setup() {
  Serial.begin(115200);
  
  pinMode(SENSOR_PIN_5, INPUT);
  pinMode(SENSOR_PIN_6, INPUT);
  
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  int sensorValue5 = digitalRead(SENSOR_PIN_5);
  int sensorValue6 = digitalRead(SENSOR_PIN_6);
  float currentReading = analogRead(CURRENT_SENSOR_PIN) * (5.0 / 1023.0); // Convert ADC value to voltage
  String status5 = (sensorValue5 == HIGH) ? "Fault" : "Good";
  String status6 = (sensorValue6 == HIGH) ? "Fault" : "Good";

  String html = "<html><head><title>Readings</title></head><body>";
  html += "<h1>Pole Readings</h1>";
  html += "<table border='1'>";
  html += "<tr><th>Pole No.</th><th>Status</th></tr>";
  html += "<tr><td>D5</td><td>" + status5 + "</td></tr>";
  html += "<tr><td>D6</td><td>" + status6 + "</td></tr>";
  html += "</table>";
  html += "<h2>Current Sensor Reading</h2>";
  html += "<p>The current sensor reading is: " + String(currentReading, 2) + " volts</p>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}