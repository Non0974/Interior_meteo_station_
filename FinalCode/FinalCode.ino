#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_SGP40.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define SCREEN_WIDTH   128
#define SCREEN_HEIGHT   64
#define OLED_RESET      -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_BMP280  bmp;
Adafruit_SGP40   sgp;

void setup() 
{
  Serial.begin(115200);
  delay(2000);
  Wire.begin(21, 22);

  //OLED SH1106
  if (!display.begin(SCREEN_ADDRESS, true)) 
  {
    Serial.println("OLED SH1106 non trouvé !");
    while (1);
  }
  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);
  display.setTextSize(1);
  display.setCursor(20, 24);
  display.println("Station Météo");
  display.display();
  delay(1500);

  //BMP280
  if (!bmp.begin(0x76)) 
  {
    Serial.println("BMP280 non trouvé !");
    while (1);
  }
  Serial.println("BMP280 OK");

  //SGP40
  if (!sgp.begin()) 
  {
    Serial.println("SGP40 non trouvé !");
    while (1);
  }
  Serial.println("SGP40 OK");

  //Préchauffage du SGP40
  Serial.println("Préchauffage du SGP40...");
  for (int i = 10; i > 0; i--) 
  {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(10, 18);
    display.println("Prechauffage SGP40");
    display.setTextSize(2);
    display.setCursor(50, 36);
    display.print(i);
    display.print("s");
    display.display();
    delay(1000);
  }
}

String getQualite(int32_t vocIndex) 
{
  if      (vocIndex < 50)  return "EXCELLENT";
  else if (vocIndex < 150) return "BON      ";
  else if (vocIndex < 250) return "MOYEN    ";
  else if (vocIndex < 400) return "MAUVAIS  ";
  else                     return "DANGER   ";
}

void loop() 
{
  //Lecture du BMP280
  float temp = bmp.readTemperature();
  float pres = bmp.readPressure() / 100.0F;

  //Lecture du SGP40
  int32_t vocIndex = sgp.measureVocIndex(temp, 50);
  String qualite = getQualite(vocIndex);

  //Serial
  Serial.printf("Temp: %.1f°C | Pres: %.1f hPa\n", temp, pres);
  Serial.printf("VOC Index: %d | Qualité: %s\n\n", vocIndex, qualite.c_str());

  //Affichage OLED
  display.clearDisplay();
  display.setTextWrap(false);

  //Température
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Temp :");
  display.setTextSize(2);
  display.setCursor(52, 0);
  display.print(temp, 1);
  display.print("C");

  //Pression
  display.setTextSize(1);
  display.setCursor(0, 18);
  display.print("Pres : ");
  display.print(pres, 1);
  display.print(" hPa");

  //Séparateur
  display.drawLine(0, 30, 128, 30, SH110X_WHITE);

  //VOC index
  display.setCursor(0, 34);
  display.print("VOC index :");
  display.setTextSize(2);
  display.setCursor(74, 32);
  display.print(vocIndex);

  //Qualité de l'air
  display.setTextSize(1);
  display.setCursor(0, 52);
  display.print("Qualité de l'air : ");
  display.print(qualite);

  display.display();
  delay(1000);
}