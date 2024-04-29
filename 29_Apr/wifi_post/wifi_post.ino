#include <WiFi.h>
#include <EEPROM.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebSrv.h>

AsyncWebServer server(80);

const int ssidadd = 0;
const int passwordadd = ssidadd + sizeof(String);
const int Ipadd = passwordadd + sizeof(String);
const int GIpadd = Ipadd + sizeof(String);
const int NMaskadd = GIpadd + sizeof(String);
const int epipadd = NMaskadd + sizeof(String);

IPAddress newip, newgip, newnmask;
int epip = 0;
IPAddress ip; // Declare 'ip' as non-const

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);
  
  String ssid, password;
  IPAddress gateway, subnet;

  // Default SSID and password
  ssid = "esp32";
  password = "esp12345";

  // Default IP configuration
  ip = IPAddress(192, 168, 1, 1);
  gateway = IPAddress(192, 168, 1, 1);
  subnet = IPAddress(255, 255, 255, 0);

  EEPROM.get(ssidadd, ssid);
  EEPROM.get(passwordadd, password);
  EEPROM.get(Ipadd, ip);
  EEPROM.get(GIpadd, gateway);
  EEPROM.get(NMaskadd, subnet);
  EEPROM.get(epipadd, epip);

  if (epip == 1) {
    WiFi.softAP(ssid.c_str(), password.c_str());
    delay(100);
    WiFi.softAPConfig(ip, gateway, subnet);
  }

  server.on("/admin", HTTP_POST, [&ip](AsyncWebServerRequest* request) { // Capture 'ip' by reference
    AsyncWebParameter* action = request->getParam(0);
    String postBody = action->value();

    if (postBody == "reset_admin") {
      AsyncWebParameter* new_ssid = request->getParam(1);
      String newssid = new_ssid->value();
      AsyncWebParameter* new_password = request->getParam(2);
      String newpassword = new_password->value();
      AsyncWebParameter* new_ip = request->getParam(3);
      newip.fromString(new_ip->value());
      AsyncWebParameter* new_gip = request->getParam(4);
      newgip.fromString(new_gip->value());
      AsyncWebParameter* new_nmask = request->getParam(5);
      newnmask.fromString(new_nmask->value());
      request->send(200, "text/plain", "time reset successful");

      EEPROM.put(ssidadd, newssid);
      EEPROM.put(passwordadd, newpassword);
      EEPROM.put(Ipadd, newip);
      EEPROM.put(GIpadd, newgip);
      EEPROM.put(NMaskadd, newnmask);
      epip = 1;
      EEPROM.put(epipadd, epip);
      EEPROM.commit();
      if (epip == 1) {
        WiFi.softAP(newssid.c_str(), newpassword.c_str());
        delay(100);
        WiFi.softAPConfig(newip, newgip, newnmask);
        ip = WiFi.softAPIP(); // Update 'ip' variable
      }
    } else {
      request->send(400, "text/plain", "invalid action");
    }
  });

  server.begin();
}

void loop() {
}
