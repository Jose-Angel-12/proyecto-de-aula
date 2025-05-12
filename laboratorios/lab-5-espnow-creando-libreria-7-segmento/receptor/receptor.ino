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

uint8_t segPins[8] = {SA, SB, SC, SD, SE, SF, SG, SP};

typedef struct struct_message {
  int valor;
} struct_message;

struct_message incomingData;
int valorRecibido = 0;

SevenSegment display(segPins, LSB, MSB, 5);

void onDataRecv(const uint8_t *mac, const uint8_t *incomingDataBytes, int len) {
  memcpy(&incomingData, incomingDataBytes, sizeof(incomingData));
  valorRecibido = incomingData.valor;
  Serial.print("Dato recibido: ");
  Serial.println(valorRecibido);
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  display.begin();

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error al iniciar ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(onDataRecv);
}

void loop() {
  display.displayNumber(valorRecibido);
}
