#include <WiFi.h>
#include <ESPAsyncWebSrv.h>

const char* ssid = "SUKRIT";
const char* password = "Sukrit@2023";
IPAddress staticIP(192, 168, 1, 100); // Change to desired static IP address
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  WiFi.config(staticIP, gateway, subnet); // Set static IP configuration
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  // Print ESP IP address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Hello from ESP32!");
  });

  // Start server
  server.begin();
}

void loop() {
  // Your code here
}
