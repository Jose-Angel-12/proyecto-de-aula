/*
  Objetivo:
  Configurar emisor del ESPNOW

  Requisitos:
  Se debe saber la MAC del receptor.
*/

#define BAUD_RATE 115200

///////////////////////////////////////////////// Declarar librerías

#include <esp_now.h>  // Librería para comunicación ESP-NOW
#include <WiFi.h>     // Librería WiFi para ESP32

///////////////////////////////////////////////// Definir variables

// Dirección MAC del receptor
// Se debe reemplazar con la dirección MAC del receptor
// Ejemplo:
// Si la dirección del receptor es 08:d1:f9:de:03:48, entonces queda
// uint8_t broadcastAddress[] = {0x08, 0xD1, 0xF9, 0xDE, 0x03, 0x48};
//
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Estructura para enviar datos
//
// DEBE SER IGUAL que la estructura en el receptor
typedef struct struct_message {
  char a[32];   // Campo para texto (hasta 32 caracteres)
  int b;        // Campo para valores enteros
  float c;      // Campo para valores decimales
  bool d;       // Campo para valores booleanos
} struct_message;

// Variable para almacenar los datos a enviar
struct_message myData;

// Variable para almacenar información del dispositivo receptor
esp_now_peer_info_t peerInfo;

///////////////////////////////////////////////// Funciones auxiliares

// Función de callback que se ejecuta al enviar datos
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("\r\nEstado del último envío:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Entrega exitosa" : "Fallo en entrega");
}

///////////////////////////////////////////////// Función setup y loop

void setup()
{
  // Inicializar el monitor serial a 115200 baudios
  Serial.begin(BAUD_RATE);

  // Configurar el dispositivo como estación WiFi
  WiFi.mode(WIFI_STA);

  // Inicializar ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error al inicializar ESP-NOW");
    return;
  }

  // Registrar la función de callback para conocer el estado del envío
  esp_now_register_send_cb(OnDataSent);

  // Configurar información del dispositivo receptor
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);  // Copiar dirección MAC
  peerInfo.channel = 0;                             // Usar canal 0
  peerInfo.encrypt = false;                         // Sin encriptación

  // Añadir el dispositivo receptor a la lista de pares
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Error al añadir el receptor");
    return;
  }
}

void loop()
{
  // Asignar valores a los campos de datos
  strcpy(myData.a, "THIS IS A CHAR");  // Texto fijo
  myData.b = random(1,20);             // Entero aleatorio entre 1 y 20
  myData.c = 1.2;                      // Valor decimal fijo
  myData.d = false;                    // Valor booleano fijo

  // Enviar los datos via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));

  if (result == ESP_OK)
  {
    Serial.println("Datos enviados con éxito");
  }
  else
  {
    Serial.println("Error al enviar los datos");
  }
  delay(2000);  // Esperar 2 segundos entre envíos
}