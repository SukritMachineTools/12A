#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "SUKRIT";
const char* password = "Sukrit@2023";
const IPAddress ip(192, 168, 1, 16);
const IPAddress gateway(192, 168, 1, 1);
const IPAddress subnet(255, 255, 255, 0);

WebServer server(80);

void handleUpdate() {
  server.send(200, "text/plain", "hello");
}

void setup() {
  Serial.begin(115200);

  // Connect to WiFi
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected");

  // Set up routes
  server.on("/update", HTTP_GET, handleUpdate);

  server.begin();
  Serial.println("Web server started");
}

void loop() {
  server.handleClient();
}
