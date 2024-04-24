#include <EEPROM.h>

#define EEPROM_SIZE 4096 // Size of EEPROM on ESP32-WROOM-32 module

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  
  // Initialize EEPROM with the size of EEPROM_SIZE
  EEPROM.begin(EEPROM_SIZE);

  // Clear EEPROM
  for (int i = 0; i < EEPROM_SIZE; ++i) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();

  Serial.println("EEPROM cleared.");
}

void loop() {
  // Your main code goes here
}
