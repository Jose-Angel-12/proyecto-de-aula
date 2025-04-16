#include <esp_now.h>
#include <WiFi.h>

#define LED_PIN 2

typedef struct struct_message {
  char a[32];
  int b;
  float c;
  bool d;
} struct_message;

struct_message myData;

// ✅ NUEVA función con firma compatible con ESP-IDF 5.x
void OnDataRecv(const esp_now_recv_info_t *recv_info, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));

  Serial.print("Dato recibido: ");
  Serial.println(myData.d ? "true" : "false");

  digitalWrite(LED_PIN, myData.d ? HIGH : LOW);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error al iniciar ESP-NOW");
    return;
  }

  // ✅ Registrar callback usando la nueva firma
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // Nada aquí, solo callbacks
}