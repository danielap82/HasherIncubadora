#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include "config.h"
#include "wifi_net.h"
#include "sensor.h"
#include "relay.h"
#include "mqtt_mod.h"

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

#define OTA_USER "admin"
#define OTA_PASS "incubadora123"

void handleRoot();
void handleConfig();
void handleSave();
void handleWiFiSave();
void handleManual();
void handleApiStatus();
void handleApiSensor();
void handleApiHistory();

void webInit() {
  server.on("/", handleRoot);
  server.on("/config", handleConfig);
  server.on("/save", handleSave);
  server.on("/wifisave", handleWiFiSave);
  server.on("/manual", handleManual);

  server.on("/api/status", handleApiStatus);
  server.on("/api/sensor", handleApiSensor);
  server.on("/api/history", handleApiHistory);

  httpUpdater.setup(&server, "/update", OTA_USER, OTA_PASS);

  server.begin();
  Serial.println("Servidor web iniciado");
}

void webLoop() {
  server.handleClient();
}

// ------------------------------
// PÁGINA PRINCIPAL
// ------------------------------
void handleRoot() {
  String html = "<html><head><meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>Incubadora Hasher</title>";
  html += "<script src='https://cdn.jsdelivr.net/npm/chart.js'></script>";
  html += "<style>";
  html += "body{font-family:Arial,Helvetica,sans-serif;background:#eef;margin:0;padding:0;}";
  html += ".container{max-width:900px;margin:20px auto;padding:20px;background:#fff;border-radius:10px;}";
  html += "h2{text-align:center;margin-top:0;}";
  html += ".grid{display:flex;flex-wrap:wrap;gap:10px;}";
  html += ".card{flex:1;min-width:150px;background:#f5f5f5;padding:10px;border-radius:8px;}";
  html += ".label{font-size:0.9em;color:#555;}";
  html += ".value{font-size:1.4em;font-weight:bold;}";
  html += ".buttons{margin-top:15px;text-align:center;}";
  html += "button{margin:5px;padding:8px 12px;border:none;border-radius:5px;background:#1976d2;color:#fff;cursor:pointer;}";
  html += "button:hover{background:#145ca0;}";
  html += "canvas{max-width:100%;height:auto;}";
  html += "</style></head><body>";
  html += "<div class='container'>";
  html += "<h2>Incubadora Hasher</h2>";

  html += "<div class='grid'>";
  html += "<div class='card'><div class='label'>Temperatura actual</div><div class='value' id='tempValue'>--.- °C</div></div>";
  html += "<div class='card'><div class='label'>Humedad actual</div><div class='value' id='humValue'>--.- %</div></div>";
  html += "<div class='card'><div class='label'>Umbral humedad</div><div class='value' id='umbralValue'>-- %</div></div>";
  html += "<div class='card'><div class='label'>Intervalo control</div><div class='value' id='intervalValue'>-- s</div></div>";
  html += "<div class='card'><div class='label'>Tiempo activación</div><div class='value' id='tiempoValue'>-- ms</div></div>";
  html += "<div class='card'><div class='label'>Modo</div><div class='value' id='modoValue'>--</div></div>";
  html += "</div>";

  html += "<div class='buttons'>";
  html += "<a href='/config'><button>Configuración</button></a>";
  html += "<a href='/update'><button>Actualizar firmware</button></a>";
  html += "<a href='/manual'><button>Manual / Ayuda</button></a>";
  html += "</div>";

  html += "<h3>Histórico (últimos ~100 minutos)</h3>";
  html += "<canvas id='historyChart'></canvas>";

  html += "</div>";

  html += "<script>";
  html += "let chart;";
  html += "function createChart(labels, tempData, humData){";
  html += " const ctx=document.getElementById('historyChart').getContext('2d');";
  html += " if(chart){chart.destroy();}";
  html += " chart=new Chart(ctx,{type:'line',data:{";
  html += "   labels:labels,";
  html += "   datasets:[";
  html += "     {label:'Temp (°C)',data:tempData,borderColor:'rgba(255,99,132,1)',backgroundColor:'rgba(255,99,132,0.2)',yAxisID:'y1'},";
  html += "     {label:'Humedad (%)',data:humData,borderColor:'rgba(54,162,235,1)',backgroundColor:'rgba(54,162,235,0.2)',yAxisID:'y2'}";
  html += "   ]},";
  html += "   options:{responsive:true,interaction:{mode:'index',intersect:false},";
  html += "     stacked:false,plugins:{legend:{position:'top'}},";
  html += "     scales:{";
  html += "       y1:{type:'linear',position:'left',title:{display:true,text:'Temp (°C)'}},";
  html += "       y2:{type:'linear',position:'right',title:{display:true,text:'Humedad (%)'},grid:{drawOnChartArea:false}}";
  html += "     }";
  html += "   }";
  html += " });";
  html += "}";

  html += "function updateStatus(){";
  html += " fetch('/api/status').then(r=>r.json()).then(d=>{";
  html += "   document.getElementById('tempValue').innerText=d.temp.toFixed(1)+' °C';";
  html += "   document.getElementById('humValue').innerText=d.hum.toFixed(1)+' %';";
  html += "   document.getElementById('umbralValue').innerText=d.umbral.toFixed(1)+' %';";
  html += "   document.getElementById('intervalValue').innerText=(d.tiempoEspera/1000)+' s';";
  html += "   document.getElementById('tiempoValue').innerText=d.tiempoRiego+' ms';";
  html += "   document.getElementById('modoValue').innerText=d.modoAP ? 'AP/configuración' : 'Normal';";
  html += " }).catch(e=>console.log(e));";
  html += "}";

  html += "function updateHistory(){";
  html += " fetch('/api/history').then(r=>r.json()).then(d=>{";
  html += "   const labels=[];";
  html += "   for(let i=0;i<d.count;i++){labels.push('');}";
  html += "   createChart(labels,d.t,d.h);";
  html += " }).catch(e=>console.log(e));";
  html += "}";

  html += "updateStatus();";
  html += "updateHistory();";
  html += "setInterval(updateStatus,3000);";
  html += "setInterval(updateHistory,15000);";
  html += "</script>";

  html += "</body></html>";

  server.send(200, "text/html", html);
}

// ------------------------------
// PÁGINA CONFIGURACIÓN
// ------------------------------
void handleConfig() {
  String html = "<html><head><meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>Configuración Incubadora Hasher</title>";
  html += "<style>";
  html += "body{font-family:Arial;background:#eef;padding:20px;margin:0;}";
  html += ".container{max-width:600px;margin:20px auto;background:#fff;padding:20px;border-radius:10px;}";
  html += "h2{text-align:center;margin-top:0;}";
  html += "input{width:100%;padding:8px;margin:5px 0;box-sizing:border-box;}";
  html += "input[type=submit]{width:auto;}";
  html += "hr{margin:20px 0;}";
  html += "</style></head><body>";
  html += "<div class='container'><h2>Config. Incubadora Hasher</h2>";

  html += "<form action='/wifisave'>";
  html += "<h3>Red WiFi</h3>";
  html += "SSID:<br><input type='text' name='ssid' value='" + String(cfg.ssid) + "'><br>";
  html += "Password:<br><input type='text' name='pass' value='" + String(cfg.pass) + "'><br><br>";

  html += "<h3>IP fija (opcional)</h3>";
  html += "IP:<br><input type='text' name='ip' value='" + String(cfg.ip) + "'><br>";
  html += "Gateway:<br><input type='text' name='gateway' value='" + String(cfg.gateway) + "'><br>";
  html += "Subnet:<br><input type='text' name='subnet' value='" + String(cfg.subnet) + "'><br>";
  html += "DNS:<br><input type='text' name='dns' value='" + String(cfg.dns) + "'><br>";

  html += "<input type='submit' value='Guardar WiFi y red'>";
  html += "</form><hr>";

  html += "<form action='/save'>";
  html += "<h3>Parámetros de control</h3>";
  html += "Umbral humedad (%):<br><input type='number' step='0.1' name='umbral' value='" + String(cfg.umbralHumedad) + "'><br>";
  html += "Intervalo control (ms):<br><input type='number' name='espera' value='" + String(cfg.tiempoEspera) + "'><br>";
  html += "Tiempo activación (ms):<br><input type='number' name='riego' value='" + String(cfg.tiempoRiego) + "'><br>";
  html += "<input type='submit' value='Guardar parámetros'>";
  html += "</form><hr>";

  html += "<form action='/save'>";
  html += "<h3>MQTT</h3>";
  html += "Host MQTT:<br><input type='text' name='mqttHost' value='" + String(cfg.mqttHost) + "'><br>";
  html += "Puerto MQTT:<br><input type='number' name='mqttPort' value='" + String(cfg.mqttPort) + "'><br>";
  html += "Usuario MQTT:<br><input type='text' name='mqttUser' value='" + String(cfg.mqttUser) + "'><br>";
  html += "Password MQTT:<br><input type='text' name='mqttPass' value='" + String(cfg.mqttPass) + "'><br>";
  html += "<small>Deja host vacío para desactivar MQTT.</small><br>";
  html += "<input type='submit' value='Guardar MQTT'>";
  html += "</form>";

  html += "</div></body></html>";

  server.send(200, "text/html", html);
}

void handleSave() {
  if (server.hasArg("umbral")) {
    cfg.umbralHumedad = server.arg("umbral").toFloat();
    cfg.tiempoEspera  = server.arg("espera").toInt();
    cfg.tiempoRiego   = server.arg("riego").toInt();
  }

  if (server.hasArg("mqttHost")) {
    String hst = server.arg("mqttHost");
    String usr = server.arg("mqttUser");
    String pwd = server.arg("mqttPass");
    uint16_t prt = server.arg("mqttPort").toInt();

    strncpy(cfg.mqttHost, hst.c_str(), sizeof(cfg.mqttHost) - 1);
    cfg.mqttHost[sizeof(cfg.mqttHost) - 1] = 0;

    strncpy(cfg.mqttUser, usr.c_str(), sizeof(cfg.mqttUser) - 1);
    cfg.mqttUser[sizeof(cfg.mqttUser) - 1] = 0;

    strncpy(cfg.mqttPass, pwd.c_str(), sizeof(cfg.mqttPass) - 1);
    cfg.mqttPass[sizeof(cfg.mqttPass) - 1] = 0;

    cfg.mqttPort = prt > 0 ? prt : 1883;
  }

  configSave();

  server.sendHeader("Location", "/config");
  server.send(303);
}

void handleWiFiSave() {
  strcpy(cfg.ssid, server.arg("ssid").c_str());
  strcpy(cfg.pass, server.arg("pass").c_str());

  strcpy(cfg.ip,      server.arg("ip").c_str());
  strcpy(cfg.gateway, server.arg("gateway").c_str());
  strcpy(cfg.subnet,  server.arg("subnet").c_str());
  strcpy(cfg.dns,     server.arg("dns").c_str());

  configSave();

  server.send(200, "text/html",
              "<h2>Configuración guardada</h2>"
              "<p>La Incubadora Hasher se reiniciará y tratará de conectarse a la WiFi configurada.</p>");

  delay(1000);
  ESP.restart();
}

// ------------------------------
// API REST
// ------------------------------
void handleApiStatus() {
  String json = "{";

  json += "\"temp\":" + String(sensorGetTemp(), 2) + ",";
  json += "\"hum\":" + String(sensorGetHum(), 2) + ",";
  json += "\"umbral\":" + String(cfg.umbralHumedad, 2) + ",";
  json += "\"tiempoEspera\":" + String(cfg.tiempoEspera) + ",";
  json += "\"tiempoRiego\":" + String(cfg.tiempoRiego) + ",";
  json += "\"modoAP\":" + String(wifiIsAPMode() ? "true" : "false") + ",";
  json += "\"ssid\":\"" + String(cfg.ssid) + "\",";
  json += "\"ip\":\"" + wifiGetIPString() + "\",";
  json += "\"uptime\":" + String(millis()/1000);

  json += "}";

  server.send(200, "application/json", json);
}

void handleApiSensor() {
  String json = "{";
  json += "\"temp\":" + String(sensorGetTemp(), 2) + ",";
  json += "\"hum\":" + String(sensorGetHum(), 2);
  json += "}";
  server.send(200, "application/json", json);
}

void handleApiHistory() {
  String json = "{";

  json += "\"interval\":" + String(sensorGetSampleIntervalSeconds()) + ",";

  unsigned int count = sensorGetHistoryCount();
  json += "\"count\":" + String(count) + ",";

  json += "\"t\":[";
  if (count > 0) {
    for (unsigned int i = 0; i < count; i++) {
      unsigned int idx;
      if (count == HISTORY_SIZE) {
        extern unsigned int historyIndex;
        idx = (historyIndex + i) % HISTORY_SIZE;
      } else {
        idx = i;
      }
      json += String(historyTemp[idx], 2);
      if (i < count - 1) json += ",";
    }
  }
  json += "],";

  json += "\"h\":[";
  if (count > 0) {
    for (unsigned int i = 0; i < count; i++) {
      unsigned int idx;
      if (count == HISTORY_SIZE) {
        extern unsigned int historyIndex;
        idx = (historyIndex + i) % HISTORY_SIZE;
      } else {
        idx = i;
      }
      json += String(historyHum[idx], 2);
      if (i < count - 1) json += ",";
    }
  }
  json += "]";

  json += "}";

  server.send(200, "application/json", json);
}

// ------------------------------
// MANUAL / AYUDA
// ------------------------------
void handleManual() {
  String html = "<html><head><meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>Manual Incubadora Hasher</title>";
  html += "<style>";
  html += "body{font-family:Arial;background:#eef;margin:0;padding:0;}";
  html += ".container{max-width:900px;margin:20px auto;background:#fff;padding:20px;border-radius:10px;}";
  html += "h2{text-align:center;margin-top:0;}";
  html += "h3{margin-top:30px;color:#1976d2;}";
  html += "p,li{font-size:15px;line-height:1.5;}";
  html += ".section{margin-bottom:40px;}";
  html += "button{padding:8px 12px;background:#1976d2;color:#fff;border:none;border-radius:5px;cursor:pointer;}";
  html += "button:hover{background:#145ca0;}";
  html += "</style></head><body>";

  html += "<div class='container'>";
  html += "<h2>Manual de la Incubadora Hasher</h2>";

  // Usuario final
  html += "<div class='section'>";
  html += "<h3>📘 Manual para Usuario Final</h3>";

  html += "<p>La Incubadora Hasher es un sistema automático que controla la humedad interna mediante un sensor SHT4x y un actuador conectado a un relé. Su objetivo es mantener condiciones estables para incubación.</p>";

  html += "<h4>Cómo conectarse</h4>";
  html += "<ul>";
  html += "<li>Si el dispositivo está conectado a tu WiFi, accede a su IP (por ejemplo: 192.168.1.199 o la que tengas configurada).</li>";
  html += "<li>Si no está conectado, crea su propia red WiFi llamada <b>Incubadora-Hasher</b>.</li>";
  html += "<li>Conéctate a esa red y entra en <b>http://192.168.4.1</b> desde el navegador.</li>";
  html += "</ul>";

  html += "<h4>Configuración básica</h4>";
  html += "<ul>";
  html += "<li><b>Umbral de humedad:</b> nivel mínimo de humedad antes de activar el actuador.</li>";
  html += "<li><b>Intervalo de control:</b> cada cuánto tiempo comprueba la humedad.</li>";
  html += "<li><b>Tiempo de activación:</b> cuánto tiempo permanece activado el relé cuando se dispara.</li>";
  html += "</ul>";

  html += "<h4>Actualización del firmware</h4>";
  html += "<p>Entra en la opción <b>Actualizar firmware</b> o ve a <b>/update</b>. Usuario: <b>admin</b>, contraseña: <b>incubadora123</b>. Selecciona el archivo .bin y espera a que reinicie.</p>";

  html += "<h4>Gráficas</h4>";
  html += "<p>La página principal muestra una gráfica con las últimas ~100 minutos de temperatura y humedad. Sirve para ver la estabilidad de la incubadora.</p>";

  html += "</div>";

  // Técnico avanzado
  html += "<div class='section'>";
  html += "<h3>🛠️ Manual Técnico Avanzado</h3>";

  html += "<h4>Esquema de conexiones</h4>";
  html += "<pre style='background:#f0f0f0;padding:10px;border-radius:8px;overflow-x:auto;'>";
  html += "                ┌──────────────────────────┐\n";
  html += "                │        ESP8266 (NodeMCU)  │\n";
  html += "                └──────────────────────────┘\n";
  html += "                       │           │\n";
  html += "                       │ I2C       │\n";
  html += "                       ▼           ▼\n";
  html += "                ┌──────────┐   ┌──────────┐\n";
  html += "                │   D1 (SCL)│──▶│ SCL SHT4x│\n";
  html += "                │   D2 (SDA)│──▶│ SDA      │\n";
  html += "                └──────────┘   └──────────┘\n";
  html += "\n";
  html += "Alimentación sensor SHT4x:\n";
  html += "   3V3  ─────────▶  VCC\n";
  html += "   GND  ─────────▶  GND\n";
  html += "\n";
  html += "Relé (activo en LOW):\n";
  html += "   D6 (GPIO12) ─────▶ IN del módulo relé\n";
  html += "   3V3          ─────▶ VCC del relé\n";
  html += "   GND          ─────▶ GND del relé\n";
  html += "\n";
  html += "Alimentación general:\n";
  html += "   USB 5V ─────▶ Vin del ESP8266\n";
  html += "   GND    ─────▶ GND común\n";
  html += "\n";
  html += "Notas:\n";
  html += " - El relé debe ser compatible con señal lógica (activo en LOW).\n";
  html += " - El SHT4x funciona directamente a 3.3V.\n";
  html += " - No conectar cargas de 230V sin aislamiento adecuado.\n";
  html += "</pre>";

  html += "<h4>MQTT</h4>";
  html += "<ul>";
  html += "<li>Topic base: <b>incubadora_hasher</b>.</li>";
  html += "<li>Publicación de estado: <b>incubadora_hasher/estado/temp</b>, <b>incubadora_hasher/estado/hum</b>, <b>incubadora_hasher/estado/rele</b>, <b>incubadora_hasher/estado/mode</b>.</li>";
  html += "<li>Control remoto: <b>incubadora_hasher/control/umbral</b>, <b>.../intervalo</b>, <b>.../tiempo</b>, <b>.../rele</b> (ON/OFF).</li>";
  html += "<li>Autodiscovery Home Assistant: sensores <b>Incubadora Temp</b> e <b>Incubadora Hum</b>.</li>";
  html += "</ul>";

  html += "<h4>Control automático</h4>";
  html += "<p>Cada <i>tiempoEspera</i> milisegundos se compara la humedad actual con el umbral configurado. Si la humedad es inferior al umbral, se activa el relé durante <i>tiempoRiego</i> milisegundos.</p>";

  html += "<h4>Watchdog del sensor</h4>";
  html += "<p>Si durante más de 3 minutos no se reciben lecturas válidas del SHT4x (valores 0, NaN o fuera de rango), el dispositivo se reinicia automáticamente para intentar recuperar el sensor.</p>";

  html += "</div>";

  html += "<div style='text-align:center;'><a href='/'><button>Volver</button></a></div>";

  html += "</div></body></html>";

  server.send(200, "text/html", html);
}