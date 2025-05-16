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

// =================== USUARIOS RFID ===================
struct UsuarioRFID {
  String uid;
  String nombre;
  String password;
  bool activo;
};

UsuarioRFID usuarios[] = {
  {"43F4A914", "1111", "1234", false},
  {"4357FFA5", "2222", "2345", false},
  {"5B0A8122", "3333", "3456", false},
  {"B32A4B29", "4444", "4567", false}
};

// =================== USUARIOS WEB ===================
struct UsuarioWeb {
  String usuario;
  String contrasena;
};

UsuarioWeb usuariosWeb[] = {
  {"admin", "admin123"},
  // Puedes agregar más aquí
  // {"usuario2", "clave2"}
};

// =================== SESIÓN WEB ===================
bool webLogeado = false;

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

  for (UsuarioRFID& u : usuarios) {
    if (uid == u.uid) {
      if (u.activo) {
        mostrarLCD("Bienvenido", u.nombre.substring(u.nombre.length() - 4));
        activarCerradura();
      } else {
        mostrarLCD("Sin acceso", "");
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
      inputUser = "";
      inputPassword = "";
      mostrarLCD("Introduce Usuario", "");
    } else if (tecla == '#') {
      bool accesoValido = false;
      for (UsuarioRFID& u : usuarios) {
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
    } else {
      if (inputUser.length() < 4) {
        inputUser += tecla;
        mostrarLCD("Usuario: ", inputUser);
      } else if (inputPassword.length() < 4) {
        inputPassword += tecla;
        mostrarLCD("Contrasena: ", inputPassword);
      }
    }
  }
}

void handleLogin() {
  if (server.hasArg("user") && server.hasArg("pass")) {
    String user = server.arg("user");
    String pass = server.arg("pass");
    webLogeado = false;
    for (UsuarioWeb& u : usuariosWeb) {
      if (user == u.usuario && pass == u.contrasena) {
        webLogeado = true;
        break;
      }
    }
    if (webLogeado) {
      server.sendHeader("Location", "/");
      server.send(302, "text/plain", "Login correcto");
    } else {
      server.send(200, "text/html", "<h3>Usuario o contraseña incorrectos</h3><a href='/login'>Volver</a>");
    }
  } else {
    String html = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1'><meta charset='UTF-8'><title>Login: Control de Acceso</title>";
    html += "<style>body{font-family: Arial, sans-serif;text-align: center;background-color: #f0f0f0;}button{display: inline-block;margin-top: 5px;padding: 5px 10px;background: #007BFF;color: white;border-radius: 5px;text-decoration: none;}</style></head><body>";
    html +="<form method='get'><h3>Iniciar sesión</h3>Usuario: <input name='user'><br>Contraseña: <input name='pass' type='password'><br><button value='Entrar'>Entrar</button></form></body><html/>";

    server.send(200, "text/html", html);
  }
}

void handleRoot() {
  if (!webLogeado) {
    server.sendHeader("Location", "/login");
    server.send(302, "text/plain", "Redirigiendo a login...");
    return;
  }

  String html = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1'><meta charset='UTF-8'><title>Control de Acceso RFID</title>";
  html += "<style>body{font-family: Arial, sans-serif; text-align:center; background-color:#f0f0f0;} .container{max-width: 400px; margin: auto; background: white; padding: 20px; border-radius: 10px;} h2{color: #333;} .user-card{background: #fafafa; padding: 10px; margin: 10px 0; border-radius: 8px;} .activo{color: green; font-weight: bold;} .inactivo{color: red; font-weight: bold;} a{display:inline-block; margin-top:5px; padding:5px 10px; background:#007BFF; color:white; border-radius:5px; text-decoration:none;} a:hover{background:#0056b3;}</style></head><body>";
  html += "<div class='container'><h2>Control de acceso RFID</h2>";

  for (int i = 0; i < 4; i++) {
    html += "<div class='user-card'>";
    html += "<div><b>Usuario:</b> " + usuarios[i].nombre + "</div>";
    html += "<div><b>UID:</b> *****" + usuarios[i].uid.substring(usuarios[i].uid.length() - 2) + "</div>";
    html += "<div>Estado: " + String(usuarios[i].activo ? "<span class='activo'>ACTIVO</span>" : "<span class='inactivo'>INACTIVO</span>") + "</div>";
    html += "<a href='/toggle?uid=" + usuarios[i].uid + "'>Cambiar estado</a>";
    html += "</div>";
  }

  html += "<a href='/abrir'>ABRIR MANUAL</a><br><a href='/logout'>Cerrar sesión</a>";
  html += "</div></body></html>";
  server.send(200, "text/html", html);
}

void handleToggle() {
  if (!webLogeado) {
    server.sendHeader("Location", "/login");
    server.send(302, "text/plain", "Redirigiendo a login...");
    return;
  }

  if (server.hasArg("uid")) {
    String uid = server.arg("uid");
    for (UsuarioRFID& u : usuarios) {
      if (u.uid == uid) {
        u.activo = !u.activo;
        break;
      }
    }
  }
  handleRoot();
}

void handleAbrir() {
  if (webLogeado) {
    activarCerradura();
    server.sendHeader("Location", "/");
    server.send(302, "text/plain", "Cerradura abierta");
  } else {
    server.sendHeader("Location", "/login");
    server.send(302, "text/plain", "Redirigiendo a login...");
  }
}

void handleLogout() {
  webLogeado = false;
  server.sendHeader("Location", "/login");
  server.send(302, "text/plain", "Cerrando sesión...");
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
  server.on("/login", handleLogin);
  server.on("/toggle", handleToggle);
  server.on("/abrir", handleAbrir);
  server.on("/logout", handleLogout);
  server.begin();
  mostrarLCD("Acerca tu TAG", "o usa el teclado");
}

// =================== LOOP ===================
void loop() {
  manejarRFID();
  manejarTeclado();
  server.handleClient();
}
