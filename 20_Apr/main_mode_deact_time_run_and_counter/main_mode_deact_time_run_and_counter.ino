#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);  // Address 0x27 for a 20x4 character display

const int interruptPin = 18;

bool counterRunning = true;

unsigned long previousMillis = 0;
const long interval = 1000;  // Update interval in milliseconds

int hours = 0;
int minutes = 0;
int seconds = 0;

int counterHours = 0;
int counterMinutes = 0;
int counterSeconds = 0;

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

    // Update LCD display
    updateLCD();
    
    // Update time in EEPROM
    EEPROM.write(0, hours);
    EEPROM.write(1, minutes);
    EEPROM.write(2, seconds);
    EEPROM.commit();
  }
}

void updateLCD() {
  // Blank the screen when counterRunning is false
  if (!counterRunning) {
    lcd.clear();
    // Keep backlight on
    lcd.backlight();
  } else {
    // Display time on LCD
    lcd.setCursor(0, 0);
    lcd.print("  Mode:Maintenance  ");
    lcd.setCursor(0, 2);
    lcd.print("   Time:");
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

    // Display counter on LCD
    lcd.setCursor(0, 1);
    lcd.print("  Counter: ");
    lcd.print(counterHours);
    lcd.print(":");
    if (counterMinutes < 10) {
      lcd.print("0");
    }
    lcd.print(counterMinutes);
    lcd.print(":");
    if (counterSeconds < 10) {
      lcd.print("0");
    }
    lcd.print(counterSeconds);
  }
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
