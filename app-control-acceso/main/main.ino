#include <Wire.h>                     // Comunicación I2C
#include <LiquidCrystal_I2C.h>       // Librería para manejar pantalla LCD I2C
#include <SPI.h>                     // Comunicación SPI para el lector RFID
#include <MFRC522.h>                 // Librería del módulo RFID RC522
#include <ESP32Servo.h>              // Librería para controlar servos en ESP32
#include <WiFi.h>                    // Librería para red WiFi
#include <WebServer.h>               // Servidor web local

// --- LCD I2C ---
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Dirección I2C 0x27, LCD de 16 columnas x 2 filas

// --- Pines del lector RFID ---
#define SS_PIN 5
#define RST_PIN 4
MFRC522 rfid(SS_PIN, RST_PIN);       // Crear objeto del lector RFID

// --- Servo ---
Servo servo;
#define SERVO_PIN 13
#define OPEN_POS 90                   // Posición del servo para abrir
#define CLOSE_POS 0                   // Posición del servo para cerrar

// --- Configuración WiFi ---
const char* ssid = "ESP32_AP";
const char* password = "12345678";
WebServer server(80);                // Crear servidor web en el puerto 80

// --- Lista de UIDs autorizados ---
#define MAX_UIDS 20
String allowedUIDs[MAX_UIDS];
int uidCount = 0;
String lastUID = "";

void setup() {
  Serial.begin(115200);

  // --- LCD ---
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("RFID + LCD + Web");
  delay(2000);
  lcd.clear();
  lcd.print("Acerca tu TAG");

  // --- RFID ---
  SPI.begin();
  rfid.PCD_Init();

  // --- Servo ---
  servo.attach(SERVO_PIN);
  servo.write(CLOSE_POS);

  // --- WiFi Access Point ---
  WiFi.softAP(ssid, password);
  Serial.println("[WiFi] AP creado");
  Serial.print("IP local: ");
  Serial.println(WiFi.softAPIP());

  // --- Rutas del servidor web ---
  server.on("/", handleRoot);
  server.on("/add", handleAdd);
  server.begin();
  Serial.println("[WebServer] Servidor iniciado");
}

void loop() {
  server.handleClient();  // Gestionar peticiones web

  // Verificar si se presenta una nueva tarjeta RFID
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  // Obtener el UID de la tarjeta
  String uidStr = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) uidStr += "0";
    uidStr += String(rfid.uid.uidByte[i], HEX);
  }
  uidStr.toUpperCase();
  lastUID = uidStr;

  Serial.print("[RFID] UID detectado: ");
  Serial.println(uidStr);

  bool accessGranted = false;
  for (int i = 0; i < uidCount; i++) {
    if (uidStr.equalsIgnoreCase(allowedUIDs[i])) {
      accessGranted = true;
      break;
    }
  }

  lcd.clear();
  if (accessGranted) {
    lcd.setCursor(0, 0);
    lcd.print("Acceso Permitido");
    lcd.setCursor(0, 1);
    lcd.print("Abriendo cerradura");
    servo.write(OPEN_POS);
    delay(3000);
    servo.write(CLOSE_POS);
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Acceso Denegado");
    lcd.setCursor(0, 1);
    lcd.print("Cerradura cerrada");
  }

  delay(3000);
  lcd.clear();
  lcd.print("Acerca tu TAG");
  rfid.PICC_HaltA();  // Detener comunicación con la tarjeta
}

// --- Página principal ---
void handleRoot() {
  String html = "<html><head><title>Control RFID</title></head><body>";
  html += "<h2>Último UID detectado:</h2>" + lastUID + "<br>";
  html += "<form action='/add' method='get'>";
  html += "UID: <input name='uid' value='" + lastUID + "'><input type='submit' value='Autorizar'>";
  html += "</form><br><h3>UIDs autorizados:</h3><ul>";
  for (int i = 0; i < uidCount; i++) {
    html += "<li>" + allowedUIDs[i] + "</li>";
  }
  html += "</ul></body></html>";
  server.send(200, "text/html", html);
}

// --- Agregar UID a la lista ---
void handleAdd() {
  if (server.hasArg("uid")) {
    String newUID = server.arg("uid");
    newUID.toUpperCase();
    if (uidCount < MAX_UIDS) {
      allowedUIDs[uidCount++] = newUID;
      Serial.println("[Web] UID agregado: " + newUID);
    }
  }
  server.sendHeader("Location", "/");
  server.send(303);
}
