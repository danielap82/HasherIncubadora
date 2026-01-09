#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include "config.h"
#include "wifi_net.h"
#include "sensor.h"
#include "relay.h"
#include "mqtt_mod.h"
#include "manual.h"

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

// Autenticación unificada
const char* WEB_USER = "hasher";
const char* WEB_PASS = "proyectoincubadora2026";

#define OTA_USER "hasher"
#define OTA_PASS "proyectoincubadora2026"

bool checkAuth() {
  if (!server.authenticate(WEB_USER, WEB_PASS)) {
    server.requestAuthentication();
    return false;
  }
  return true;
}

// Prototipos
void handleRoot();
void handleConfig();
void handleSave();
void handleWiFiSave();
void handleManual();
void handleManualResumido();
void handleManualTecnico();
void handleManualUsuario();
void handleManualIngles();
void handleManualASCII();
void handleManualImpresion();
void handleApiStatus();
void handleApiSensor();
void handleApiHistory();
void handleRele();

void webInit() {
  server.on("/", handleRoot);
  server.on("/config", handleConfig);
  server.on("/save", handleSave);
  server.on("/wifisave", handleWiFiSave);

  // Centro de ayuda / manuales
  server.on("/manual", handleManual);
  server.on("/manual/resumido", handleManualResumido);
  server.on("/manual/tecnico", handleManualTecnico);
  server.on("/manual/usuario", handleManualUsuario);
  server.on("/manual/en", handleManualIngles);
  server.on("/manual/ascii", handleManualASCII);
  server.on("/manual/print", handleManualImpresion);

  // APIs
  server.on("/api/status", handleApiStatus);
  server.on("/api/sensor", handleApiSensor);
  server.on("/api/history", handleApiHistory);
  // ------------------------------------------------------
//  DEBUG ANTIGUO  (/debug)
// ------------------------------------------------------
server.on("/debug", HTTP_GET, []() {
    String out = "";

    float h = sensorGetHum();
    float t = sensorGetTemp();

    out += "Humedad actual: " + String(h, 2) + "%\n";
    out += "Temperatura actual: " + String(t, 2) + "°C\n";

    out += "\nHistorial:\n";
    out += "  Muestras: " + String(sensorGetHistoryCount()) + "\n";
    out += "  Intervalo: " + String(sensorGetSampleIntervalSeconds()) + " s\n";

    // Último riego (si lo tienes restaurado)
    extern float ultimaHumRiego;
    extern float ultimaTempRiego;
    extern unsigned long ultimoRiegoTimestamp;

    out += "\nÚltimo riego:\n";
    out += "  Humedad: " + String(ultimaHumRiego, 2) + "%\n";
    out += "  Temperatura: " + String(ultimaTempRiego, 2) + "°C\n";

    unsigned long hace = (millis() - ultimoRiegoTimestamp) / 1000;
    out += "  Hace: " + String(hace) + " s\n";

    server.send(200, "text/plain", out);
});

// ------------------------------------------------------
//  HISTORIAL ANTIGUO  (/history)
// ------------------------------------------------------
server.on("/history", HTTP_GET, []() {
    unsigned int count = sensorGetHistoryCount();
    
    // 1. Inicializa variables para calcular las sumas
    float totalTemp = 0;
    float totalHum = 0;

    // 2. Comienza a construir el JSON (la primera parte sigue igual)
    String json = "{";
    json += "\"count\":" + String(count) + ",";
    json += "\"interval\":" + String(sensorGetSampleIntervalSeconds()) + ",";

    // Temperaturas
    json += "\"temp\":[";
    for (unsigned int i = 0; i < count; i++) {
        float currentTemp = historyTemp[i]; // Guarda el valor actual
        totalTemp += currentTemp;            // Suma al total
        json += String(currentTemp, 2);
        if (i < count - 1) json += ",";
    }
    json += "],";

    // Humedades
    json += "\"hum\":[";
    for (unsigned int i = 0; i < count; i++) {
        float currentHum = historyHum[i]; // Guarda el valor actual
        totalHum += currentHum;            // Suma al total
        json += String(currentHum, 2);
        if (i < count - 1) json += ",";
    }
    json += "],"; // Nota: he añadido una coma aquí para el nuevo campo siguiente

    // 3. Calcula las medias
    float avgTemp = (count > 0) ? (totalTemp / count) : 0;
    float avgHum = (count > 0) ? (totalHum / count) : 0;

    // 4. Añade los nuevos campos de media al JSON
    // Usamos String(..., 2) para limitar a dos decimales en la salida JSON
    json += "\"temp_media\":" + String(avgTemp, 2) + ",";
    json += "\"hum_media\":" + String(avgHum, 2);
    
    json += "}";

    server.send(200, "application/json", json);
});


  // Control manual del relé
  server.on("/rele", handleRele);

  // OTA
  httpUpdater.setup(&server, "/update", OTA_USER, OTA_PASS);

  server.begin();
  Serial.println("Servidor web iniciado");
}

void webLoop() {
  server.handleClient();
}

// ------------------------------
// SIDEBAR (C2‑C)
// ------------------------------
String sidebar() {
  return String(
    "<div class='sidebar'>"
      "<div class='menu-item'><a href='/'><span class='icon'>🏠</span> Dashboard</a></div>"
      "<div class='menu-item'><a href='/config'><span class='icon'>⚙️</span> Configuración</a></div>"
      "<div class='menu-item'><a href='/manual'><span class='icon'>📘</span> Manuales</a></div>"
      "<div class='menu-item'><a href='/update'><span class='icon'>⬆️</span> OTA</a></div>"
      "<div class='menu-item'><a href='/api/status'><span class='icon'>📡</span> API</a></div>"
    "</div>"
  );
}

// ------------------------------
// ESTILOS
// ------------------------------
String estilos() {
  return String(
    "<style>"
    "body{margin:0;font-family:Arial;background:#121212;color:#e0e0e0;}"
    ".sidebar{position:fixed;left:0;top:0;width:200px;height:100%;background:#1e1e1e;padding-top:20px;box-shadow:2px 0 10px rgba(0,0,0,0.5);}"
    ".menu-item{padding:15px 20px;}"
    ".menu-item a{color:#e0e0e0;text-decoration:none;font-size:18px;display:flex;align-items:center;gap:10px;}"
    ".menu-item:hover{background:#333;}"
    ".icon{font-size:22px;}"
    ".content{margin-left:220px;padding:20px;}"
    ".card{background:#1e1e1e;padding:15px;border-radius:10px;margin-bottom:15px;border:1px solid #333;}"
    ".value{font-size:1.6em;font-weight:bold;}"
    "button{padding:10px 15px;border:none;border-radius:5px;background:#1976d2;color:white;cursor:pointer;margin-right:10px;}"
    "button:hover{background:#145ca0;}"
    "canvas{background:#1e1e1e;border:1px solid #333;border-radius:10px;}"

    /* === ESTILOS NUEVOS PARA EL BLOQUE DE ESTADÍSTICAS === */
    ".stats-block{"
      "margin-top:20px;"
      "padding:15px 18px;"
      "background:#1e1e1e;"
      "border-radius:10px;"
      "border:1px solid #333;"
    "}"
    ".stats-block h3{"
      "margin:0 0 12px 0;"
      "font-size:1.2em;"
      "color:#e0e0e0;"
    "}"
    ".stats-row{"
      "display:flex;"
      "justify-content:space-between;"
      "gap:20px;"
    "}"
    ".stat-item{"
      "display:flex;"
      "flex-direction:column;"
    "}"
    ".stat-label{"
      "font-size:0.9em;"
      "color:#b0b0b0;"
    "}"
    ".stat-value{"
      "font-size:1.3em;"
      "font-weight:bold;"
      "color:#e0e0e0;"
    "}"
    /* === FIN ESTILOS ESTADÍSTICAS === */

    "</style>"
  );
}

// ------------------------------
// DASHBOARD
// ------------------------------
void handleRoot() {
  if (!checkAuth()) return;

  String html = "<html><head><meta charset='UTF-8'><title>Incubadora Hasher</title>";
  html += estilos();
  html += "<script src='https://cdn.jsdelivr.net/npm/chart.js'></script>";
  html += "</head><body>";

  html += sidebar();
  html += "<div class='content'>";

  html += "<h1>Dashboard</h1>";

  html += "<div class='card'><b>Temperatura:</b> <span id='tempValue' class='value'>-- °C</span></div>";
  html += "<div class='card'><b>Humedad:</b> <span id='humValue' class='value'>-- %</span></div>";
  html += "<div class='card'><b>Umbral:</b> <span id='umbralValue'>-- %</span></div>";
  html += "<div class='card'><b>Intervalo:</b> <span id='intervalValue'>-- s</span></div>";
  html += "<div class='card'><b>Tiempo ON:</b> <span id='tiempoValue'>-- ms</span></div>";
  html += "<div class='card'><b>Tiempo desde último riego:</b> <span id='riegoTime'>-- s</span></div>";
  html += "<div class='card'><b>Relé:</b> <span id='releState'>--</span></div>";
  html += "<div class='stats-block'>";
  html += "  <h3>Estadísticas</h3>";
  html += "  <div class='stats-row'>";
  html += "    <div class='stat-item'>";
  html += "      <span class='stat-label'>Temperatura media</span>";
  html += "      <span class='stat-value' id='temp_media'>-- ºC</span>";
  html += "    </div>";
  html += "    <div class='stat-item'>";
  html += "      <span class='stat-label'>Humedad media</span>";
  html += "      <span class='stat-value' id='hum_media'>-- %</span>";
  html += "    </div>";
  html += "  </div>";
  html += "</div>";

  html += "<button onclick='controlRele(\"ON\")'>Relé ON</button>";
  html += "<button onclick='controlRele(\"OFF\")'>Relé OFF</button>";

  html += "<h2>Histórico</h2>";
  html += "<canvas id='historyChart' width='400' height='200'></canvas>";

  html += "</div>";

  // JS
  html += "<script>";
  html += "let chart=null;";
  html += "function updateStatus(){fetch('/api/status').then(r=>r.json()).then(d=>{";
  html += "document.getElementById('tempValue').innerText=d.temp.toFixed(1)+' °C';";
  html += "document.getElementById('humValue').innerText=d.hum.toFixed(1)+' %';";
  html += "document.getElementById('umbralValue').innerText=d.umbral.toFixed(1)+' %';";
  html += "document.getElementById('intervalValue').innerText=(d.tiempoEspera/1000)+' s';";
  html += "document.getElementById('tiempoValue').innerText=d.tiempoRiego+' ms';";
  html += "document.getElementById('riegoTime').innerText=d.tiempoDesdeRiego+' s';";
  html += "document.getElementById('releState').innerText=d.releOn?'ON':'OFF';";
  html += "});}";
  html += "function updateHistory(){";
  html += "fetch('/history').then(r=>r.json()).then(d=>{";
  html += "const labels=new Array(d.count).fill('');";
  html += "if(chart) chart.destroy();";
  html += "chart=new Chart(document.getElementById('historyChart'),{";
  html += "type:'line',";
  html += "data:{labels:labels,datasets:[";
  html += "{label:'Temp',data:d.temp,borderColor:'red',backgroundColor:'rgba(255,0,0,0.2)'},";
  html += "{label:'Hum',data:d.hum,borderColor:'cyan',backgroundColor:'rgba(0,255,255,0.2)'}";
  html += "]}});";
  html += "document.getElementById('temp_media').textContent = d.temp_media.toFixed(1) + ' ºC';";
  html += "document.getElementById('hum_media').textContent  = d.hum_media.toFixed(1) + ' %';";
  html += "});";
  html += "}";
  html += "function controlRele(cmd){fetch('/rele?cmd='+cmd).then(()=>updateStatus());}";
  html += "updateStatus();updateHistory();setInterval(updateStatus,3000);setInterval(updateHistory,15000);";
  html += "</script>";

  html += "</body></html>";

  server.send(200, "text/html", html);
}

// ------------------------------
// CONFIGURACIÓN
// ------------------------------
void handleConfig() {
  if (!checkAuth()) return;

  String html = "<html><head><meta charset='UTF-8'><title>Configuración</title>";
  html += estilos();
  html += "</head><body>";

  html += sidebar();
  html += "<div class='content'>";

  html += "<h1>Configuración</h1>";

  html += "<form action='/wifisave'>";
  html += "<h2>WiFi</h2>";
  html += "SSID:<br><input name='ssid' value='" + String(cfg.ssid) + "'><br>";
  html += "Password:<br><input name='pass' value='" + String(cfg.pass) + "'><br>";

  html += "<h2>IP fija</h2>";
  html += "IP:<br><input name='ip' value='" + String(cfg.ip) + "'><br>";
  html += "Gateway:<br><input name='gateway' value='" + String(cfg.gateway) + "'><br>";
  html += "Subnet:<br><input name='subnet' value='" + String(cfg.subnet) + "'><br>";
  html += "DNS:<br><input name='dns' value='" + String(cfg.dns) + "'><br>";

  html += "<button type='submit'>Guardar WiFi</button>";
  html += "</form><hr>";

  html += "<form action='/save'>";
  html += "<h2>Control</h2>";
  html += "Umbral (%):<br><input name='umbral' value='" + String(cfg.umbralHumedad) + "'><br>";
  html += "Intervalo (ms):<br><input name='espera' value='" + String(cfg.tiempoEspera) + "'><br>";
  html += "Tiempo ON (ms):<br><input name='riego' value='" + String(cfg.tiempoRiego) + "'><br>";
  html += "<button type='submit'>Guardar</button>";
  html += "</form><hr>";

  html += "<form action='/save'>";
  html += "<h2>MQTT</h2>";
  html += "Host:<br><input name='mqttHost' value='" + String(cfg.mqttHost) + "'><br>";
  html += "Puerto:<br><input name='mqttPort' value='" + String(cfg.mqttPort) + "'><br>";
  html += "Usuario:<br><input name='mqttUser' value='" + String(cfg.mqttUser) + "'><br>";
  html += "Password:<br><input name='mqttPass' value='" + String(cfg.mqttPass) + "'><br>";
  html += "<button type='submit'>Guardar MQTT</button>";
  html += "</form>";

  html += "</div></body></html>";

  server.send(200, "text/html", html);
}

// ------------------------------
// GUARDAR CONFIG
// ------------------------------
void handleSave() {
  if (!checkAuth()) return;

  if (server.hasArg("umbral")) {
    cfg.umbralHumedad = server.arg("umbral").toFloat();
    cfg.tiempoEspera  = server.arg("espera").toInt();
    cfg.tiempoRiego   = server.arg("riego").toInt();
  }

  if (server.hasArg("mqttHost")) {
    strncpy(cfg.mqttHost, server.arg("mqttHost").c_str(), sizeof(cfg.mqttHost));
    cfg.mqttPort = server.arg("mqttPort").toInt();
    strncpy(cfg.mqttUser, server.arg("mqttUser").c_str(), sizeof(cfg.mqttUser));
    strncpy(cfg.mqttPass, server.arg("mqttPass").c_str(), sizeof(cfg.mqttPass));
  }

  configSave();
  server.sendHeader("Location", "/config");
  server.send(303);
}

// ------------------------------
// GUARDAR WIFI
// ------------------------------
void handleWiFiSave() {
  if (!checkAuth()) return;

  strcpy(cfg.ssid, server.arg("ssid").c_str());
  strcpy(cfg.pass, server.arg("pass").c_str());
  strcpy(cfg.ip, server.arg("ip").c_str());
  strcpy(cfg.gateway, server.arg("gateway").c_str());
  strcpy(cfg.subnet, server.arg("subnet").c_str());
  strcpy(cfg.dns, server.arg("dns").c_str());

  configSave();

  server.send(200, "text/html",
              "<h2>Guardado</h2><p>Reiniciando...</p>");

  delay(1000);
  ESP.restart();
}

// ------------------------------
// API STATUS
// ------------------------------
void handleApiStatus() {
  if (!checkAuth()) return;

  String json = "{";
  json += "\"temp\":" + String(sensorGetTemp(), 2) + ",";
  json += "\"hum\":" + String(sensorGetHum(), 2) + ",";
  json += "\"umbral\":" + String(cfg.umbralHumedad, 2) + ",";
  json += "\"tiempoEspera\":" + String(cfg.tiempoEspera) + ",";
  json += "\"tiempoRiego\":" + String(cfg.tiempoRiego) + ",";
  json += "\"tiempoDesdeRiego\":" + String(relayGetSecondsSinceActivation()) + ",";
  json += "\"releOn\":" + String(relayIsOn() ? "true" : "false") + ",";
  json += "\"ssid\":\"" + String(cfg.ssid) + "\",";
  json += "\"ip\":\"" + wifiGetIPString() + "\",";
  json += "\"uptime\":" + String(millis()/1000);
  json += "}";

  server.send(200, "application/json", json);
}

// ------------------------------
// API SENSOR
// ------------------------------
void handleApiSensor() {
  if (!checkAuth()) return;

  String json = "{";
  json += "\"temp\":" + String(sensorGetTemp(), 2) + ",";
  json += "\"hum\":" + String(sensorGetHum(), 2);
  json += "}";

  server.send(200, "application/json", json);
}

// ------------------------------
// API HISTORIAL
// ------------------------------
void handleApiHistory() {
  if (!checkAuth()) return;

  extern float historyTemp[HISTORY_SIZE];
  extern float historyHum[HISTORY_SIZE];

  unsigned int count = sensorGetHistoryCount();

  String json = "{";
  json += "\"interval\":" + String(sensorGetSampleIntervalSeconds()) + ",";
  json += "\"count\":" + String(count) + ",";
  
  json += "}";

  server.send(200, "application/json", json);
}

// ------------------------------
// CONTROL MANUAL DEL RELÉ
// ------------------------------
void handleRele() {
  if (!checkAuth()) return;

  if (!server.hasArg("cmd")) {
    server.send(400, "text/plain", "Falta cmd");
    return;
  }

  String cmd = server.arg("cmd");
  cmd.toUpperCase();

  if (cmd == "ON") {
    relaySet(true);
    server.send(200, "text/plain", "OK ON");
  } else if (cmd == "OFF") {
    relaySet(false);
    server.send(200, "text/plain", "OK OFF");
  } else {
    server.send(400, "text/plain", "Comando no válido");
  }
}

// ------------------------------
// MANUALES
// ------------------------------
void handleManual() {
  if (!checkAuth()) return;
  server.send(200, "text/html", manualCompleto());
}

void handleManualResumido() {
  if (!checkAuth()) return;
  server.send(200, "text/html", manualResumido());
}

void handleManualTecnico() {
  if (!checkAuth()) return;
  server.send(200, "text/html", manualTecnico());
}

void handleManualUsuario() {
  if (!checkAuth()) return;
  server.send(200, "text/html", manualUsuario());
}

void handleManualIngles() {
  if (!checkAuth()) return;
  server.send(200, "text/html", manualIngles());
}

void handleManualASCII() {
  if (!checkAuth()) return;
  server.send(200, "text/plain", manualASCII());
}

void handleManualImpresion() {
  if (!checkAuth()) return;
  server.send(200, "text/html", manualImpresion());
}