#include "manual.h"

// ======================================================
//  ESTILOS COMUNES (tema oscuro)
// ======================================================
String estiloOscuro() {
  return String(
    "<style>"
    "body{font-family:Arial;background:#121212;color:#e0e0e0;margin:0;padding:20px;}"
    ".container{max-width:900px;margin:auto;background:#1e1e1e;padding:20px;border-radius:10px;}"
    "h1,h2,h3{color:#90caf9;}"
    "pre{background:#0d0d0d;padding:10px;border-radius:8px;border:1px solid #333;overflow-x:auto;}"
    "button{background:#424242;color:#fff;padding:8px 12px;border:none;border-radius:5px;cursor:pointer;}"
    "button:hover{background:#616161;}"
    "a{color:#90caf9;text-decoration:none;}"
    "</style>"
  );
}

// ======================================================
//  MANUAL COMPLETO
// ======================================================
String manualCompleto() {
  String html = "<html><head><meta charset='UTF-8'>";
  html += estiloOscuro();
  html += "<title>Manual Completo</title></head><body>";
  html += "<div class='container'>";

  html += "<h1>Manual Completo de la Incubadora Hasher</h1>";
  html += "<p>Este manual describe en detalle el funcionamiento, instalación, mantenimiento y uso del sistema Incubadora Hasher.</p>";

  html += "<h2>1. Introducción</h2>";
  html += "<p>La Incubadora Hasher es un sistema automatizado de control de humedad basado en ESP8266, sensor SHT4x y relé.</p>";

  html += "<h2>2. Características</h2>";
  html += "<ul>"
          "<li>Control automático de humedad</li>"
          "<li>Sensor SHT4x de alta precisión</li>"
          "<li>Histórico de datos con gráfica en vivo</li>"
          "<li>Control manual del relé</li>"
          "<li>Protección anti-bloqueo</li>"
          "<li>Servidor web integrado</li>"
          "<li>Actualización OTA</li>"
          "<li>MQTT opcional</li>"
          "</ul>";

  html += "<h2>3. Acceso al sistema</h2>";
  html += "<p>Usuario: <b>hasher</b><br>Contraseña: <b>proyectoincubadora2026</b></p>";

  html += "<h2>4. Panel principal</h2>";
  html += "<p>Incluye lecturas en tiempo real, control manual del relé y gráfica histórica.</p>";

  html += "<h2>5. Configuración</h2>";
  html += "<p>Permite configurar WiFi, parámetros de control y MQTT.</p>";

  html += "<h2>6. OTA</h2>";
  html += "<p>Actualización del firmware desde el navegador mediante /update.</p>";

  html += "<h2>7. Esquema de conexiones</h2>";
  html += "<pre>"
          "SHT4x:\n"
          "  VCC  -> 3V3\n"
          "  GND  -> GND\n"
          "  SDA  -> D2 (GPIO4)\n"
          "  SCL  -> D1 (GPIO5)\n\n"
          "Relé:\n"
          "  IN   -> D6 (GPIO12)\n"
          "  VCC  -> 3V3\n"
          "  GND  -> GND\n"
          "</pre>";

  html += "<h2>8. Solución de problemas</h2>";
  html += "<ul>"
          "<li>Lecturas 1.99 → SDA/SCL invertidos o sensor mal alimentado</li>"
          "<li>Relé no activa → revisar cableado</li>"
          "<li>No conecta WiFi → revisar SSID/clave</li>"
          "</ul>";

  html += "<div style='text-align:center;margin-top:20px;'><a href='/'><button>⬅ Volver</button></a></div>";
  html += "</div></body></html>";
  return html;
}

// ======================================================
//  MANUAL RESUMIDO
// ======================================================
String manualResumido() {
  String html = "<html><head><meta charset='UTF-8'>";
  html += estiloOscuro();
  html += "<title>Manual Resumido</title></head><body>";
  html += "<div class='container'>";

  html += "<h1>Manual Resumido</h1>";
  html += "<p>Guía rápida para usar la Incubadora Hasher.</p>";

  html += "<h2>Acceso</h2>";
  html += "<p>Usuario: <b>hasher</b> — Contraseña: <b>proyectoincubadora2026</b></p>";

  html += "<h2>Funciones principales</h2>";
  html += "<ul>"
          "<li>Control automático de humedad</li>"
          "<li>Control manual del relé</li>"
          "<li>Gráfica histórica</li>"
          "<li>Configuración WiFi y MQTT</li>"
          "<li>Actualización OTA</li>"
          "</ul>";

  html += "<h2>Conexiones</h2>";
  html += "<pre>"
          "SHT4x → D1/D2\n"
          "Relé → D6\n"
          "</pre>";

  html += "<div style='text-align:center;margin-top:20px;'><a href='/'><button>⬅ Volver</button></a></div>";
  html += "</div></body></html>";
  return html;
}

// ======================================================
//  MANUAL TÉCNICO
// ======================================================
String manualTecnico() {
  String html = "<html><head><meta charset='UTF-8'>";
  html += estiloOscuro();
  html += "<title>Manual Técnico</title></head><body>";
  html += "<div class='container'>";

  html += "<h1>Manual Técnico para Instaladores</h1>";

  html += "<h2>Hardware</h2>";
  html += "<ul>"
          "<li>ESP8266 NodeMCU v2</li>"
          "<li>Sensor SHT4x (I2C)</li>"
          "<li>Relé activo en LOW</li>"
          "<li>Alimentación 3.3V</li>"
          "</ul>";

  html += "<h2>Conexiones</h2>";
  html += "<pre>"
          "SHT4x:\n"
          "  SDA → D2\n"
          "  SCL → D1\n"
          "Relé:\n"
          "  IN → D6\n"
          "</pre>";

  html += "<h2>Software</h2>";
  html += "<ul>"
          "<li>Lecturas cada 5s</li>"
          "<li>Histórico circular de 1200 muestras</li>"
          "<li>Protección anti-bloqueo del relé</li>"
          "<li>Reinicio automático por fallo de sensor</li>"
          "</ul>";

  html += "<h2>OTA</h2>";
  html += "<p>Ruta: <b>/update</b></p>";

  html += "<div style='text-align:center;margin-top:20px;'><a href='/'><button>⬅ Volver</button></a></div>";
  html += "</div></body></html>";
  return html;
}

// ======================================================
//  MANUAL USUARIO
// ======================================================
String manualUsuario() {
  String html = "<html><head><meta charset='UTF-8'>";
  html += estiloOscuro();
  html += "<title>Manual Usuario</title></head><body>";
  html += "<div class='container'>";

  html += "<h1>Manual para Usuarios</h1>";
  html += "<p>Guía sencilla para usar la incubadora sin conocimientos técnicos.</p>";

  html += "<h2>Qué hace la incubadora</h2>";
  html += "<p>Mantiene la humedad estable activando un humidificador cuando es necesario.</p>";

  html += "<h2>Cómo acceder</h2>";
  html += "<p>Entra en la IP del dispositivo desde tu móvil o PC.</p>";

  html += "<h2>Qué puedes ver</h2>";
  html += "<ul>"
          "<li>Temperatura</li>"
          "<li>Humedad</li>"
          "<li>Gráfica histórica</li>"
          "<li>Botón para activar el humidificador</li>"
          "</ul>";

  html += "<h2>Qué puedes configurar</h2>";
  html += "<ul>"
          "<li>WiFi</li>"
          "<li>Umbral de humedad</li>"
          "<li>Tiempo de activación</li>"
          "</ul>";

  html += "<div style='text-align:center;margin-top:20px;'><a href='/'><button>⬅ Volver</button></a></div>";
  html += "</div></body></html>";
  return html;
}

// ======================================================
//  MANUAL INGLÉS
// ======================================================
String manualIngles() {
  String html = "<html><head><meta charset='UTF-8'>";
  html += estiloOscuro();
  html += "<title>English Manual</title></head><body>";
  html += "<div class='container'>";

  html += "<h1>Incubadora Hasher - Full Manual (English)</h1>";

  html += "<h2>Overview</h2>";
  html += "<p>The Incubadora Hasher is an automated humidity control system based on ESP8266, SHT4x sensor and relay.</p>";

  html += "<h2>Main Features</h2>";
  html += "<ul>"
          "<li>Automatic humidity control</li>"
          "<li>Real-time dashboard</li>"
          "<li>Historical graph</li>"
          "<li>Manual relay control</li>"
          "<li>OTA firmware updates</li>"
          "<li>MQTT integration</li>"
          "</ul>";

  html += "<h2>Connections</h2>";
  html += "<pre>"
          "SHT4x:\n"
          "  SDA → D2\n"
          "  SCL → D1\n"
          "Relay:\n"
          "  IN → D6\n"
          "</pre>";

  html += "<div style='text-align:center;margin-top:20px;'><a href='/'><button>⬅ Back</button></a></div>";
  html += "</div></body></html>";
  return html;
}

// ======================================================
//  MANUAL ASCII
// ======================================================
String manualASCII() {
  String txt =
"INCUBADORA HASHER - MANUAL ASCII\n"
"=================================\n\n"
"CONEXIONES:\n"
"  SHT4x:\n"
"    VCC -> 3V3\n"
"    GND -> GND\n"
"    SDA -> D2\n"
"    SCL -> D1\n\n"
"  RELÉ:\n"
"    IN  -> D6\n"
"    VCC -> 3V3\n"
"    GND -> GND\n\n"
"FUNCIONAMIENTO:\n"
"  - Lee humedad cada 5s\n"
"  - Activa el relé si baja del umbral\n"
"  - Histórico de 1200 muestras\n"
"  - OTA en /update\n\n"
"USUARIO:\n"
"  hasher / proyectoincubadora2026\n";

  return txt;
}

// ======================================================
//  MANUAL IMPRESIÓN
// ======================================================
String manualImpresion() {
  String html = "<html><head><meta charset='UTF-8'>";
  html += "<style>"
          "body{font-family:Arial;background:white;color:black;padding:20px;}"
          "h1,h2{color:black;}"
          "pre{background:#f0f0f0;padding:10px;border-radius:5px;}"
          "</style>";
  html += "<title>Manual para imprimir</title></head><body>";

  html += "<h1>Incubadora Hasher - Manual para impresión</h1>";
  html += "<p>Versión optimizada para imprimir en papel.</p>";

  html += "<h2>Resumen</h2>";
  html += "<p>Control automático de humedad con sensor SHT4x y relé.</p>";

  html += "<h2>Conexiones</h2>";
  html += "<pre>"
          "SHT4x:\n"
          "  SDA -> D2\n"
          "  SCL -> D1\n"
          "Relé:\n"
          "  IN  -> D6\n"
          "</pre>";

  html += "<h2>Acceso</h2>";
  html += "<p>Usuario: hasher — Contraseña: proyectoincubadora2026</p>";

  html += "</body></html>";
  return html;
}