/*

Pin RC522	Pin ESP32
SDA	5
SCK	18
MOSI	23
MISO	19
GND	GND
RST	4
3.3V	3.3V


Pin LCD	Pin ESP32
VCC	3.3V o 5V
GND	GND
SDA	21
SCL	22
*/
#include <Wire.h>                     // Comunicación I2C
#include <LiquidCrystal_I2C.h>       // Pantalla LCD I2C
#include <SPI.h>                     // Comunicación SPI para RFID
#include <MFRC522.h>                 // Librería del módulo RFID

// --- Configuración LCD I2C ---
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Dirección 0x27, pantalla 16x2

// --- Pines RFID ---
#define SS_PIN 5      // SDA
#define RST_PIN 4     // Reset

MFRC522 rfid(SS_PIN, RST_PIN); // Crear objeto del lector

void setup() {
  Serial.begin(115200);

  // --- Inicializar LCD ---
  lcd.init();            // Iniciar LCD
  lcd.backlight();       // Encender retroiluminación
  lcd.setCursor(0, 0);
  lcd.print("RFID + LCD I2C");
  delay(2000);
  lcd.clear();

  // --- Inicializar RFID ---
  SPI.begin();           // Iniciar bus SPI
  rfid.PCD_Init();       // Iniciar RFID
  lcd.print("Acerca tu TAG");
}

void loop() {
  // Esperar hasta que se acerque una tarjeta
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  // Leer el UID y mostrar en LCD y Serial
  String uidStr = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    uidStr += String(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
    uidStr += String(rfid.uid.uidByte[i], HEX);
  }
  uidStr.toUpperCase();

  // Mostrar en LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("UID Detectado:");
  lcd.setCursor(0, 1);
  lcd.print(uidStr);

  // También mostrar en el monitor serial
  Serial.print("UID: ");
  Serial.println(uidStr);

  // Detener lectura un momento
  delay(3000);
  lcd.clear();
  lcd.print("Acerca tu TAG");

  // Finalizar comunicación con tarjeta
  rfid.PICC_HaltA();
}

