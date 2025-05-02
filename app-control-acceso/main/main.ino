#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ESP32Servo.h>

// =================== CONFIGURACIÓN DE WIFI (AP) ===================
const char* ssid = "ESP32_ACCESS";
const char* password = "12345678";
WebServer server(80);

// =================== LCD ===================
LiquidCrystal_I2C lcd(0x27, 16, 2);

// =================== RFID ===================
#define SS_PIN 5
#define RST_PIN 4
MFRC522 rfid(SS_PIN, RST_PIN);

// =================== SERVO ===================
Servo servo;
#define SERVO_PIN 13
#define OPEN_POS 90
#define CLOSE_POS 0

// =================== BUZZER ===================
#define BUZZER_PIN 15

// =================== USUARIOS ===================
struct Usuario {
  String uid;
  String nombre;
  String password;
  bool activo;
};

Usuario usuarios[] = {
  {"43F4A914", "1111", "1234", false},
  {"4357FFA5", "2222", "2345", false},
  {"5B0A8122", "3333", "3456", false},
  {"B32A4B29", "4444", "4567", false}
};

// =================== FUNCIONES ===================
void activarCerradura() {
  servo.write(OPEN_POS);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(300);
  digitalWrite(BUZZER_PIN, LOW);
  delay(3000);
  servo.write(CLOSE_POS);
}

void mostrarLCD(const String& mensaje1, const String& mensaje2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(mensaje1);
  lcd.setCursor(0, 1);
  lcd.print(mensaje2);
}

void manejarRFID() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return;

  String uid = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) uid += "0";
    uid += String(rfid.uid.uidByte[i], HEX);
  }
  uid.toUpperCase();
  Serial.println("[RFID] UID detectado: " + uid);

  for (Usuario& u : usuarios) {
    if (uid == u.uid) {
      if (u.activo) {
        mostrarLCD("Bienvenido", u.nombre);
        activarCerradura();
      } else {
        mostrarLCD("Sin acceso", "");
      }
      break;
    }
  }

  delay(2000);
  mostrarLCD("Acerca tu TAG", "");
  rfid.PICC_HaltA();
}

// =================== WEB SERVER ===================
void handleRoot() {
  String html = "<html><head><meta name='viewport' content='width=device-width, initial-scale=1'/></head><body>";
  html += "<h2>Control de acceso RFID</h2>";
  for (int i = 0; i < 4; i++) {
    html += "<p>UID *****" + usuarios[i].uid.substring(usuarios[i].uid.length() - 2) +
            " - Estado: " + (usuarios[i].activo ? "<b>ACTIVO</b>" : "<b>INACTIVO</b>") +
            " <a href='/toggle?uid=" + usuarios[i].uid + "'>[Cambiar]</a></p>";
  }
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleToggle() {
  if (server.hasArg("uid")) {
    String uid = server.arg("uid");
    for (Usuario& u : usuarios) {
      if (u.uid == uid) {
        u.activo = !u.activo;
        Serial.println("[WEB] Estado cambiado UID: " + uid + " -> " + (u.activo ? "ACTIVO" : "INACTIVO"));
        break;
      }
    }
  }
  handleRoot();
}

// =================== SETUP ===================
void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  mostrarLCD("Iniciando...", "");

  SPI.begin();
  rfid.PCD_Init();

  servo.attach(SERVO_PIN);
  servo.write(CLOSE_POS);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // WiFi como Access Point
  WiFi.softAP(ssid, password);
  Serial.println("AP IP address: " + WiFi.softAPIP().toString());

  server.on("/", handleRoot);
  server.on("/toggle", handleToggle);
  server.begin();

  mostrarLCD("Acerca tu TAG", "");
}

// =================== LOOP ===================
void loop() {
  manejarRFID();
  server.handleClient();
}
