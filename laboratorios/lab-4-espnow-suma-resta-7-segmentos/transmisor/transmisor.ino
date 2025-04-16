#include <esp_now.h>
#include <WiFi.h>

#define BUTTON_SUM 34
#define BUTTON_RES 32
#define BUTTON_MID 35

uint8_t broadcastAddress[] = { 0xF8, 0xB3, 0xB7, 0x2C, 0x6E, 0xA8 };

typedef struct struct_message {
  int valor;
} struct_message;

struct_message myData;
esp_now_peer_info_t peerInfo;

int contador = 0;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Enviado" : "Fallo");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  pinMode(BUTTON_SUM, INPUT_PULLUP);
  pinMode(BUTTON_RES, INPUT_PULLUP);
  pinMode(BUTTON_MID, INPUT_PULLUP);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error al iniciar ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Error al añadir peer");
    return;
  }

  // Enviar valor inicial 0
  myData.valor = contador;
  esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));
}

void loop() {
  bool botonPresionado = false;

  if (digitalRead(BUTTON_SUM) == LOW) {
    contador = min(99, contador + 1);
    botonPresionado = true;
    Serial.println("Botón SUMA");
  }

  if (digitalRead(BUTTON_RES) == LOW) {
    contador = max(0, contador - 1);
    botonPresionado = true;
    Serial.println("Botón RESTA");
  }

  if (digitalRead(BUTTON_MID) == LOW) {
    contador = 50;
    botonPresionado = true;
    Serial.println("Botón MITAD");
  }

  if (botonPresionado) {
    myData.valor = contador;
    esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));
    delay(300);  // Antirrebote
  }
}
