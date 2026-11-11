#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_SGP40.h>

//WiFi 
const char* WIFI_SSID = "Wifi";
const char* WIFI_PASS = "MDP";

WebServer       server(80);
Adafruit_BMP280 bmp;
Adafruit_SGP40  sgp;

float   temp = 0, pres = 0;
int32_t voc  = 0;
unsigned long lastRead = 0;

//Page web
const char PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="fr">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Station météo</title>
<style>
  body{font-family:system-ui,sans-serif;background:#111;color:#eee;margin:0;padding:16px;text-align:center}
  h1{font-size:1.2rem;font-weight:500;opacity:.7}
  .card{background:#1e1e1e;border-radius:16px;padding:16px;margin:12px auto;max-width:360px}
  .val{font-size:2.6rem;font-weight:600}
  .lbl{opacity:.6;font-size:.9rem}
  #qual{font-size:1.4rem;font-weight:600}
</style>
</head>
<body>
<h1>Station météo intérieure</h1>
<div class="card"><div class="lbl">Température</div><div class="val"><span id="t">--</span> °C</div></div>
<div class="card"><div class="lbl">Pression</div><div class="val"><span id="p">--</span> hPa</div></div>
<div class="card">
  <div class="lbl">Qualité de l'air (VOC index)</div>
  <div class="val" id="v">--</div>
  <div id="qual">...</div>
</div>
<script>
function qualite(v){
  if(v==0)  return ["Calibrage en cours...","#aaa"];
  if(v<50)  return ["Excellent","#4caf50"];
  if(v<150) return ["Bon","#cddc39"];
  if(v<250) return ["Moyen","#ff9800"];
  if(v<400) return ["Mauvais","#f44336"];
  return ["Danger","#b71c1c"];
}
async function maj(){
  try{
    const d = await (await fetch('/data')).json();
    t.textContent = d.temp.toFixed(1);
    p.textContent = d.pres.toFixed(1);
    v.textContent = d.voc;
    const q = qualite(d.voc);
    qual.textContent = q[0];
    qual.style.color = q[1];
  }catch(e){ qual.textContent = "Connexion perdue"; qual.style.color = "#aaa"; }
}
maj(); setInterval(maj, 2000);
</script>
</body>
</html>
)rawliteral";

//Routes du serveur
void handleRoot() {
  server.send_P(200, "text/html; charset=utf-8", PAGE);
}

void handleData() {
  char buf[96];
  snprintf(buf, sizeof(buf), "{\"temp\":%.1f,\"pres\":%.1f,\"voc\":%d}",temp, pres, (int)voc);
  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "application/json", buf);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Wire.begin(21, 22);

  if (!bmp.begin(0x76)) {
    Serial.println("BMP280 non trouve !");
    while (1);
  }
  Serial.println("BMP280 OK");

  if (!sgp.begin()) {
    Serial.println("SGP40 non trouve !");
    while (1);
  }
  Serial.println("SGP40 OK");

  // Connexion au partage de connexion du téléphone
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.print("Connexion au partage");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connecte ! Adresse a taper sur le telephone : http://");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();
}

void loop() {
  // Reconnexion si le partage a été coupé
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.reconnect();
    delay(2000);
    return;
  }

  server.handleClient();

  // Le SGP40 doit être lu environ 1 fois par seconde
  if (millis() - lastRead >= 1000) {
    lastRead = millis();
    temp = bmp.readTemperature();
    pres = bmp.readPressure() / 100.0F;
    voc  = sgp.measureVocIndex(temp, 50);

    Serial.printf("Temp: %.1f C | Pres: %.1f hPa | VOC: %d\n", temp, pres, (int)voc);
  }
}