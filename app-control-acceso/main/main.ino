#include <Wire.h>                     
#include <LiquidCrystal_I2C.h>       
#include <SPI.h>                     
#include <MFRC522.h>                 
#include <ESP32Servo.h>             
#include <WiFi.h>                    
#include <WebServer.h>              

// --- LCD ---
LiquidCrystal_I2C lcd(0x27, 16, 2);

// --- Pines RFID ---
#define SS_PIN 5     
#define RST_PIN 4    
MFRC522 rfid(SS_PIN, RST_PIN);

// --- Servo ---
Servo servo;
#define SERVO_PIN 13
#define OPEN_POS 90   // Posición abierta
#define CLOSE_POS 0   // Posición cerrada

// --- WiFi ---
const char* ssid = "ESP32_AP";
const char* password = "12345678";
WebServer server(80);

// --- Lista de UID permitidos ---
#define MAX_UIDS 10
String allowedUIDs[MAX_UIDS];
int uidCount = 0;

// --- Funciones Web ---
void handleRoot() {
  String html = "<html><body><h1>Control de Acceso</h1>";
  html += "<form action='/add' method='get'>Agregar UID: <input name='uid'><input type='submit'></form>";
  html += "<h3>UIDs Autorizados:</h3><ul>";
  for (int i = 0; i < uidCount; i++) {
    html += "<li>" + allowedUIDs[i] + "</li>";
  }
  html += "</ul></body></html>";
  server.send(200, "text/html", html);
}

void handleAdd() {
  if (server.hasArg("uid")) {
    String newUID = server.arg("uid").c_str();
    newUID.toUpperCase();
    if (uidCount < MAX_UIDS) {
      allowedUIDs[uidCount++] = newUID;
      Serial.println("UID agregado: " + newUID);
    }
  }
  server.sendHeader("Location", "/");
  server.send(303);
}

// --- Setup ---
void setup() {
  Serial.begin(115200);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Inicializando...");

  SPI.begin();
  rfid.PCD_Init();

  servo.attach(SERVO_PIN);
  servo.write(CLOSE_POS);

  // --- Iniciar WiFi como AP ---
  WiFi.softAP(ssid, password);
  Serial.println("AP Iniciado. IP:");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/add", handleAdd);
  server.begin();

  lcd.clear();
  lcd.print("Acerca tu TAG");
}

// --- Loop ---
void loop() {
  server.handleClient();

  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  String uidStr = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) uidStr += "0";
    uidStr += String(rfid.uid.uidByte[i], HEX);
  }
  uidStr.toUpperCase();

  Serial.print("UID Detectado: ");
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
    lcd.print("Cerradura abierta");
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

  rfid.PICC_HaltA();
}
