#include <ESP8266WiFi.h>
#include "config.h"
#include "wifi_net.h"

static bool modoAP = false;

void iniciarAP() {
  modoAP = true;
  WiFi.mode(WIFI_AP);
  WiFi.softAP("Incubadora-Hasher");
  Serial.println("AP iniciado: Incubadora-Hasher");
  Serial.println("IP: 192.168.4.1");
}

void wifiInit() {
  if (strlen(cfg.ssid) < 1) {
    iniciarAP();
    return;
  }

  Serial.print("Conectando a ");
  Serial.println(cfg.ssid);

  bool usarIPFija = strlen(cfg.ip) > 0;

  if (usarIPFija) {
    IPAddress ip, gw, sn, dns;
    ip.fromString(cfg.ip);
    gw.fromString(cfg.gateway);
    sn.fromString(cfg.subnet);
    dns.fromString(cfg.dns);
    WiFi.config(ip, gw, sn, dns);
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(cfg.ssid, cfg.pass);

  unsigned long inicio = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - inicio < 10000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi conectado");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    modoAP = false;
  } else {
    Serial.println("\nNo se pudo conectar. Activando AP...");
    iniciarAP();
  }
}

bool wifiIsAPMode() {
  return modoAP;
}

String wifiGetIPString() {
  if (modoAP) {
    return WiFi.softAPIP().toString();
  } else {
    return WiFi.localIP().toString();
  }
}