#include <WiFi.h>
#include <WebServer.h>
#include <EEPROM.h>
#include <Wire.h> // Include the Wire library for I2C
#include <LiquidCrystal_I2C.h> // Include the LiquidCrystal_I2C library

const int EEPROM_SIZE = 128; // Adjust size as needed
const int CONFIG_START_ADDR = 0; // Start address in EEPROM for configuration data
const int LCD_COLS = 20;
const int LCD_ROWS = 4;
const int LCD_ADDR = 0x27; // I2C address of the LCD
const int GPIO18 = 18; // GPIO pin for mode detection

const char *defaultSSID = "ESP32Hotspot";
const char *defaultPassword = "password123";
IPAddress defaultLocalIP(192, 168, 4, 1); // Default static IP for the ESP32 hotspot

char ssid[32];
char password[64];
IPAddress local_IP;

WebServer server(80);

LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS); // Initialize the LCD object

void setup() {
  Serial.begin(115200);
  pinMode(GPIO18, INPUT_PULLUP); // Set GPIO18 as input with internal pull-up resistor

  lcd.init(); // Initialize the LCD
  lcd.backlight(); // Turn on the backlight

  // Initialize EEPROM
  EEPROM.begin(EEPROM_SIZE);
  // Check if EEPROM is empty
  bool isEmpty = true;
  for (int i = 0; i < EEPROM_SIZE; i++) {
    if (EEPROM.read(i) != 255) { // 255 is the default value of EEPROM
      isEmpty = false;
      break;
    }
  }
  EEPROM.end();

  // If EEPROM is empty, set default values
  if (isEmpty) {
    strcpy(ssid, defaultSSID);
    strcpy(password, defaultPassword);
    local_IP = defaultLocalIP;

    // Write default values to EEPROM
    writeConfigToEEPROM();
  } else {
    // Read configuration from EEPROM
    readConfigFromEEPROM();
  }

  // Connect to the hotspot
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_IP, local_IP, IPAddress(255, 255, 255, 0));

  delay(100);

  server.on("/", HTTP_GET, handleRoot);
  server.on("/config", HTTP_GET, handleConfig);

  server.begin();
}

void loop() {
  server.handleClient();
  maintenance(); // Check for maintenance mode
}

void handleRoot() {
  server.send(200, "text/plain", "Welcome to ESP32 Hotspot Configuration!");
}

void handleConfig() {
  wifi_cred();
  
  // Send response with updated configuration
  String response = "SSID: ";
  response += String(ssid) + "\n";
  response += "Password: " + String(password) + "\n";
  response += "IP Address: " + local_IP.toString();
  server.send(200, "text/plain", response);
}

void wifi_cred() {
  if (server.hasArg("ssid")) {
    String newSSID = server.arg("ssid");
    newSSID.toCharArray(ssid, sizeof(ssid));
    Serial.print("Updated SSID: ");
    Serial.println(newSSID);
  }

  if (server.hasArg("password")) {
    String newPassword = server.arg("password");
    newPassword.toCharArray(password, sizeof(password));
    Serial.print("Updated Password: ");
    Serial.println(newPassword);
  }

  if (server.hasArg("ip")) {
    String newIP = server.arg("ip");
    IPAddress newIPAddress;
    if (newIPAddress.fromString(newIP)) {
      local_IP = newIPAddress;
      Serial.print("Updated IP Address: ");
      Serial.println(newIP);
    }
  }

  // Store configuration in EEPROM temporarily
  writeConfigToEEPROM();

  // Reconnect to the hotspot with new parameters
  WiFi.softAPdisconnect(true);
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_IP, local_IP, IPAddress(255, 255, 255, 0));
}

void maintenance() {
  if (digitalRead(GPIO18) == LOW) {
    // Set the LCD cursor to the first column of the second row
    lcd.setCursor(1,0);
    lcd.print("Mode: Maintenance");
  } else {
    // Clear the LCD screen
    lcd.clear();
  }
}

void readConfigFromEEPROM() {
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.get(CONFIG_START_ADDR, ssid);
  EEPROM.get(CONFIG_START_ADDR + sizeof(ssid), password);
  EEPROM.get(CONFIG_START_ADDR + sizeof(ssid) + sizeof(password), local_IP);
  EEPROM.end();

  // Check if configuration is valid
  if (strcmp(ssid, "") == 0) {
    strcpy(ssid, defaultSSID);
  }
  if (strcmp(password, "") == 0) {
    strcpy(password, defaultPassword);
  }
  if (local_IP == IPAddress(0, 0, 0, 0)) {
    local_IP = defaultLocalIP;
  }
}

void writeConfigToEEPROM() {
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.put(CONFIG_START_ADDR, ssid);
  EEPROM.put(CONFIG_START_ADDR + sizeof(ssid), password);
  EEPROM.put(CONFIG_START_ADDR + sizeof(ssid) + sizeof(password), local_IP);
  EEPROM.commit();
  EEPROM.end();

  // Wait for 2 seconds to allow writing to EEPROM
  delay(2000);
}
