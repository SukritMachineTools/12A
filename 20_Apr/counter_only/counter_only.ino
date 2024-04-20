#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);  // Address 0x27 for a 20x4 character display

const int interruptPin = 18;

bool timerRunning = false;
unsigned long startTime = 0;
unsigned long elapsedTime = 0;

void setup() {
  Wire.begin();
  lcd.init();
  lcd.backlight();

  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), toggleTimer, CHANGE);

  // Initialize EEPROM
  EEPROM.begin(512);

  // Set initial time if EEPROM is empty
  if (EEPROM.read(0) == 255 && EEPROM.read(1) == 255) {
    // Set initial time (HH:MM:SS)
    EEPROM.write(0, 0); // hours
    EEPROM.write(1, 0);  // minutes
    EEPROM.write(2, 0);  // seconds
    EEPROM.commit();
  }
}

void loop() {
  if (timerRunning) {
    unsigned long currentTime = millis();
    elapsedTime = currentTime - startTime;

    // Calculate hours, minutes, and seconds
    int hours = elapsedTime / 3600000;
    int minutes = (elapsedTime % 3600000) / 60000;
    int seconds = (elapsedTime % 60000) / 1000;

    // Display elapsed time on LCD
    lcd.setCursor(0, 0);
    lcd.print("Counter:");
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

  delay(1000); // Update every second
}

void toggleTimer() {
  // Check the state of GPIO pin 18
  if (digitalRead(interruptPin) == LOW) {
    // If pin is LOW, start the timer
    timerRunning = true;
    startTime = millis() - elapsedTime; // Resume from where it left off
  } else {
    // If pin is HIGH, stop the timer
    timerRunning = false;
  }
}
