/*
    Objetivo:
    Encender un LED cuando se detecte que se toca un pin táctil.
*/

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/touch_pad.h"
#include "driver/gpio.h"

#define LED     1
#define T3      TOUCH_PAD_NUM3
#define LIMITE  450
#define ESPERA  50

// Variable para guardar el valor del pin tactil
uint16_t touch_value;

void app_main()
{
    // Config led
    gpio_reset_pin(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);

    // Inicializar el subsistema de pines táctiles
    touch_pad_init();

    // Configurar los pines táctiles que se van a utilizar
    touch_pad_config(T3, 0);

    while(1)
    {
        // Guardar el valor del pin tactil en la variable touch_value
        touch_pad_read(T3, &touch_value);

        // Encender led de acuerdo al tactil
        if (touch_value > LIMITE) gpio_set_level(LED, 1);
        else                      gpio_set_level(LED, 0);

        // Esperar N ms antes de leer de nuevo
        vTaskDelay( ESPERA / portTICK_PERIOD_MS);
    }
}