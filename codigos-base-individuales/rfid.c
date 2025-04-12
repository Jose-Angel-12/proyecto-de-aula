
//https://www.youtube.com/watch?v=oIUBarWEJeM

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "esp_system.h"

// Incluye tu librería de MFRC522 para ESP-IDF
#include "mfrc522.h"

#define PIN_NUM_MISO 19
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK  18
#define PIN_NUM_SS   5
#define PIN_NUM_RST  17

#define LED_OK   21
#define LED_FAIL 22

static const char *TAG = "RFID";

void app_main(void)
{
    // Configurar pines de LED como salida
    gpio_pad_select_gpio(LED_OK);
    gpio_set_direction(LED_OK, GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(LED_FAIL);
    gpio_set_direction(LED_FAIL, GPIO_MODE_OUTPUT);

    // Inicializar SPI y MFRC522
    mfrc522_start(PIN_NUM_SS, PIN_NUM_RST);
    ESP_LOGI(TAG, "Aproxime o seu cartao do leitor...");

    while (true) {
        if (mfrc522_is_new_card_present() && mfrc522_read_card_serial()) {
            uint8_t *uid = mfrc522_get_uid();
            uint8_t size = mfrc522_get_uid_size();

            printf("UID da tag: ");
            char conteudo[64] = "";
            for (int i = 0; i < size; i++) {
                printf("%02X ", uid[i]);
                char byte_str[4];
                sprintf(byte_str, "%02X ", uid[i]);
                strcat(conteudo, byte_str);
            }
            printf("\n");

            // Validar UID (compara con "45 15 30 5B")
            if (strcmp(conteudo, "45 15 30 5B ") == 0) {
                printf("Bem vindo!\n");
                gpio_set_level(LED_OK, 1);
                vTaskDelay(pdMS_TO_TICKS(3000));
                gpio_set_level(LED_OK, 0);
            } else {
                printf("Tag nao cadastrada!\n");
                gpio_set_level(LED_FAIL, 1);
                vTaskDelay(pdMS_TO_TICKS(3000));
                gpio_set_level(LED_FAIL, 0);
            }

            mfrc522_halt();
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
