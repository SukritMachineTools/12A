#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "SUKRIT";
const char* password = "Sukrit@2023";
IPAddress staticIP(192, 168, 1, 103); // Static IP address
IPAddress gateway(192, 168, 1, 1);     // Gateway IP address
IPAddress subnet(255, 255, 255, 0);    // Subnet mask

WebServer server(80);

const int ledPin = 19; // GPIO pin 19 for the LED
bool ledState = LOW;

void setup() {
  pinMode(ledPin, OUTPUT);
  
  // Initialize Serial port
  Serial.begin(115200);

  // Connect to Wi-Fi with static IP
  WiFi.config(staticIP, gateway, subnet);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());
  
  // Set up routes
  server.on("/", handleRoot);
  server.on("/led-on", handleLedOn);
  server.on("/led-off", handleLedOff);
  
  // Start server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  String ledStatus = (digitalRead(ledPin) == HIGH) ? "ON" : "OFF";
  server.send(200, "text/plain", ledStatus);
}

void handleLedOn() {
  digitalWrite(ledPin, HIGH);
  server.send(200, "text/plain", "LED turned ON");
}

void handleLedOff() {
  digitalWrite(ledPin, LOW);
  server.send(200, "text/plain", "LED turned OFF");
}
