#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);  // Address 0x27 for a 20x4 character display

const int interruptPin = 18;

bool displayRunning = true;

void setup() {
  Wire.begin();
  lcd.init();
  lcd.backlight();

  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), toggleDisplay, CHANGE);

  // Initialize EEPROM
  EEPROM.begin(512);

  // Set initial time if EEPROM is empty
  if (EEPROM.read(0) == 255 && EEPROM.read(1) == 255) {
    // Set initial time (HH:MM:SS)
    EEPROM.write(0, 12); // hours
    EEPROM.write(1, 0);  // minutes
    EEPROM.write(2, 0);  // seconds
    EEPROM.commit();
  }
}

void loop() {
  if (displayRunning) {
    // Print "Mode: Maintenance"
    lcd.setCursor(0, 0);
    lcd.print("Mode: Maintenance");

    // Read time from EEPROM
    int hours = EEPROM.read(0);
    int minutes = EEPROM.read(1);
    int seconds = EEPROM.read(2);

    // Display time on LCD
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
  }
  delay(1000); // Update every second
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

//This code stops the currnet time as soon as gpio18 is no longer low and prints Mode: Maintenance ,even when gpio18 is no longer low 
