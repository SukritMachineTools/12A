#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h> // Include the Wire library for I2C communication
#include <LiquidCrystal_I2C.h> // Include the LiquidCrystal_I2C library for I2C LCD

const char* ssid = "SUKRIT";
const char* password = "Sukrit@2023";

WebServer server(80);

IPAddress staticIP(192, 168, 1, 16); // Static IP address
IPAddress gateway(192, 168, 1, 1);    // Gateway IP address
IPAddress subnet(255, 255, 255, 0);   // Subnet mask

// Initialize the LCD with its I2C address and dimensions (20x4)
LiquidCrystal_I2C lcd(0x27, 20, 4);

int activeMode = 0; // Variable to track the active mode
unsigned long lastRefreshTime = 0; // Variable to store the last refresh time

void setup() {
  Serial.begin(115200);
  pinMode(18, INPUT);
  pinMode(32, INPUT);
  pinMode(33, INPUT); // Add GPIO 33 as input
  pinMode(25, INPUT); // Add GPIO 25 as input
  pinMode(26, INPUT); // Add GPIO 26 as input

  // Initialize the LCD
  lcd.init();
  lcd.backlight(); // Turn on the backlight

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
  updateLCDBasedOnPinState(); // Update LCD based on pin states

  // Check if 15 seconds have elapsed since the last refresh
  if (millis() - lastRefreshTime >= 120000) {
    lastRefreshTime = millis();
    updateLCDBasedOnPinState(); // Refresh LCD
  }
  delay(6000);
}

void updateLCDBasedOnPinState() {
  // Check each pin and update LCD based on its state
  if (digitalRead(18) == LOW) {
    activeMode = 1;
    updateLCD("Mode: Maintenance");
  } else if (digitalRead(32) == LOW) {
    activeMode = 2;
    updateLCD("Mode: Operation");
  } else if (digitalRead(33) == LOW) {
    activeMode = 3;
    updateLCD("Mode3 Enabled");
  } else if (digitalRead(25) == LOW) {
    activeMode = 4;
    updateLCD("Mode: Service");
  } else if (digitalRead(26) == LOW) {
    activeMode = 5;
    updateLCD("Mode5 Enabled");
  } else {
    activeMode = 0;
    lcd.clear();
  }
}

void updateLCD(const char* message) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(message);
}

void handleRoot() {
  server.send(200, "text/plain", "Hello from ESP32!");
}

void handleCheckPin() {
  server.send(200, "text/plain", (activeMode == 1) ? "Maintenance Mode Enabled" : "Maintenance Mode Disabled");
}

void handleCheckPin32() {
  server.send(200, "text/plain", (activeMode == 2) ? "Operation Mode Enabled" : "Operation Mode Disabled");
}

void handleCheckPin33() {
  server.send(200, "text/plain", (activeMode == 3) ? "Mode3 Enabled" : "Mode3 Disabled");
}

void handleCheckPin25() {
  server.send(200, "text/plain", (activeMode == 4) ? "Service Mode Enabled" : "Service Mode Disabled");
}

void handleCheckPin26() {
  server.send(200, "text/plain", (activeMode == 5) ? "Mode5 Enabled" : "Mode5 Disabled");
}
