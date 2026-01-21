#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

/* ===== AP Settings ===== */
const char* ap_ssid = "HiveNode-ESP32";
const char* ap_password = "hivepassword"; // must be >= 8 chars, or use NULL for open

WebServer server(80);

unsigned long lastPrint = 0;
uint32_t aliveCount = 0;

void handleRoot() {
  String page = "<!DOCTYPE html><html><head><meta charset='utf-8'>";
  page += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  page += "<title>Hive Node AP</title></head><body>";
  page += "<h1>ESP32-S3 Hive Node (AP Mode)</h1>";
  page += "<p>Status: <b>ALIVE</b></p>";
  page += "<p>Alive count: " + String(aliveCount) + "</p>";
  page += "<p>AP SSID: " + String(ap_ssid) + "</p>";
  page += "<p>Connect laptop to Wi-Fi <b>" + String(ap_ssid) + "</b>, then open <b>http://192.168.4.1</b></p>";
  page += "</body></html>";

  server.send(200, "text/html", page);
}

void handleHealth() {
  // Useful for future sensor JSON endpoints
  String json = "{";
  json += "\"status\":\"alive\",";
  json += "\"aliveCount\":" + String(aliveCount);
  json += "}";
  server.send(200, "application/json", json);
}

void setup() {
  delay(3000); // allow USB CDC enumeration
  Serial.begin(115200);
  Serial.println("BOOT OK");

  /* ===== Start Access Point ===== */
  WiFi.mode(WIFI_AP);
  bool ok = WiFi.softAP(ap_ssid, ap_password);

  if (!ok) {
    Serial.println("Failed to start AP!");
    while (true) delay(1000);
  }

  IPAddress apIP = WiFi.softAPIP();

  Serial.println("AP started!");
  Serial.print("SSID: ");
  Serial.println(ap_ssid);
  Serial.print("AP IP: ");
  Serial.println(apIP); // usually 192.168.4.1

  /* ===== Web Routes ===== */
  server.on("/", handleRoot);
  server.on("/health", handleHealth);
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  server.handleClient();

  if (millis() - lastPrint >= 1000) {
    lastPrint = millis();
    aliveCount++;
    Serial.println("alive");
  }
}
