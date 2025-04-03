/*
simulación WOKWI:
https://wokwi.com/projects/426613926979823617

*/


#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define SA 5
#define SB 4
#define SC 19
#define SD 25
#define SE 26
#define SF 33
#define SG 32
#define SP 14
#define WAIT 10
#define LSB 13
#define MSB 12
#define SUM 16
#define RES 17

// Arreglo para los pines del segmento
int segPins[] = {SA, SB, SC, SD, SE, SF, SG, SP};

// Arreglo de los segmentos para mostrar del 0 a la F
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
    {1, 1, 1, 1, 1, 1, 1, 1}, // Fondo negro
};

// Función para mostrar en el 7 segmentos un dígito
void mostrar(int digit)
{
    for (int a = 0; a < 8; a++)
    {
        gpio_set_level(segPins[a], segCode[digit][a]);
    }
}

void setup()
{
    // Configurar pines de los segmentos como salida
    for (int i = 0; i < 8; i++)
    {
        gpio_reset_pin(segPins[i]);
        gpio_set_direction(segPins[i], GPIO_MODE_OUTPUT);
    }

    gpio_reset_pin(LSB);
    gpio_set_direction(LSB, GPIO_MODE_OUTPUT); // LSB
    gpio_reset_pin(MSB);
    gpio_set_direction(MSB, GPIO_MODE_OUTPUT); // MSB

    gpio_set_direction(SUM, GPIO_MODE_INPUT);
    gpio_pulldown_en(SUM);
    gpio_set_direction(RES, GPIO_MODE_INPUT);
    gpio_pulldown_en(RES);
}

void mostrarDigito(int unidad, int decena)
{
    // Mostrar el dígito menos significativo (unidad)
    mostrar(16); // Apagar todo
    gpio_set_level(LSB, 1);
    gpio_set_level(MSB, 0);
    mostrar(unidad);
    vTaskDelay(WAIT / portTICK_PERIOD_MS);

    // Mostrar el dígito más significativo (decena)
    mostrar(16); // Apagar todo
    gpio_set_level(LSB, 0);
    gpio_set_level(MSB, 1);
    mostrar(decena);
    vTaskDelay(WAIT / portTICK_PERIOD_MS);
}

void app_main()
{
    setup(); // Configurar salidas

    while (1)
    {
        int num = 0;
        // Recorrer los valores del 1 al 100 en hexadecimal
        while (num < 100)
        {
            if (num >= 0)
            {
                int unidad = num % 10; // Obtener el dígito menos significativo
                int decena = num / 10; // Obtener el dígito más significativo

                if (gpio_get_level(SUM) == 1)
                {
                    printf("PUSH SUM: %d\n", gpio_get_level(SUM));
                    num++;
                    vTaskDelay(300 / portTICK_PERIOD_MS);
                    mostrarDigito(unidad, decena);
                }

                if (gpio_get_level(RES) == 1)
                {
                    printf("PUSH RES: %d\n", gpio_get_level(RES));
                    num--;
                    vTaskDelay(300 / portTICK_PERIOD_MS);
                    mostrarDigito(unidad, decena);
                }
                mostrarDigito(unidad, decena);
            }
            else
            {
                num = 0;
            }
        }
    }
}
