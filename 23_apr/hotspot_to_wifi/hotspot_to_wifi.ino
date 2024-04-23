#include <Wire.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebSrv.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include <RTClib.h> 
#include <ESP32Time.h>

ESP32Time rtc(3600);
LiquidCrystal_I2C lcd(0x27, 20, 4);

int counter1 = 0;
int t_Start = 0;
int t_stop = 0;
int optoPin2 = 13;
int optoPinrst = 33;
int optoPinm1 = 27;
int optoPinm2 = 18;
int optoPinm3 = 26;
int optoPinm4 = 14;
int optoPinm5 = 25;

int optoPin2State = LOW;
int lastoptoPin2State = LOW;
int optoPinm1State = LOW;

unsigned long lastDebounceTime1 = 0;
unsigned long debounceDelay = 50;
unsigned long p_on_time = 0;
unsigned long prod_time = 0;
unsigned long n_prod_time = 0;
unsigned long eff_time = 0;
unsigned long var1 = 0;
unsigned long var2 = 0;

int counter1Address = 0;
int cycleTimeAddress = sizeof(counter1);

unsigned long cycleStartTime = 0;
unsigned long cycleTime = 0;

unsigned long newVar2 = 0;
unsigned long lastVar2Update = 0;
unsigned long var2UpdateInterval = 1000;

String currentRtcTime;
int rtaddress = 10;
const int maxStringLength = 50;
unsigned long sec = 0;
unsigned long min1 = 0;
unsigned long hr = 0; 
int rtcHourAddress = +sizeof(int);
int rtcMinuteAddress = rtcHourAddress + sizeof(int);
int rtcSecondAddress = rtcMinuteAddress + sizeof(int);
int storedHour, storedMinute, storedSecond;
int Ttimead = rtcSecondAddress + sizeof(int);
int Ctimeadd = Ttimead + sizeof(int);
int Prt1add = Ctimeadd + sizeof(int);
int PTadd = Prt1add + sizeof(int);
int NPTadd = PTadd + sizeof(int);
int var1add = NPTadd + sizeof(int);
int counter1add = var1add + sizeof(int);
unsigned long p_on_timeval, prod_timeval, n_prod_timeval, cycleTimeval, var1val, counter1val;
int milliFlag = 0;
unsigned long resetcycleStartTime = 0;
String myvar2;
int getsec, getmin, gethr;
unsigned long p_o = 0;
unsigned long p_oadd = counter1add + sizeof(unsigned long);
unsigned long p_min = 0;
unsigned long p_minadd = p_oadd + sizeof(unsigned long);

unsigned long mode = 0;
int mtime = 0, setime = 0;
unsigned long mcycleStartTime = 0, scycleStartTime = 0;
unsigned long mcycleTime = 0, scycleTime = 0;
unsigned long mcycleadd = p_minadd + sizeof(unsigned long);
unsigned long scycleadd = mcycleadd + sizeof(unsigned long);
int pflag = 0, mflag = 0, sflag = 1;
static unsigned long mTime = 0, pTime = 0, sTime = 0;
static bool srunning = false, mrunning = false;
int mhr = 0, mmin = 0, shr = 0, smin = 0;

IPAddress newip(192, 168, 1, 121);
IPAddress newgip(192, 168, 1, 1);
IPAddress newnmask(255, 255, 255, 0);
int epip = 0;
const char* ssid = "SUKRIT";
const char* password = "Sukrit@2023";
IPAddress Ip(192, 168, 1, 121);
IPAddress GIp(192, 168, 1, 1);
IPAddress NMask(255, 255, 255, 0);
unsigned long Ipadd = scycleadd + sizeof(unsigned long);
unsigned long GIpadd = Ipadd + sizeof(unsigned long);
unsigned long NMaskadd = GIpadd + sizeof(unsigned long);
unsigned long ssidadd = NMaskadd + sizeof(unsigned long);
unsigned long passwordadd = ssidadd + sizeof(unsigned long);
unsigned long epipadd = passwordadd + sizeof(unsigned long);

const int relay2 = 19;
const int relay1 = 23;

String emp = "     ";
AsyncWebServer server(80);

void notFound(AsyncWebServerRequest* request) {
  request->send(404, "text/plain", "Not found");
}

void resettime() {
  // code
}

void resetparams() {
  counter1 = 0;
  cycleTime = 0;
  counter1val = 0;
  cycleTime = 0;
  var1 = 0;
  var1val = 0;
  resetcycleStartTime = millis();
  milliFlag = 1;
  p_o = 0;
  p_min = 0;
  mcycleTime = 0;
  scycleTime = 0;
  mmin = 0;
  smin = 0;

  lcd.setCursor(0, 1);
  lcd.print("    ");

  lcd.setCursor(0, 1);
  lcd.print("C.T.:" + String(cycleTime) + "  ");

  lcd.setCursor(10, 1);
  lcd.print("   ");

  lcd.setCursor(10, 1);
  lcd.print("PRT1:" + String(counter1) + "  ");

  rtc.setTime(0, 0, 23, 12, 6, 2023);

  lcd.setCursor(10, 0);
  lcd.print(String(p_on_time) + " ");

  lcd.setCursor(0, 2);
  lcd.print("P.T.: " + String(prod_time) + " ");

  lcd.setCursor(10, 2);
  lcd.print("NP.T.: " + String(n_prod_time) + " ");

  lcd.setCursor(0, 3);
  lcd.print(String(var1) + " ");

  lcd.setCursor(15, 0);
  lcd.print(String(p_o) + " ");

  lcd.setCursor(17, 0);
  lcd.print(String(p_min) + " ");

  EEPROM.put(mcycleadd, mmin);
  EEPROM.put(scycleadd, smin);
  EEPROM.put(p_oadd, p_o);
  EEPROM.put(p_minadd, p_min);
  EEPROM.put(counter1Address, counter1);
  EEPROM.commit();
}

void setup() {
  pinMode(optoPin2, INPUT);
  pinMode(optoPinrst, INPUT);
  pinMode(optoPinm1, INPUT);
  pinMode(optoPinm2, INPUT);
  pinMode(optoPinm3, INPUT);
  pinMode(optoPinm4, INPUT);
  pinMode(optoPinm5, INPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println(WiFi.localIP());
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "text/html", homepage);
  });

  server.on("/on1", HTTP_GET, [](AsyncWebServerRequest* request) {
    digitalWrite(relay1, HIGH);
    request->send(200, "text/plain", "Relay 1 is ON");
  });

  server.on("/off1", HTTP_GET, [](AsyncWebServerRequest* request) {
    digitalWrite(relay1, LOW);
    request->send(200, "text/plain", "Relay 1 is OFF");
  });

  server.on("/on2", HTTP_GET, [](AsyncWebServerRequest* request) {
    digitalWrite(relay2, HIGH);
    request->send(200, "text/plain", "Relay 2 is ON");
  });

  server.on("/off2", HTTP_GET, [](AsyncWebServerRequest* request) {
    digitalWrite(relay2, LOW);
    request->send(200, "text/plain", "Relay 2 is OFF");
  });

  server.onNotFound(notFound);
  server.begin();

  lcd.begin(20, 4);
  lcd.init();
  lcd.backlight();
  rtc.begin();
  lcd.setCursor(0, 0);
  lcd.print("C:");
  rtc.getTime();
  storedHour = rtc.hour;
  storedMinute = rtc.minute;
  storedSecond = rtc.second;
  lcd.print(rtc.hour);
  lcd.print(':');
  lcd.print(rtc.minute);
  lcd.print(':');
  lcd.print(rtc.second);
  lcd.setCursor(0, 1);
  lcd.print("  ");
  lcd.setCursor(0, 1);
  lcd.print("C.T.:" + String(cycleTime) + "  ");
  lcd.setCursor(10, 1);
  lcd.print("  ");
  lcd.setCursor(10, 1);
  lcd.print("PRT1:" + String(counter1) + "  ");
  lcd.setCursor(0, 2);
  lcd.print("P.T.: " + String(prod_time) + " ");
  lcd.setCursor(10, 2);
  lcd.print("NP.T.: " + String(n_prod_time) + " ");
  lcd.setCursor(0, 3);
  lcd.print(String(var1) + " ");
  lcd.setCursor(0, 3);
  lcd.print(String(var1) + " ");

  EEPROM.begin(512);

  EEPROM.get(mcycleadd, mmin);
  EEPROM.get(scycleadd, smin);
  EEPROM.get(p_oadd, p_o);
  EEPROM.get(p_minadd, p_min);
  EEPROM.get(counter1Address, counter1);
  EEPROM.get(cycleTimeAddress, cycleTime);
}

void loop() {
  rtc.getTime();
  getsec = rtc.second;
  getmin = rtc.minute;
  gethr = rtc.hour;
  mode = digitalRead(optoPinrst);
  if (mode == 1) {
    lcd.setCursor(0, 0);
    lcd.print("Reseting Parameters");
    delay(5000);
    lcd.setCursor(0, 0);
    lcd.print("                 ");
    resetparams();
  }

  if (milliFlag == 1) {
    mcycleTime = millis() - resetcycleStartTime;
    scycleTime = millis() - resetcycleStartTime;
    mcycleStartTime = mcycleStartTime + mcycleTime;
    scycleStartTime = scycleStartTime + scycleTime;
    mcycleTime = 0;
    scycleTime = 0;
    milliFlag = 0;
  }

  p_on_time = (millis() - mcycleStartTime) / 60000;
  prod_time = (millis() - scycleStartTime) / 60000;
  n_prod_time = ((millis() - scycleStartTime) / 60000) - p_on_time;

  if (p_on_time >= 60) {
    p_on_time = 0;
    mcycleStartTime = millis();
  }

  if (prod_time >= 60) {
    prod_time = 0;
    scycleStartTime = millis();
  }

  if (n_prod_time >= 60) {
    n_prod_time = 0;
    mcycleStartTime = millis();
  }

  if (optoPin2State == HIGH && lastoptoPin2State == LOW) {
    if (getsec == 0 && getmin == 0) {
      counter1 = counter1 + 1;
    }
  }

  lastoptoPin2State = optoPin2State;

  if (optoPin2State == HIGH) {
    if (getsec == 0 && getmin == 0) {
      lcd.setCursor(0, 0);
      lcd.print("C.T.:" + String(cycleTime) + "  ");

      lcd.setCursor(10, 0);
      lcd.print("P.O.:" + String(p_on_time) + " ");

      lcd.setCursor(10, 1);
      lcd.print("P.T.:" + String(prod_time) + " ");

      lcd.setCursor(10, 2);
      lcd.print("NP.T.:" + String(n_prod_time) + " ");

      lcd.setCursor(0, 1);
      lcd.print("PRT1:" + String(counter1) + "  ");
    }
  }

  if (getsec == 0 && getmin == 0) {
    cycleTime = cycleTime + 1;
  }

  if (getsec == 0 && getmin == 0 && gethr == 0) {
    cycleTime = 0;
  }

  if (counter1 == 100) {
    digitalWrite(relay1, HIGH);
  } else {
    digitalWrite(relay1, LOW);
  }

  if (counter1 == 200) {
    digitalWrite(relay2, HIGH);
  } else {
    digitalWrite(relay2, LOW);
  }

  if (WiFi.status() == WL_CONNECTED) {
    lcd.setCursor(0, 3);
    lcd.print("C:");
    lcd.print(rtc.hour);
    lcd.print(':');
    lcd.print(rtc.minute);
    lcd.print(':');
    lcd.print(rtc.second);
    lcd.print(" IP:");
    lcd.print(WiFi.localIP());
  }

  if (getsec == 0 && getmin == 0) {
    EEPROM.put(mcycleadd, mmin);
    EEPROM.put(scycleadd, smin);
    EEPROM.put(p_oadd, p_o);
    EEPROM.put(p_minadd, p_min);
    EEPROM.put(counter1Address, counter1);
    EEPROM.put(cycleTimeAddress, cycleTime);
    EEPROM.commit();
  }
}
