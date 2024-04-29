#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);  // Set the LCD address to 0x27 for a 20 chars and 4 line display

const int gpioPin1 = 18;
const int gpioPin2 = 25;
const int gpioPin3 = 32;
const int gpioPin4 = 14;

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
    mainte(true); // GPIO 18 is low
    delay(100); // debounce delay
    while (digitalRead(gpioPin1) == LOW) {} // wait until gpio18 is no longer low
    lcd.clear();
  } else if (digitalRead(gpioPin2) == LOW) {
    prod(true); // GPIO 25 is low
    delay(100); // debounce delay
    while (digitalRead(gpioPin2) == LOW) {} // wait until gpio25 is no longer low
    lcd.clear();
  } else if (digitalRead(gpioPin3) == LOW) {
    setting(true); // GPIO 32 is low
    delay(100); // debounce delay
    while (digitalRead(gpioPin3) == LOW) {} // wait until gpio32 is no longer low
    lcd.clear();
  } else if (digitalRead(gpioPin4) == LOW) {
    network(true); // GPIO 14 is low
    delay(100); // debounce delay
    while (digitalRead(gpioPin4) == LOW) {} // wait until gpio14 is no longer low
    lcd.clear();
  }  
  else {
    mainte(false); // GPIO 18 is high
    prod(false); // GPIO 25 is high
    setting(false); // GPIO 32 is high
    network(false); // GPIO 14 is high
  }
}

void mainte(bool gpioState) {
  if (gpioState) {
    lcd.setCursor(0, 0);
    lcd.print("Mode: Maintenance");
  } else {
    lcd.clear();
  }
}

void prod(bool gpioState) {
  if (gpioState) {
    lcd.setCursor(0, 0);
    lcd.print("Mode: Production");
  } else {
    lcd.clear();
  }
}

void setting(bool gpioState) {
  if (gpioState) {
    lcd.setCursor(0, 0);
    lcd.print("Mode: Setting");
  } else {
    lcd.clear();
  }
}

void network(bool gpioState) {
  if (gpioState) {
    lcd.setCursor(0, 0);
    lcd.print("Mode: Network");
  } else {
    lcd.clear();
  }
}
