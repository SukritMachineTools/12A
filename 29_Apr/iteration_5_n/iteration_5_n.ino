#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);  // Set the LCD address to 0x27 for a 20 chars and 4 line display

const int gpioPin1 = 18;
const int gpioPin2 = 25;
const int gpioPin3 = 32;
const int gpioPin4 = 14;

bool timerRunning = false;
unsigned long startTime = 0;
unsigned long elapsedTime = 0;

int mode = 0; // Variable to store the current mode
int hours, minutes, seconds; // Declare global variables for time

void toggleTimer(); // Function prototype for toggleTimer()

void setup() {
  Wire.begin();
  pinMode(gpioPin1, INPUT);
  pinMode(gpioPin2, INPUT);
  pinMode(gpioPin3, INPUT);
  pinMode(gpioPin4, INPUT);
  lcd.init();                      // initialize the lcd
  lcd.backlight();
  attachInterrupt(digitalPinToInterrupt(gpioPin1), toggleTimer, CHANGE);
  // Initialize EEPROM
  EEPROM.begin(512);

  if (EEPROM.read(0) == 255 && EEPROM.read(1) == 255) {
    // Set initial time (HH:MM:SS)
    EEPROM.write(0, 0); // hours
    EEPROM.write(1, 0);  // minutes
    EEPROM.write(2, 0);  // seconds
    EEPROM.commit();
  }

}

void loop() {
  if (digitalRead(gpioPin1) == LOW) {
    mode = 1; // GPIO 18 is low
  } else if (digitalRead(gpioPin2) == LOW) {
    mode = 2; // GPIO 25 is low
  } else if (digitalRead(gpioPin3) == LOW) {
    mode = 3; // GPIO 32 is low
  } else if (digitalRead(gpioPin4) == LOW) {
    mode = 4; // GPIO 14 is low
  }
  else {
    mode = 0; // No GPIO is low
  }

  // Display the mode on the LCD
  if (mode == 1) {
    mainte(true);
    delay(100); // debounce delay
    while (digitalRead(gpioPin1) == LOW) {} // wait until gpio18 is no longer low
  } else if (mode == 2) {
    prod(true);
    delay(100); // debounce delay
    while (digitalRead(gpioPin2) == LOW) {} // wait until gpio25 is no longer low
  } else if (mode == 3) {
    setting(true);
    delay(100); // debounce delay
    while (digitalRead(gpioPin3) == LOW) {} // wait until gpio32 is no longer low
  } else if (mode == 4) {
    network(true);
    delay(100); // debounce delay
    while (digitalRead(gpioPin4) == LOW) {} // wait until gpio14 is no longer low
  } else {
    mainte(false); // No GPIO is low
    prod(false);
    setting(false);
    network(false);

    timer();

  }
}

void mainte(bool gpioState) {
  if (gpioState) {
    lcd.setCursor(0, 0);
    lcd.print("Mode: Maintenance");
    lcd.setCursor(18, 3);
    lcd.print(mode);
  } else {
    lcd.clear();
  }
}

void prod(bool gpioState) {
  if (gpioState) {
    lcd.setCursor(0, 0);
    lcd.print("Mode: Production");

    lcd.setCursor(0, 1);
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

    lcd.setCursor(18, 3);
    lcd.print(mode);
  } else {
    elapsedTime = 0;
    lcd.clear();
  }
  delay(1000); // Update every second
}

void setting(bool gpioState) {
  if (gpioState) {
    lcd.setCursor(0, 0);
    lcd.print("Mode: Setting");
    lcd.setCursor(18, 3);
    lcd.print(mode);
  } else {
    lcd.clear();
  }
}

void network(bool gpioState) {
  if (gpioState) {
    lcd.setCursor(0, 0);
    lcd.print("Mode: Network");
    lcd.setCursor(18, 3);
    lcd.print(mode);
  } else {
    lcd.clear();
  }
}

void timer() {
  if (timerRunning) {
    unsigned long currentTime = millis();
    elapsedTime = currentTime - startTime;

    // Calculate hours, minutes, and seconds
    hours = elapsedTime / 3600000;
    minutes = (elapsedTime % 3600000) / 60000;
    seconds = (elapsedTime % 60000) / 1000;
  }
}

void toggleTimer() {
  // Check the state of GPIO pin 18
  if (digitalRead(gpioPin1) == LOW) {
    // If pin is LOW, start the timer
    timerRunning = true;
    startTime = millis() - elapsedTime; // Resume from where it left off
  } else {
    // If pin is HIGH, stop the timer
    timerRunning = false;
  }
}
