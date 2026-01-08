#include <ESP8266WebServer.h>
#include "sensor.h"
#include "relay.h"
#include "config.h"

ESP8266WebServer server(80);

void webInit() {

    // -------------------------
    // /history (antiguo)
    // -------------------------
    server.on("/history", HTTP_GET, []() {
        unsigned int count = sensorGetHistoryCount();

        String json = "{";
        json += "\"count\":" + String(count) + ",";
        json += "\"interval\":" + String(sensorGetSampleIntervalSeconds()) + ",";
        json += "\"temp\":[";
        for (unsigned int i = 0; i < count; i++) {
            json += String(historyTemp[i], 2);
            if (i < count - 1) json += ",";
        }
        json += "],";

        json += "\"hum\":[";
        for (unsigned int i = 0; i < count; i++) {
            json += String(historyHum[i], 2);
            if (i < count - 1) json += ",";
        }
        json += "]";

        json += "}";

        server.send(200, "application/json", json);
    });

    // -------------------------
    // /debug.html y /debug
    // -------------------------
    server.on("/debug.html", HTTP_GET, []() {
        String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<title>Debug Riego</title>
<style>
body { font-family: monospace; background: #111; color: #0f0; padding: 20px; }
pre { font-size: 18px; }
</style>
</head>
<body>
<h2>Estado del sistema</h2>
<pre id="log">Cargando...</pre>

<script>
function update() {
    fetch("/debug")
        .then(r => r.text())
        .then(t => document.getElementById("log").textContent = t)
        .catch(e => document.getElementById("log").textContent = "Error: " + e);
}
setInterval(update, 1500);
update();
</script>
</body>
</html>
)rawliteral";

        server.send(200, "text/html", html);
    });

    server.on("/debug", HTTP_GET, []() {
        String out = "";

        float h = sensorGetHum();
        float t = sensorGetTemp();
        unsigned long ahora = millis();

        out += "Humedad: " + String(h, 2) + "\n";
        out += "Temperatura: " + String(t, 2) + "\n";

        out += "Historial: " + String(sensorGetHistoryCount()) + " muestras\n";

        server.send(200, "text/plain", out);
    });

    server.begin();
}

void webLoop() {
    server.handleClient();
}