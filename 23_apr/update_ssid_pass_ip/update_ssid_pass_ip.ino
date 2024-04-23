#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebSrv.h>

const char* ssid = "SUKRIT";
const char* password = "Sukrit@2023";

IPAddress ip(192, 168, 1, 100); // Initial IP address
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "ESP32 WiFi Configuration");
  });

  server.on("/update_wifi", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("ssid") && request->hasParam("password")) {
        String ssid = request->getParam("ssid")->value();
        String password = request->getParam("password")->value();

        // Update WiFi credentials
        WiFi.begin(ssid.c_str(), password.c_str());
        request->send(200, "text/plain", "WiFi credentials updated. Rebooting...");
        delay(5000);
        ESP.restart();
    } else {
        request->send(400, "text/plain", "Missing parameters");
    }
  });

  server.on("/update_ip", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("ip") && request->hasParam("gateway") && request->hasParam("subnet")) {
        String ip = request->getParam("ip")->value();
        String gateway = request->getParam("gateway")->value();
        String subnet = request->getParam("subnet")->value();

        // Update IP configuration
        IPAddress newIP, newGateway, newSubnet;
        newIP.fromString(ip);
        newGateway.fromString(gateway);
        newSubnet.fromString(subnet);
        
        // Set static IP address
        WiFi.config(newIP, newGateway, newSubnet);
        
        request->send(200, "text/plain", "IP address updated. Rebooting...");
        delay(5000);
        ESP.restart();
    } else {
        request->send(400, "text/plain", "Missing parameters");
    }
  });

  server.begin();
}

void loop() {
}
