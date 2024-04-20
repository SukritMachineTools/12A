#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include <WiFi.h>
#include <WebServer.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);  // Address 0x27 for a 20x4 character display

const int interruptPin = 18;
const int optoPinm3 = 19;  // Pin for setting mode indication

bool counterRunning = true;
bool settingMode = false;
unsigned long settingModeStartTime = 0;

unsigned long previousMillis = 0;
const long interval = 1000;  // Update interval in milliseconds

int hours = 0;
int minutes = 0;
int seconds = 0;

int counterHours = 0;
int counterMinutes = 0;
int counterSeconds = 0;

WebServer server(80);

void setup() {
  Wire.begin();
  lcd.init();
  lcd.backlight();
  lcd.clear();

  pinMode(interruptPin, INPUT_PULLUP);
  pinMode(optoPinm3, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), toggleDisplay, CHANGE);

  // Initialize EEPROM
  EEPROM.begin(512);

  // Read time from EEPROM
  hours = EEPROM.read(0);
  minutes = EEPROM.read(1);
  seconds = EEPROM.read(2);

  // Connect to WiFi
  WiFi.mode(WIFI_AP);
  WiFi.softAP("ESP32AP", "password"); // Change SSID and password as needed

  // Start server
  server.on("/admin", HTTP_POST, handleAdmin);
  server.begin();
}

void loop() {
  server.handleClient();  // Handle HTTP requests

  unsigned long currentMillis = millis();

  if (digitalRead(optoPinm3) == LOW) {
    // Setting Mode
    settingMode = true;
    if (settingModeStartTime == 0) {
      settingModeStartTime = currentMillis;
    }
    updateLCDSettingMode(currentMillis - settingModeStartTime);
  } else {
    settingMode = false;
    settingModeStartTime = 0;
    updateLCD();
  }

  if (!settingMode) {
    if (currentMillis - previousMillis >= interval) {
      // Update time every interval
      previousMillis = currentMillis;

      // Always increment time
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

      // Increment counter only if counter is running
      if (counterRunning) {
        counterSeconds++;
        if (counterSeconds >= 60) {
          counterSeconds = 0;
          counterMinutes++;
          if (counterMinutes >= 60) {
            counterMinutes = 0;
            counterHours++;
            if (counterHours >= 24) {
              counterHours = 0;
            }
          }
        }
      }

      // Update time in EEPROM
      EEPROM.write(0, hours);
      EEPROM.write(1, minutes);
      EEPROM.write(2, seconds);
      EEPROM.commit();
    }
  }

  delay(100); // Small delay for stability
}

void updateLCD() {
  lcd.clear();
  // Display time on LCD
  lcd.setCursor(0, 0);
  lcd.print("   Mode: Normal    ");
  lcd.setCursor(0, 2);
  lcd.print("   Time:");
  printTime(hours, minutes, seconds);

  // Display counter on LCD
  lcd.setCursor(0, 1);
  lcd.print("  Counter: ");
  printTime(counterHours, counterMinutes, counterSeconds);
}

void updateLCDSettingMode(unsigned long elapsedTime) {
  lcd.clear();
  // Display setting mode and elapsed time on LCD
  lcd.setCursor(0, 0);
  lcd.print("  Mode: Setting    ");
  lcd.setCursor(0, 2);
  lcd.print("  Elapsed: ");
  lcd.print(elapsedTime / 1000);  // Convert milliseconds to seconds
  lcd.print("s");
}

void printTime(int hours, int minutes, int seconds) {
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
}

void toggleDisplay() {
  // Check the state of GPIO pin 18
  if (digitalRead(interruptPin) == LOW) {
    // If pin is LOW, resume counter
    counterRunning = true;
  } else {
    // If pin is HIGH, pause counter and reset counter variables
    counterRunning = false;
    counterHours = 0;
    counterMinutes = 0;
    counterSeconds = 0;
  }
}

void handleAdmin() {
  // Handle HTTP POST request to update network settings
  if (server.args() > 0) {
    String ssid = server.arg("ssid");
    String password = server.arg("password");
    String ip = server.arg("ip");
    String gateway = server.arg("gateway");
    String subnet = server.arg("subnet");

    // Update EEPROM with new network settings
    EEPROM.writeString(10, ssid);
    EEPROM.writeString(50, password);
    EEPROM.writeString(100, ip);
    EEPROM.writeString(150, gateway);
    EEPROM.writeString(200, subnet);
    EEPROM.commit();

    // Restart Wi-Fi access point with updated settings
    WiFi.softAPdisconnect(true); // Disconnect existing clients and stop the AP
    delay(1000);
    WiFi.softAP(ssid.c_str(), password.c_str(), 1, 0); // Start AP with new settings
    server.send(200, "text/plain", "Settings updated. Restarting access point...");
    ESP.restart(); // Restart the device
  } else {
    server.send(400, "text/plain", "Invalid request");
  }
}
