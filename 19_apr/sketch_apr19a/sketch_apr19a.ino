#include <Wire.h>

void setup() {
  Serial.begin(9600);
  while (!Serial);
  
  Wire.begin();
  
  Serial.println("Scanning I2C bus...");
  scanI2C();
}

void loop() {
  // Nothing to do here for this example
}

void scanI2C() {
  byte error, address;
  int deviceCount = 0;

  Serial.println("I2C devices found:");

  for(address = 1; address < 127; address++ ) {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("Device found at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("");
      deviceCount++;
    }    
    else if (error==4) {
      Serial.print("Unknown error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  
  if (deviceCount == 0) {
    Serial.println("No I2C devices found");
  } else {
    Serial.println("Scan complete");
  }
}
