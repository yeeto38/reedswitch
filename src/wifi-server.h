#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "reed-switch.h"

/*
  WiFi AP + WebServer wrapper
  - Hosts a simple webpage showing reed state + elapsed time
  - Background turns red when reed state is 0 (paused)
  - Page auto-refreshes via JS polling /status
*/

class WifiServerAP {
public:
  WifiServerAP(const char* ssid, const char* password, uint16_t port = 80)
    : _ssid(ssid), _password(password), _server(port) {}

  void begin(ReedSwitch* reed) {
    _reed = reed;

    WiFi.mode(WIFI_AP);
    bool ok = WiFi.softAP(_ssid, _password);
    if (!ok) {
      Serial.println("[WIFI] Failed to start AP");
      while (true) delay(1000);
    }

    _apIP = WiFi.softAPIP();
    Serial.print("[WIFI] AP SSID: ");
    Serial.println(_ssid);
    Serial.print("[WIFI] AP IP: ");
    Serial.println(_apIP);

    // Routes
    _server.on("/", [this]() { handleRoot(); });
    _server.on("/status", [this]() { handleStatus(); });

    _server.begin();
    Serial.println("[WIFI] Web server started");
  }

  void loop() {
    _server.handleClient();
  }

  IPAddress apIP() const { return _apIP; }

private:
  const char* _ssid;
  const char* _password;
  WebServer _server;
  IPAddress _apIP;

  ReedSwitch* _reed = nullptr;

  void handleRoot() {
    // Simple page with polling to /status every 250ms
    // Background turns red when state==0, normal when state==1
    String html;
    html.reserve(2500);

    html += "<!DOCTYPE html><html><head><meta charset='utf-8'>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
    html += "<title>Hive Node</title>";
    html += "<style>";
    html += "body{font-family:Arial, sans-serif; margin:20px; transition:background 0.2s;}";
    html += ".card{padding:16px; border-radius:12px; background:rgba(255,255,255,0.85); max-width:520px;}";
    html += ".label{color:#333; font-weight:600;}";
    html += ".value{font-size:1.2em; margin:6px 0 12px 0;}";
    html += ".small{color:#444; font-size:0.95em;}";
    html += "</style></head><body>";
    html += "<div class='card'>";
    html += "<h2>ESP32-S3 Hive Node (AP)</h2>";
    html += "<div class='small'>Connect to Wi-Fi <b>";
    html += _ssid;
    html += "</b> then open <b>http://192.168.4.1/</b></div><hr>";

    html += "<div><span class='label'>Reed state:</span> <span id='state' class='value'>--</span></div>";
    html += "<div><span class='label'>Time stable at state=1:</span> <span id='elapsed' class='value'>--</span></div>";
    html += "<div class='small'>When state becomes 0 (magnet present), timer pauses and background turns red. ";
    html += "When state returns to 1, timer resets to 0 and starts again.</div>";

    html += "</div>";

    html += "<script>";
    html += "function msToHuman(ms){";
    html += "  ms=Math.max(0, ms|0);";
    html += "  let s=Math.floor(ms/1000);";
    html += "  let h=Math.floor(s/3600); s%=3600;";
    html += "  let m=Math.floor(s/60); s%=60;";
    html += "  if(h>0) return `${h}h ${m}m ${s}s`;";
    html += "  if(m>0) return `${m}m ${s}s`;";
    html += "  return `${s}s`;";
    html += "}";
    html += "async function poll(){";
    html += "  try{";
    html += "    const r=await fetch('/status');";
    html += "    const j=await r.json();";
    html += "    document.getElementById('state').textContent = j.state;";
    html += "    document.getElementById('elapsed').textContent = msToHuman(j.elapsedMs);";
    html += "    if(j.state===0){ document.body.style.background='#ff3b3b'; }";
    html += "    else { document.body.style.background='#e8f5e9'; }";
    html += "  }catch(e){}";
    html += "}";
    html += "setInterval(poll, 250); poll();";
    html += "</script>";

    html += "</body></html>";

    _server.send(200, "text/html", html);
  }

  void handleStatus() {
    if (!_reed) {
      _server.send(500, "application/json", "{\"error\":\"reed not set\"}");
      return;
    }

    int state = _reed->getState();
    uint32_t elapsedMs = _reed->getElapsedMs();

    String json;
    json.reserve(100);
    json += "{";
    json += "\"state\":";
    json += state;
    json += ",\"elapsedMs\":";
    json += elapsedMs;
    json += "}";

    _server.send(200, "application/json", json);
  }
};
