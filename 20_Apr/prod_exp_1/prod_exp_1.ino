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
  
IPAddress newip, newgip, newnmask;
int epip = 0;
const char* ssid = "SUKRIT";
const char* password = "Sukrit@2023";
IPAddress Ip(192, 168, 1, 121);
IPAddress GIp(192, 168, 21, 1);
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
  //code
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
  Serial.begin(115200);
  Serial.println("Serial communication initialized");

  pinMode(optoPin2, INPUT);
  pinMode(optoPinrst, INPUT);
  pinMode(optoPinm1, INPUT);
  pinMode(optoPinm2, INPUT);
  pinMode(optoPinm3, INPUT);
  pinMode(optoPinm4, INPUT);
  pinMode(optoPinm5, INPUT);

  WiFi.mode(WIFI_AP);
  EEPROM.get(epipadd, epip);
  if (epip == 1) {
    EEPROM.get(ssidadd, ssid);
    EEPROM.get(passwordadd, password);
    String ip1, ip2, ip3;
    EEPROM.get(Ipadd, ip1);
    Ip.fromString(ip1);
    EEPROM.get(GIpadd, ip2);
    GIp.fromString(ip2);
    EEPROM.get(NMaskadd, ip3);
    NMask.fromString(ip3);
  }
  WiFi.softAP(ssid, password);
  delay(100);
  WiFi.softAPConfig(Ip, GIp, NMask);

  IPAddress ip = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(ip);
  Serial.println("Server started");

  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    String message = (String(p_on_time) + ", " + String(prod_time) + ", " + String(n_prod_time) + "," + String(cycleTime) + "," + String(counter1) + "," + String(var1) + "," + String(rtc.getHour()) + ":" + String(rtc.getMinute()) + ":" + String(rtc.getSecond()) + "," + String(p_o) + "," + String(p_min) + "," + String(mmin) + "," + String(smin) + "," + String(mode));
    request->send(200, "text/plain", message);
  });

  server.on("/post", HTTP_POST, [](AsyncWebServerRequest* request) {
    AsyncWebParameter* j = request->getParam(0);
    String postBody = j->value();

    if (postBody == "reset") {
      AsyncWebParameter* k = request->getParam(1);
      getsec = (k->value()).toInt();
      AsyncWebParameter* l = request->getParam(2);
      getmin = (l->value()).toInt();
      AsyncWebParameter* m = request->getParam(3);
      gethr = (m->value()).toInt();
      rtc.setTime(getsec, getmin, (gethr - 1), 12, 6, 2023);
      AsyncWebParameter* n = request->getParam(4);
      p_o = (n->value()).toInt();
      AsyncWebParameter* o = request->getParam(5);
      p_min = (o->value()).toInt();
      request->send(200, "text/plain", "time reset successful");

      sec = getsec;
      min1 = getmin;
      hr = gethr;

      EEPROM.write(rtcHourAddress, hr);
      EEPROM.write(rtcMinuteAddress, min1);
      EEPROM.write(rtcSecondAddress, sec);
      EEPROM.put(p_oadd, p_o);      // Write the integer to the first four slots
      EEPROM.put(p_minadd, p_min);  // Write the integer to the first four slots
      EEPROM.commit();
    } else {
      request->send(400, "text/plain", "invalid action");
    }
  });

  server.onNotFound(notFound);
  server.begin();

  rtc.setTime(rtcSecondAddress, rtcMinuteAddress, rtcHourAddress, 12, 6, 2023);

  lcd.begin(20, 4);
  lcd.clear();

  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
}

void loop() {
  // Your main loop code here...
}
