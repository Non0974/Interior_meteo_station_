#include <Wire.h>

void setup() 
{
  Serial.begin(115200);
  delay(3000); //laisse le temps d'ouvrir le Serial Monitor
  Wire.begin(21, 22);
  Serial.println("Scan en cours...");

  bool found = false;
  for (byte addr = 1; addr < 127; addr++) 
  {
    Wire.beginTransmission(addr);
    byte error = Wire.endTransmission();
    if (error == 0) 
    {
      Serial.print("Périphérique trouvé à 0x");
      Serial.println(addr, HEX);
      found = true;
    }
  }
  if (!found) Serial.println("Aucun périphérique trouvé !");
  Serial.println("Scan terminé.");
}

void loop() 
{
  Serial.println("loop OK"); // pour confirmer que l'ESP32 tourne
  delay(2000);
}