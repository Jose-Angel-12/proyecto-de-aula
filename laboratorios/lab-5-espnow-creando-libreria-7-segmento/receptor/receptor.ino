#include <esp_now.h>
#include <WiFi.h>
#include <SevenSegment.h>

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
} struct_message;

volatile int valorRecibido = 0;

void onDataRecv(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  struct_message incomingData;
  memcpy(&incomingData, data, sizeof(incomingData));
  valorRecibido = incomingData.valor;
  Serial.print("Dato recibido: ");
  Serial.println(valorRecibido);
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error al iniciar ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(onDataRecv);
}

void loop() {
  int unidad = valorRecibido % 10;
  int decena = (valorRecibido / 10) % 10;

  // Refrescamos 50 veces rápido para multiplexar
  for (int i = 0; i < 50; i++) {
    display.displayNumber(unidad, decena, WAIT);
  }
}

