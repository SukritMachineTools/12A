#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebSrv.h>
#include <WiFi.h>

const char* ssid = "SUKRIT";     // Change it to your WiFi SSID
const char* password = "Sukrit@2023"; // Change it to your WiFi password

LiquidCrystal_I2C lcd(0x27, 20, 4); // Address 0x27, 20 columns, 4 rows

AsyncWebServer server(80);

IPAddress ip(192, 168, 1, 160); // Static IP address
IPAddress gateway(192, 168, 1, 1); // Gateway IP address
IPAddress subnet(255, 255, 255, 0); // Subnet mask

void setup() {
  Serial.begin(115200);

  pinMode(18, INPUT_PULLUP); // Assuming the default state is HIGH

  lcd.init();
  lcd.backlight();

  WiFi.config(ip, gateway, subnet); // Set static IP, gateway, and subnet mask
  WiFi.begin(ssid, password); // Connect to WiFi network

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  server.begin();
}

void loop() {
  if (digitalRead(18) == LOW) {
    maintenance();
  } else {
    lcd.clear();
    nothing();
  }
  delay(1000); // You might want to adjust this delay depending on your application
}


void maintenance() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Mode : Maintenance");

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", "Hello from ESP32!");
  });
}
void nothing() {
  lcd.clear();
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", "");
  });
}
