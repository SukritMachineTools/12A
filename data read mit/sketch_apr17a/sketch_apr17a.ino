#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "SUKRIT";     // Enter your WiFi SSID
const char* password = "Sukrit@2023"; // Enter your WiFi Password

const int ledPin = 23; // Pin connected to the LED

WebServer server(80);

void setup() {
  Serial.begin(115200);
  
  pinMode(ledPin, OUTPUT);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  Serial.println("Connected to WiFi");
  
  // Print the IP address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/plain", "Hello from ESP32!");
  });

  // Route to switch the LED on
  server.on("/on", HTTP_GET, []() {
    digitalWrite(ledPin, HIGH);
    server.send(200, "text/plain", "LED is on");
  });

  // Route to switch the LED off
  server.on("/off", HTTP_GET, []() {
    digitalWrite(ledPin, LOW);
    server.send(200, "text/plain", "LED is off");
  });

  // Start server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
