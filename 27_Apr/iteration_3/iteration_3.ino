#include <WiFi.h>
#include <WebServer.h>
#include <EEPROM.h>
#include <Wire.h> // Include the Wire library for I2C
#include <LiquidCrystal_I2C.h> // Include the LiquidCrystal_I2C library

// Constants
const int EEPROM_SIZE = 128; // Adjust size as needed
const int CONFIG_START_ADDR = 0; // Start address in EEPROM for configuration data
const int LCD_COLS = 20;
const int LCD_ROWS = 4;
const int LCD_ADDR = 0x27; // I2C address of the LCD
const int GPIO18 = 18; // GPIO pin for mode detection

// Default network settings
const char *defaultSSID = "EP32";
const char *defaultPassword = "12345678";
IPAddress defaultLocalIP(192, 168, 4, 1); // Default static IP for the ESP32 hotspot

// Variables
char ssid[32];
char password[64];
IPAddress local_IP;

WebServer server(80);

LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS); // Initialize the LCD object

bool displayRunning = false;

// Function prototypes
void handleRoot();
void handleConfig();
void wifi_cred();
void maintenance();
void readConfigFromEEPROM();
void writeConfigToEEPROM();
void toggleDisplay(); // Declaration for toggleDisplay function

void setup() {
  Wire.begin();
  Serial.begin(115200);
  pinMode(GPIO18, INPUT_PULLUP); // Set GPIO18 as input with internal pull-up resistor
  attachInterrupt(digitalPinToInterrupt(GPIO18), toggleDisplay, CHANGE);
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

  // Define server routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/config", HTTP_GET, handleConfig);
  server.begin();

  // Set initial time (HH:MM:SS) to 00:00:00
  EEPROM.begin(512);
  EEPROM.write(0, 0); // hours
  EEPROM.write(1, 0); // minutes
  EEPROM.write(2, 0); // seconds
  EEPROM.commit();
}

void loop() {
  server.handleClient(); // Handle incoming HTTP requests
  maintenance(); // Check for maintenance mode
}

// Handlers for HTTP requests
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

// Update WiFi credentials based on HTTP request parameters
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

// Update maintenance mode display
void maintenance() {
  if (displayRunning) {
    // Read time from EEPROM
    int hours = EEPROM.read(0);
    int minutes = EEPROM.read(1);
    int seconds = EEPROM.read(2);

    // Display time on LCD
    lcd.setCursor(0, 0);
    lcd.print("Mode: Maintenance");
    lcd.setCursor(0, 1); // Move cursor to the second line
    lcd.print("Time: ");
    lcd.print(hours);
    lcd.print(":");
    if (minutes < 10) {
      lcd.print("0");
    }
    lcd.print(minutes);
    lcd.print(":");
    if (seconds < 10) {
      lcd.print("0");
    }
    lcd.print(seconds);

    // Increment seconds
    seconds++;
    if (seconds >= 60) {
      seconds = 0;
      minutes++;
      if (minutes >= 60) {
        minutes = 0;
        hours++;
        if (hours >= 24) {
          hours = 0;
        }
      }
    }

    // Update time in EEPROM
    EEPROM.write(0, hours);
    EEPROM.write(1, minutes);
    EEPROM.write(2, seconds);
    EEPROM.commit();
  } else {
    // If display is not running, clear the LCD
    lcd.clear();
  }
  delay(1000); // Update every second
}

// Read WiFi configuration from EEPROM
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

// Write WiFi configuration to EEPROM
void writeConfigToEEPROM() {
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.put(CONFIG_START_ADDR, ssid);
  EEPROM.put(CONFIG_START_ADDR + sizeof(ssid), password);
  EEPROM.put(CONFIG_START_ADDR + sizeof(ssid) + sizeof(password), local_IP);
  EEPROM.commit();
  EEPROM.end();
}

// Toggle maintenance display mode
void toggleDisplay() {
  // Check the state of GPIO pin 18
  if (digitalRead(GPIO18) == LOW) {
    // If pin is LOW, start displaying time and reset time to 00:00:00
    EEPROM.write(0, 0); // hours
    EEPROM.write(1, 0); // minutes
    EEPROM.write(2, 0); // seconds
    EEPROM.commit();
    displayRunning = true;
  } else {
    // If pin is HIGH, stop displaying time
    displayRunning = false;
  }
}
