#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);  // Address 0x27 for a 20x4 character display

const int interruptPin = 18;

bool displayRunning = true;

unsigned long previousMillis = 0;
const long interval = 1000;  // Update interval in milliseconds

int hours = 0;
int minutes = 0;
int seconds = 0;

void setup() {
  Wire.begin();
  lcd.init();
  lcd.backlight();

  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), toggleDisplay, CHANGE);

  // Initialize EEPROM
  EEPROM.begin(512);

  // Read time from EEPROM
  hours = EEPROM.read(0);
  minutes = EEPROM.read(1);
  seconds = EEPROM.read(2);
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // Update time every interval
    previousMillis = currentMillis;

    if (displayRunning) {
      // Print "Mode: Maintenance" and time if displayRunning is true
      lcd.setCursor(0, 0);
      lcd.print("  Mode:Maintenance");

      // Display time on LCD
      lcd.setCursor(0, 1); // Move cursor to the second line
      lcd.print("   Time: ");
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
    } else {
      // If displayRunning is false, print "Mode: Maintenance" and "Deactivated" on LCD
      lcd.setCursor(0, 0);
      lcd.print("  Mode:Maintenance");
      lcd.setCursor(0, 1);
      lcd.print("     Deactivated  ");
    }

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
  }
}

void toggleDisplay() {
  // Check the state of GPIO pin 18
  if (digitalRead(interruptPin) == LOW) {
    // If pin is LOW, resume time display
    displayRunning = true;
  } else {
    // If pin is HIGH, pause time display
    displayRunning = false;
  }
}

//When gpio18 != low ,it shows ("Mode: Maintenance")in line 1, in 2nd line it shows ("Deactivated")
//When gpio18 =low ,it shows ("Mode: Maintenance")in line 1, in 2nd line it shows ("Time:hh:mm:ss") 
//Note: The time doesnot stop and keep running in background even when gpio18 !=low
