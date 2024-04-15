#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include <ESP32Time.h>
ESP32Time rtc(3600);  // offset in seconds GMT+1


LiquidCrystal_I2C lcd(0x27, 20, 4); // Set the I2C address to 0x27 and dimensions to 20x4


int counter1 = 0; // Counter controlled by Button 1
int t_Start = 0;
int t_stop = 0;


int button1Pin = 12; // Button 1 pin
int prod_ButtonPin = 14; // Button 2 pin
int resetButtonPin = 13; // Reset button pin
int ledPin = 2; // LED pin


int button1State = LOW;
int lastButton1State = LOW;
int p_b_state=LOW;
int p_b_laststate=LOW;
unsigned long lastDebounceTime1 = 0;
unsigned long debounceDelay = 50;
unsigned long p_on_time=0;
unsigned long prod_time=0;
unsigned long n_prod_time=0;
unsigned long eff_time=0;
unsigned long var1=0;
unsigned long var2=0;


int counter1Address = 0; // Address in the EEPROM emulation to store the counter1 value
int cycleTimeAddress = sizeof(counter1); // Address in the EEPROM emulation to store the cycle time value


unsigned long cycleStartTime = 0; // To store the time when the button is pressed
unsigned long cycleTime = 0; // To store the calculated cycle time




ESP8266WebServer server(80);




const char* ssid = "POCO";
const char* password = "";




void handleRoot()
 {
//String message = "test theddd !";


// server.send(200, "text/plain", message);
}




void setup() {


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




  rtc.setTime(30, 49, 12, 12, 6, 2023);


  EEPROM.begin(sizeof(counter1) + sizeof(cycleTime)); // Initialize EEPROM with the desired data size


  // Read the counter and cycle time values from EEPROM during initialization
  EEPROM.get(counter1Address, counter1);
  EEPROM.get(cycleTimeAddress, cycleTime);


  if (counter1 < 0 || counter1 > 9999) {
    // Invalid counter1 value stored in EEPROM, reset to zero
    counter1 = 0;
  }
  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(resetButtonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);






}




void loop()
{


  server.handleClient();
  String message = ( String(p_on_time) + ", " + String(prod_time) + ", " + String(n_prod_time) + "," + String(cycleTime) + "," + String(counter1));


  server.send(200, "text/plain", message);




button1State = digitalRead(button1Pin);
 if (button1State != lastButton1State)
   {
    if (button1State == LOW)
     {
     
       cycleStartTime = millis();
  //     lcd.setCursor(0, 2);
   // lcd.print(rtc.getTime());
    }
    if (button1State == HIGH)
     {
      cycleTime = millis() - cycleStartTime; // Calculate the cycle time
      cycleTime=cycleTime/1000;
      var1=cycleTime+var1;
      counter1=counter1+1;
 //     lcd.setCursor(0, 3);
 //     lcd.print(rtc.getTime());
      lcd.setCursor(0, 1);
      lcd.print("C.T.:" + String(cycleTime));
      lcd.setCursor(10, 1);
      lcd.print("PRT1:" + String(counter1));




    }
    lastButton1State = button1State;
  }






  p_on_time=millis()/60000;
  prod_time=var1/60;
  n_prod_time=p_on_time-prod_time;
  lcd.setCursor(0, 0);
  lcd.print(rtc.getTime());
  lcd.setCursor(10, 0);
  lcd.print(p_on_time);    
   lcd.setCursor(0, 2);
  lcd.print("P.T.: " + String(prod_time));
 lcd.setCursor(10, 2);
  lcd.print("NP.T.: " + String(n_prod_time));
  lcd.setCursor(0,3);
  lcd.print(var1);






  if (digitalRead(resetButtonPin) == LOW) {
    counter1 = 0;
    cycleTime=0;
     lcd.setCursor(0, 1);
     lcd.print("    " );
        lcd.setCursor(0, 1);
      lcd.print("C.T.:" + String(cycleTime));
      lcd.setCursor(10, 1);
      lcd.print("   " );
         lcd.setCursor(10, 1);
      lcd.print("PRT1:" + String(counter1));


    // Store the reset counter values in EEPROM
    EEPROM.put(counter1Address, counter1);
    EEPROM.commit();
  }




}
