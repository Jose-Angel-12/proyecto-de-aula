//Device ID: 18fd4e9b-47b1-4f3c-91ea-e3893ac1f387
// secret key: l7Zfb0PNWZq7MeX5RjheDYXPV
#include <esp_now.h>
#include <WiFi.h>
#include "arduino_secrets.h"
#include "thingProperties.h"
#include <SevenSegment.h>
#include <esp_wifi.h>

// Pines para 7 segmentos
#define SA 5
#define SB 4
#define SC 19
#define SD 25
#define SE 26
#define SF 33
#define SG 32
#define SP 14
#define LSB 13
#define MSB 12
#define WAIT 2

int segPins[] = {SA, SB, SC, SD, SE, SF, SG, SP};
SevenSegment display(segPins, LSB, MSB);

typedef struct struct_message {
  int valor;
  bool guardar;
} struct_message;

volatile int valorActual = 0;
volatile bool guardarNube = false;

void onDataRecv(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  struct_message incomingData;
  memcpy(&incomingData, data, sizeof(incomingData));
  valorActual = incomingData.valor;
  guardarNube = incomingData.guardar;
  Serial.print("Dato recibido: ");
  Serial.print(valorActual);
  if (guardarNube) Serial.println(" (Guardar)");
  else Serial.println();
}

void setup() {
  Serial.begin(115200);

  // 1. Conexión WiFi (para la nube)
  WiFi.mode(WIFI_STA);
  WiFi.begin(SECRET_SSID, SECRET_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi conectado");

  // 2. Nube Arduino
  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  // 3. Forzar ESP-NOW al canal de la WiFi
  int canal = WiFi.channel();
  esp_wifi_set_channel(canal, WIFI_SECOND_CHAN_NONE);

  // 4. Iniciar ESPNOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error al iniciar ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(onDataRecv);

  // 5. Inicializar display
  valorActual = 0;
  guardarNube = false;
}

void loop() {
  ArduinoCloud.update();

  int unidad = valorActual % 10;
  int decena = (valorActual / 10) % 10;
  for (int i = 0; i < 50; i++) {
    display.displayNumber(unidad, decena, WAIT);
  }

  // Si hay que guardar en la nube (solo si se recibió la orden)
  if (guardarNube) {
    valorNube = valorActual; // Esto lo sube a la nube
    guardarNube = false;
    Serial.println("Valor guardado en la nube!");
    delay(500); // Evitar repeticiones múltiples
  }
}
void onValorNubeChange() {}