#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ESP32Servo.h>
#include <Keypad.h>

// =================== CONFIGURACIÓN DE WIFI (AP) ===================
const char* ssid = "ESP32_ACCESS";
const char* password = "12345678";  // Mínimo 8 caracteres
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
#define BUZZER_PIN 16

// =================== TECLADO ===================
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {14, 15, 25, 26};
byte colPins[COLS] = {27, 32, 33, 12};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

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
        mostrarLCD("Bienvenido", u.nombre.substring(u.nombre.length() - 4));
        activarCerradura();
      } else {
        mostrarLCD("Sin acceso", "" );
      }
      break;
    }
  }
  delay(2000);
  mostrarLCD("Acerca tu TAG", "o usa el teclado");
  rfid.PICC_HaltA();
}

String inputUser = "";
String inputPassword = "";
void manejarTeclado() {
  char tecla = keypad.getKey();
  if (tecla) {
    if (tecla == '*') {
      inputUser = "";  // Reiniciar entrada de usuario
      inputPassword = "";  // Reiniciar entrada de contraseña
      mostrarLCD("Introduce Usuario", "");
    } 
    else if (tecla == '#') {
      bool accesoValido = false;
      for (Usuario& u : usuarios) {
        if (inputUser == u.nombre && inputPassword == u.password && u.activo) {
          mostrarLCD("Acceso Concedido", u.nombre);
          activarCerradura();
          accesoValido = true;
          mostrarLCD("Acerca tu TAG", "o usa el teclado");
          break;
        }
      }
      if (!accesoValido) {
        mostrarLCD("Acceso Denegado", "");
      }
    } 
    else {
      if (inputUser.length() < 4) {
        inputUser += tecla;
        mostrarLCD("Usuario: ", inputUser);
      } 
      else if (inputPassword.length() < 4) {
        inputPassword += tecla;
        mostrarLCD("Contrasena: ", inputPassword);
      }
    }
  }
}

void handleRoot() {
  String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
      <meta charset='UTF-8'>
      <meta name='viewport' content='width=device-width, initial-scale=1.0'>
      <title>Control de Acceso RFID</title>
      <style>
        body {
          background: #f0f2f5;
          font-family: Arial, sans-serif;
          display: flex;
          flex-direction: column;
          align-items: center;
          margin: 0;
          padding: 0;
        }
        .container {
          margin-top: 30px;
          background: #fff;
          padding: 20px;
          border-radius: 12px;
          box-shadow: 0 4px 8px rgba(0,0,0,0.1);
          max-width: 400px;
          width: 90%;
        }
        h2 {
          text-align: center;
          margin-bottom: 20px;
          color: #333;
        }
        .user {
          background: #f9f9f9;
          margin: 10px 0;
          padding: 15px;
          border-radius: 8px;
          text-align: center;
          box-shadow: 0 2px 4px rgba(0,0,0,0.05);
        }
        .estado {
          display: block;
          margin: 10px 0;
          font-weight: bold;
          color: #555;
        }
        .boton {
          background: #28a745;
          color: white;
          padding: 10px 20px;
          text-decoration: none;
          border-radius: 8px;
          display: inline-block;
          margin-top: 10px;
          transition: background 0.3s ease;
        }
        .boton:hover {
          background: #218838;
        }
      </style>
    </head>
    <body>
      <div class='container'>
        <h2>Control de Acceso RFID</h2>
  )rawliteral";

  for (int i = 0; i < 4; i++) {
    html += "<div class='user'>";
    html += "<p>Usuario: " + usuarios[i].nombre + "<p>";
    html += "<p>UID *****" + usuarios[i].uid.substring(usuarios[i].uid.length() - 2) + "</p>";
    html += "<span class='estado'>" + String(usuarios[i].activo ? "ACTIVO" : "INACTIVO") + "</span>";
    html += "<a class='boton' href='/toggle?uid=" + usuarios[i].uid + "'>Cambiar Estado</a>";
    html += "</div>";
  }

  html += R"rawliteral(
      </div>
    </body>
    </html>
  )rawliteral";

  server.send(200, "text/html", html);
}

void handleToggle() {
  if (server.hasArg("uid")) {
    String uid = server.arg("uid");
    for (Usuario& u : usuarios) {
      if (u.uid == uid) {
        u.activo = !u.activo;
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

  WiFi.softAP(ssid, password);
  Serial.println("AP IP address: " + WiFi.softAPIP().toString());

  server.on("/", handleRoot);
  server.on("/toggle", handleToggle);
  server.begin();
  mostrarLCD("Acerca tu TAG", "o usa el teclado");
}

// =================== LOOP ===================
void loop() {
  manejarRFID();
  manejarTeclado();
  server.handleClient();
}
