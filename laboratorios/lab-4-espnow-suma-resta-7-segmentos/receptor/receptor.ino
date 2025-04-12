/*
  Objetivo:
  Configurar receptor del ESPNOW

  Requisitos:
  Se debe saber la MAC del receptor.
*/

#define BAUD_RATE 115200

///////////////////////////////////////////////// Declarar librerías

#include <esp_now.h>  // Para comunicación peer-to-peer ESP-NOW
#include <WiFi.h>     // Para funcionalidades WiFi del ESP32

///////////////////////////////////////////////// Definir variables

// Estructura para enviar recibir datos
//
// DEBE SER IGUAL que la estructura en el emisor
typedef struct struct_message {
    char a[32];   // Campo para cadena de texto (hasta 32 caracteres)
    int b;        // Campo para valor entero
    float c;      // Campo para valor flotante
    bool d;       // Campo para valor booleano
} struct_message;

// Variable global para almacenar los datos recibidos
struct_message myData;

///////////////////////////////////////////////// Funciones auxiliares

// Función de callback que se ejecuta cuando se reciben datos
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len)
{
  // Copiar los datos recibidos a nuestra estructura
  memcpy(&myData, incomingData, sizeof(myData));

  // Imprimir información de la recepción
  Serial.print("Bytes recibidos: ");
  Serial.println(len);

  // Imprimir cada campo de los datos recibidos

  Serial.print("Texto: ");
  Serial.println(myData.a);

  Serial.print("Entero: ");
  Serial.println(myData.b);

  Serial.print("Flotante: ");
  Serial.println(myData.c);

  Serial.print("Booleano: ");
  Serial.println(myData.d ? "true" : "false");

  // Salto de línea
  Serial.println();
}

///////////////////////////////////////////////// Función setup y loop

void setup()
{
  // Inicializar el puerto serial para monitoreo
  Serial.begin(115200);

  // Configurar el dispositivo como estación WiFi
  WiFi.mode(WIFI_STA);

  // Inicializar el protocolo ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error al inicializar ESP-NOW");
    return; // Detener ejecución si hay error
  }

  // Registrar la función de callback para manejar datos recibidos
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
}

void loop()
{
  // No es necesario código aquí, todo se maneja por callbacks cuando se reciben datos
}