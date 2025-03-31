#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/touch_pad.h"
#include "driver/gpio.h"

#define LED1 26
#define LED2 25
#define LED3 33
#define LED4 32

#define T1 TOUCH_PAD_NUM4
#define T2 TOUCH_PAD_NUM5
#define T3 TOUCH_PAD_NUM6
#define T4 TOUCH_PAD_NUM7

#define LIMITE 300
#define ESPERA 500
#define LONGITUD_SECUENCIA 8
#define LEDS 4

// Secuencia deseada (modificable)
uint8_t secuencia[LONGITUD_SECUENCIA][LEDS] = {
    {1, 1, 0, 0},
    {1, 1, 1, 1},
    {0, 1, 1, 0},
    {0, 0, 1, 1},
    {1, 0, 1, 0},
    {0, 1, 1, 1},
    {0, 1, 0, 1},
    {1, 1, 0, 1},
};

int leds[] = {LED1, LED2, LED3, LED4};
int tactiles[] = {T1, T2, T3, T4};

void configurarPines() {
    for (int i = 0; i < LEDS; i++) {
        gpio_reset_pin(leds[i]);
        gpio_set_direction(leds[i], GPIO_MODE_OUTPUT);
        gpio_set_level(leds[i], 0);
    }
}

void configurarTactiles() {
    touch_pad_init();
    for (int i = 0; i < LEDS; i++) {
        touch_pad_config(tactiles[i], 0);
    }
}

// Mostrar la secuencia acumulada hasta el nivel actual
void mostrarSecuenciaAcumulada(int nivel) {
    for (int fila = 0; fila <= nivel; fila++) {
        for (int i = 0; i < LEDS; i++) {
            if (secuencia[fila][i] == 1) {
                gpio_set_level(leds[i], 1);
                vTaskDelay(ESPERA / portTICK_PERIOD_MS);
                gpio_set_level(leds[i], 0);
                vTaskDelay(ESPERA / portTICK_PERIOD_MS);
            }
        }
        vTaskDelay(ESPERA / portTICK_PERIOD_MS);
    }
}

int leerTouch() {
    uint16_t touch_value;
    for (int i = 0; i < LEDS; i++) {
        touch_pad_read(tactiles[i], &touch_value);
        if (touch_value < LIMITE) {
            return i;  // Devuelve el índice del pin tocado
        }
    }
    return -1;
}

void juegoMemoria(void *pvParameter) {
    int nivel = 0;
    while (1) {
        // Mostrar toda la secuencia acumulada hasta el nivel actual
        mostrarSecuenciaAcumulada(nivel);

        // Recolectar el orden correcto de toques esperado:
        int ordenEsperado[LONGITUD_SECUENCIA * LEDS];
        int pasosEsperados = 0;

        // Armar la lista de los LEDs que se encendieron hasta el nivel
        for (int fila = 0; fila <= nivel; fila++) {
            for (int i = 0; i < LEDS; i++) {
                if (secuencia[fila][i] == 1) {
                    ordenEsperado[pasosEsperados++] = i;
                }
            }
        }

        // Verificar que el jugador toque los LEDs en el mismo orden
        for (int paso = 0; paso < pasosEsperados; paso++) {
            int pinPresionado = -1;
            while (pinPresionado == -1) {
                pinPresionado = leerTouch();
                vTaskDelay(50 / portTICK_PERIOD_MS);
            }

            if (pinPresionado != ordenEsperado[paso]) {
                printf("¡Error! Tocaste %d pero se esperaba %d. Reiniciando.\n", pinPresionado, ordenEsperado[paso]);
                nivel = 0;
                vTaskDelay(2000 / portTICK_PERIOD_MS);
                goto siguiente_intento;
            }

            gpio_set_level(leds[pinPresionado], 1);
            vTaskDelay(ESPERA / portTICK_PERIOD_MS);
            gpio_set_level(leds[pinPresionado], 0);
        }

        nivel++;
        if (nivel >= LONGITUD_SECUENCIA) {
            printf("¡Felicidades! Completaste toda la secuencia.\n");
            nivel = 0;
            vTaskDelay(3000 / portTICK_PERIOD_MS);
        }

    siguiente_intento:
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void app_main() {
    configurarPines();
    configurarTactiles();
    printf("Juego de memoria corregido y verificando la secuencia.\n");
    xTaskCreate(&juegoMemoria, "JuegoMemoria", 4096, NULL, 5, NULL);
}
