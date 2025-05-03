#include <esp_now.h>
#include <WiFi.h>

// Pines para los segmentos
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
#define WAIT 5

int segPins[] = {SA, SB, SC, SD, SE, SF, SG, SP};

// Segmentos para mostrar 0-F y todo apagado (16)
uint8_t segCode[17][8] = {
  {0, 0, 0, 0, 0, 0, 1, 1}, // 0
  {1, 0, 0, 1, 1, 1, 1, 1}, // 1
  {0, 0, 1, 0, 0, 1, 0, 1}, // 2
  {0, 0, 0, 0, 1, 1, 0, 1}, // 3
  {1, 0, 0, 1, 1, 0, 0, 1}, // 4
  {0, 1, 0, 0, 1, 0, 0, 1}, // 5
  {0, 1, 0, 0, 0, 0, 0, 1}, // 6
  {0, 0, 0, 1, 1, 1, 1, 1}, // 7
  {0, 0, 0, 0, 0, 0, 0, 1}, // 8
  {0, 0, 0, 0, 1, 0, 0, 1}, // 9
  {0, 0, 0, 1, 0, 0, 0, 1}, // A
  {1, 1, 0, 0, 0, 0, 0, 1}, // B
  {0, 1, 1, 0, 0, 0, 1, 1}, // C
  {1, 0, 0, 0, 0, 1, 0, 1}, // D
  {0, 1, 1, 0, 0, 0, 0, 1}, // E
  {0, 1, 1, 1, 0, 0, 0, 1}, // F
  {1, 1, 1, 1, 1, 1, 1, 1}, // Apagado
};

typedef struct struct_message {
  int valor;
} struct_message;

struct_message incomingData;
int valorRecibido = 0;

void mostrar(int digit) {
  for (int i = 0; i < 8; i++) {
    digitalWrite(segPins[i], segCode[digit][i]);
  }
}

void mostrarDigito(int unidad, int decena) {
  mostrar(16);  // Apagar primero
  digitalWrite(LSB, HIGH);
  digitalWrite(MSB, LOW);
  mostrar(unidad);
  delay(WAIT);

  mostrar(16);
  digitalWrite(LSB, LOW);
  digitalWrite(MSB, HIGH);
  mostrar(decena);
  delay(WAIT);
}

//  para la función callback compatible con el nuevo ESP-NOW
void onDataRecv(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  memcpy(&incomingData, data, sizeof(incomingData));
  valorRecibido = incomingData.valor;
  Serial.print("Dato recibido: ");
  Serial.println(valorRecibido);
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  for (int i = 0; i < 8; i++) {
    pinMode(segPins[i], OUTPUT);
    digitalWrite(segPins[i], HIGH); // Todo apagado
  }

  pinMode(LSB, OUTPUT);
  pinMode(MSB, OUTPUT);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error al iniciar ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(onDataRecv);
}

void loop() {
  int unidad = valorRecibido % 10;
  int decena = valorRecibido / 10;
  mostrarDigito(unidad, decena);
}
