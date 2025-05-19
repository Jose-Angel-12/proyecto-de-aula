#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

// Pines de los botones
#define BUTTON_UP    34
#define BUTTON_DOWN  32
#define BUTTON_SAVE  35  // Botón para guardar en la nube

// MAC del receptor (¡ajusta a tu caso!)
uint8_t broadcastAddress[] = { 0xF8, 0xB3, 0xB7, 0x2C, 0x6E, 0xA8 };

// Estructura de datos a enviar
typedef struct struct_message {
  int valor;
  bool guardar;
} struct_message;

struct_message myData;
esp_now_peer_info_t peerInfo;
int contador = 0;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Estado envío: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Enviado" : "Fallo");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  // --------- IMPORTANTE: Fuerza el canal WiFi igual al del receptor -----------
  // Cambia el número "6" por el canal real de tu WiFi (míralo en el receptor con WiFi.channel())
  esp_wifi_set_channel(6, WIFI_SECOND_CHAN_NONE);
  // ---------------------------------------------------------------------------

  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);
  pinMode(BUTTON_SAVE, INPUT_PULLUP);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error al iniciar ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0; // se ignora si usas esp_wifi_set_channel
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Error al añadir peer");
    return;
  }

  myData.valor = contador;
  myData.guardar = false;
  esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));
}

void loop() {
  bool botonPresionado = false;
  myData.guardar = false;  // Por defecto, no guardar

  if (digitalRead(BUTTON_UP) == LOW) {
    contador = min(99, contador + 1);
    botonPresionado = true;
    Serial.print("Contador = "); Serial.println(contador);
    Serial.println("Botón SUBIR");
    delay(50); // Antirrebote rápido
  }

  if (digitalRead(BUTTON_DOWN) == LOW) {
    contador = max(0, contador - 1);
    botonPresionado = true;
    Serial.print("Contador = "); Serial.println(contador);
    Serial.println("Botón BAJAR");
    delay(50); // Antirrebote rápido
  }

  if (digitalRead(BUTTON_SAVE) == LOW) {
    myData.guardar = true;  // Solo aquí activamos guardar
    botonPresionado = true;
    Serial.print("Contador = "); Serial.println(contador);
    Serial.println("Botón GUARDAR");
    delay(50); // Antirrebote rápido
  }

  if (botonPresionado) {
    myData.valor = contador;
    esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));
    delay(250); // Antirrebote general
  }
}