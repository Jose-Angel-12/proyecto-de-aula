#include <Wire.h>                     
#include <LiquidCrystal_I2C.h>       
#include <SPI.h>                     
#include <MFRC522.h>                 
#include <ESP32Servo.h>              // Librería compatible con ESP32 para controlar servos

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

// --- UIDs permitidos ---
String allowedUIDs[] = {
  "43F4A914",
  "4357FFA5"
};

void setup() {
  Serial.begin(115200);

  // --- LCD ---
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("RFID + LCD + Servo");
  delay(2000);
  lcd.clear();
  lcd.print("Acerca tu TAG");

  // --- RFID ---
  SPI.begin();           
  rfid.PCD_Init();       

  // --- Servo ---
  servo.attach(SERVO_PIN);    
  servo.write(CLOSE_POS);     // Inicia cerrado
}

void loop() {
  // Esperar tarjeta
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  // Obtener UID
  String uidStr = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) uidStr += "0";
    uidStr += String(rfid.uid.uidByte[i], HEX);
  }
  uidStr.toUpperCase();

  Serial.print("UID Detectado: ");
  Serial.println(uidStr);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("UID:");
  lcd.setCursor(0, 1);
  lcd.print(uidStr);
  delay(2000);

  bool accessGranted = false;

  for (String allowed : allowedUIDs) {
    if (uidStr.equalsIgnoreCase(allowed)) {
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

  rfid.PICC_HaltA();  // Finalizar lectura
}
