#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);  // Set the LCD address to 0x27 for a 20 chars and 4 line display

const int gpioPin1 = 18;
const int gpioPin2 = 25;
const int gpioPin3 = 32;
const int gpioPin4 = 14;

int mode = 0; // Variable to store the current mode

void setup() {
  pinMode(gpioPin1, INPUT);
  pinMode(gpioPin2, INPUT);
  pinMode(gpioPin3, INPUT);
  pinMode(gpioPin4, INPUT);
  lcd.init();                      // initialize the lcd
  lcd.backlight();
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
    lcd.setCursor(18, 3);
    lcd.print(mode);
  } else {
    lcd.clear();
  }
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
