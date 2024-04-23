#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include <RTClib.h>
#include <ESP32Time.h>

ESP32Time rtc(3600);                
LiquidCrystal_I2C lcd(0x27, 20, 4);  

int optoPinm1 = 27;
int optoPinm2 = 18;
int optoPinm3 = 26;
int optoPinm4 = 14;
int optoPinm5 = 25;

unsigned long mcycleStartTime = 0;
unsigned long mcycleTime = 0;
int mhr = 0, mmin = 0;

int rtcHourAddress = sizeof(int);
int rtcMinuteAddress = rtcHourAddress + sizeof(int);
int rtcSecondAddress = rtcMinuteAddress + sizeof(int);
int mcycleadd = sizeof(int) * 8;

bool mrunning = false;

void setup() {
  pinMode(optoPinm1, INPUT_PULLUP);
  pinMode(optoPinm2, INPUT_PULLUP);
  pinMode(optoPinm3, INPUT_PULLUP);
  pinMode(optoPinm4, INPUT_PULLUP);
  pinMode(optoPinm5, INPUT_PULLUP);

  lcd.begin(20, 4);
  lcd.init();
  lcd.backlight();

  rtc.setTime(0, 0, 23, 12, 6, 2023);
  EEPROM.begin(512);

  storedHour = int(EEPROM.read(rtcHourAddress));
  storedMinute = int(EEPROM.read(rtcMinuteAddress));
  storedSecond = int(EEPROM.read(rtcSecondAddress));
  rtc.setTime(storedSecond, storedMinute, storedHour - 1, 12, 6, 2023);

  if (!mrunning) {
    mcycleStartTime = millis();
    mrunning = true;
  }
}

void loop() {
  if (digitalRead(optoPinm2) == LOW) {
    mode = 1;
    mrunning = false;
    if (!mrunning) {
      mcycleStartTime = millis();
      mrunning = true;
    }

    if (mrunning) {
      mcycleTime = (millis() - mcycleStartTime) / 1000;
      if (mcycleTime == 0) {
        mhr = 0;
        mmin = 0;
      } else {
        mhr = mcycleTime / 3600;
        mmin = mcycleTime / 60;
      }
      lcd.setCursor(0, 0);
      lcd.print("Maintenance ;)");
      lcd.setCursor(0, 1);
      lcd.print(String(mhr) + ":" + String(mmin) + ":" + String(mcycleTime % 60));
      EEPROM.put(mcycleadd, mmin);
    }
  }
}
