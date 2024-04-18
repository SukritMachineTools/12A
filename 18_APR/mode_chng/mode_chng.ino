#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "SUKRIT";
const char* password = "Sukrit@2023";

WebServer server(80);

IPAddress staticIP(192, 168, 1, 16); // Static IP address
IPAddress gateway(192, 168, 1, 1);    // Gateway IP address
IPAddress subnet(255, 255, 255, 0);   // Subnet mask

void setup() {
  Serial.begin(115200);
  pinMode(18, INPUT);
  pinMode(32, INPUT);
  pinMode(33, INPUT); // Add GPIO 33 as input
  pinMode(25, INPUT); // Add GPIO 25 as input
  pinMode(26, INPUT); // Add GPIO 26 as input

  // Set static IP configuration
  WiFi.config(staticIP, gateway, subnet);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());
  // Define web server routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/check_pin", HTTP_GET, handleCheckPin);
  server.on("/check_pin_32", HTTP_GET, handleCheckPin32); // Route for GPIO 32
  server.on("/check_pin_33", HTTP_GET, handleCheckPin33); // Route for GPIO 33
  server.on("/check_pin_25", HTTP_GET, handleCheckPin25); // Route for GPIO 25
  server.on("/check_pin_26", HTTP_GET, handleCheckPin26); // Route for GPIO 26

  server.begin();
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  server.send(200, "text/plain", "Hello from ESP32!");
}

void handleCheckPin() {
  int pinState = digitalRead(18);
  if (pinState == LOW) {
    server.send(200, "text/plain", "Mode1 Enabled");
  } else {
    server.send(200, "text/plain", "Mode1 Disabled");
  }
}

void handleCheckPin32() {
  int pinState = digitalRead(32);
  if (pinState == LOW) {
    server.send(200, "text/plain", "Mode2 Enabled");
  } else {
    server.send(200, "text/plain", "Mode2 Disabled");
  }
}

void handleCheckPin33() {
  int pinState = digitalRead(33);
  if (pinState == LOW) {
    server.send(200, "text/plain", "Mode3 Enabled");
  } else {
    server.send(200, "text/plain", "Mode3 Disabled");
  }
}

void handleCheckPin25() {
  int pinState = digitalRead(25);
  if (pinState == LOW) {
    server.send(200, "text/plain", "Mode4 Enabled");
  } else {
    server.send(200, "text/plain", "Mode4 Disabled");
  }
}

void handleCheckPin26() {
  int pinState = digitalRead(26);
  if (pinState == LOW) {
    server.send(200, "text/plain", "Mode5 Enabled");
  } else {
    server.send(200, "text/plain", "Mode5 Disabled");
  }
}
