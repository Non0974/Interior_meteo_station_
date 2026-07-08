#include <Wire.h>

void setup() 
{
  Serial.begin(115200);
  Wire.begin(21, 22); // SDA, SCL
  Serial.println("Scan I2C...");

  for (byte addr = 1; addr < 127; addr++) 
  {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) 
    {
      Serial.print("Trouvé à 0x");
      Serial.println(addr, HEX);
    }
  }
  Serial.println("Terminé.");
}

void loop() 
{
  
}