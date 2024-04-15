#include <Wire.h>

#include <ESP8266WiFi.h>

#include <ESP8266WebServer.h>

#include <LiquidCrystal_I2C.h>

#include <EEPROM.h>

#include <ESP32Time.h>


ESP32Time rtc(3600);  // offset in seconds GMT+1
// RTC_DS3231 rtc;


LiquidCrystal_I2C lcd(0x27, 20, 4);  // Set the I2C address to 0x27 and dimensions to 20x4



int counter1 = 0;  // Counter controlled by Button 1

int t_Start = 0;

int t_stop = 0;

int optoPin2 = 2;

int optoPinrst = 14;

//int button1Pin = 12; // Button 1 pin

int prod_ButtonPin = 13;  // Button 2 pin

//int resetButtonPin = 13; // Reset button pin

//int ledPin = 2; // LED pin



//int button1State = LOW;

int optoPin2State = LOW;

//int lastButton1State = LOW;

int lastoptoPin2State = LOW;



int p_b_state = LOW;

int p_b_laststate = LOW;

unsigned long lastDebounceTime1 = 0;

unsigned long debounceDelay = 50;

unsigned long p_on_time = 0;

unsigned long prod_time = 0;

unsigned long n_prod_time = 0;

unsigned long eff_time = 0;

unsigned long var1 = 0;

unsigned long var2 = 0;


int counter1Address = 0;  // Address in the EEPROM emulation to store the counter1 value

int cycleTimeAddress = sizeof(counter1);  // Address in the EEPROM emulation to store the cycle time value



unsigned long cycleStartTime = 0;  // To store the time when the button is pressed

unsigned long cycleTime = 0;  // To store the calculated cycle time

//my variables
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
// int myvar, myvaraddress;
String myvar2;
ESP8266WebServer server(80);



const char* ssid = "DeviceL1";

const char* password = "";



// void handleRoot()

// {

//   //String message = "test theddd !";



//   // server.send(200, "text/plain", message);
// }

void handleRoot() {
  if (server.method() == HTTP_GET) {
    String message = (String(p_on_time) + ", " + String(prod_time) + ", " + String(n_prod_time) + "," + String(cycleTime) + "," + String(counter1) + "," + String(rtc.getTime()) + "," + String(var1) + "," + currentRtcTime);



    server.send(200, "text/plain", message);
  } else if (server.method() == HTTP_POST) {
    String action = server.arg("action");  // Extract the action parameter from the POST data

    if (action == "resetCounter") {
      String second = server.arg("second");
      String minute = server.arg("minute");       String hour = server.arg("hour");
      sec = second.toInt();
      min1 = minute.toInt();
      hr = hour.toInt();
      lcd.setCursor(10, 3);
      rtc.setTime(sec, min1, (hr - 1), 12, 6, 2023);
      lcd.print(currentRtcTime);  //showing time before reset
      // counter1 = 0;
      // EEPROM.put(counter1Address, counter1);
      // EEPROM.commit();
      server.send(200, "text/plain", "time reset successful");
    } else {
      server.send(400, "text/plain", "Invalid action");
    }
  } else {
    server.send(404, "text/plain", "Not found");
  }
}



void setup() {

  pinMode(optoPin2, INPUT);

  pinMode(optoPinrst, INPUT);



  //String message = " 1234567";

  //server.send(200, "text/plain", message);



  WiFi.mode(WIFI_AP);

  WiFi.softAP(ssid, password);

  IPAddress ip = WiFi.softAPIP();

  Serial.print("AP IP address: ");

  Serial.println(ip);



  server.on("/", handleRoot);

  server.begin();

  Serial.println("Server started");

  lcd.begin(20, 4);

  lcd.init();

  lcd.backlight();


  rtc.setTime(0, 0, 23, 12, 6, 2023);
  // rtc.setTime(storedSecond, storedMinute, storedHour - 1, 12, 6, 2023);




  // EEPROM.begin(sizeof(counter1) + sizeof(cycleTime) + sizeof(myvaraddress));  // Initialize EEPROM with the desired data size
  EEPROM.begin(512);


  // Read the counter and cycle time values from EEPROM during initialization

  EEPROM.get(counter1Address, counter1);

  EEPROM.get(cycleTimeAddress, cycleTime);





  if (counter1 < 0 || counter1 > 9999) {

    // Invalid counter1 value stored in EEPROM, reset to zero

    counter1 = 0;
  }

  //pinMode(button1Pin, INPUT_PULLUP);

  pinMode(optoPin2, INPUT_PULLUP);



  //pinMode(resetButtonPin, INPUT_PULLUP);

  pinMode(optoPinrst, INPUT_PULLUP);


  //pinMode(ledPin, OUTPUT);

  storedHour = int(EEPROM.read(rtcHourAddress)) ;
  storedMinute = int(EEPROM.read(rtcMinuteAddress)) ;
  storedSecond = int(EEPROM.read(rtcSecondAddress)) ;
  myvar2 = String(storedHour) + ":" + String(storedMinute) + ":" + String(storedSecond);
  lcd.setCursor(11, 3);
  lcd.print(myvar2);
}





void loop()

{



  server.handleClient();

  String message = (String(p_on_time) + ", " + String(prod_time) + ", " + String(n_prod_time) + "," + String(cycleTime) + "," + String(counter1) + "," + String(rtc.getTime()) + "," + String(var1) + "," + currentRtcTime);



  server.send(200, "text/plain", message);



  /*

button1State = digitalRead(button1Pin);

 if (button1State != lastButton1State)

   {

    if (button1State == LOW)

     {

      */



  optoPin2State = digitalRead(optoPin2);

  if (optoPin2State != lastoptoPin2State)

  {

    if (optoPin2State == LOW)

    {

      cycleStartTime = millis();

      //     lcd.setCursor(0, 2);

      // lcd.print(rtc.getTime());
    }

    //if (button1State == HIGH)

    if (optoPin2State == HIGH)

    {
      lcd.setCursor(10, 1);

      lcd.print("PRT1:" + String(counter1));

      cycleTime = millis() - cycleStartTime;  // Calculate the cycle time

      cycleTime = cycleTime / 1000;

      var1 = cycleTime + var1;

      counter1 = counter1 + 1;

      //     lcd.setCursor(0, 3);

      //     lcd.print(rtc.getTime());

      lcd.setCursor(0, 1);

      lcd.print("C.T.:" + String(cycleTime));
    }

    //lastButton1State = button1State;

    lastoptoPin2State = optoPin2State;
  }




  p_on_time = millis() / 60000;

  prod_time = var1 / 60;

  n_prod_time = p_on_time - prod_time;

  lcd.setCursor(0, 0);

  lcd.print(rtc.getTime());

  lcd.setCursor(10, 0);

  lcd.print(p_on_time);

  lcd.setCursor(0, 2);

  lcd.print("P.T.: " + String(prod_time));

  lcd.setCursor(10, 2);

  lcd.print("NP.T.: " + String(n_prod_time));

  lcd.setCursor(0, 3);

  lcd.print(var1);

  currentRtcTime = rtc.getTime();
  sec = rtc.getSecond();
  min1 = rtc.getMinute();
  hr = rtc.getHour();

  // myvar = 75;
  // myvaraddress = 10 + sizeof(int);

  EEPROM.write(rtcHourAddress, hr);
  EEPROM.write(rtcMinuteAddress, min1);
  EEPROM.write(rtcSecondAddress, sec);

  EEPROM.commit();








  //if (digitalRead(resetButtonPin) == LOW) {

  if (digitalRead(optoPinrst) == LOW) {

    counter1 = 0;

    cycleTime = 0;

    lcd.setCursor(0, 1);

    lcd.print("    ");

    lcd.setCursor(0, 1);

    lcd.print("C.T.:" + String(cycleTime) + " ");

    lcd.setCursor(10, 1);

    lcd.print("   ");

    lcd.setCursor(10, 1);

    lcd.print("PRT1:" + String(counter1) + " ");



    // Store the reset counter values in EEPROM

    EEPROM.put(counter1Address, counter1);

    EEPROM.commit();
  }
}