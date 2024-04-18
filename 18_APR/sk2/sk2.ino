 #include <WiFi.h>
#include <WebServer.h>

const char* ssid = "SUKRIT";
const char* password = "Sukrit@2023";

WebServer server(80);

IPAddress staticIP(192, 168, 1, 100); // Static IP address
IPAddress gateway(192, 168, 1, 1);    // Gateway IP address
IPAddress subnet(255, 255, 255, 0);   // Subnet mask

void setup() {
  Serial.begin(115200);
  pinMode(18, INPUT);

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
    server.send(200, "text/plain", "Button Pressed");
  } else {
    server.send(200, "text/plain", "No button Pressed");
  }
}
