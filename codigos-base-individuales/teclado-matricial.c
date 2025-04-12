#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>

#define ROWS 4
#define COLS 4

// Definir la distribución de teclas
char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

// Pines de las filas y columnas
int rowPins[ROWS] = {22, 21, 18, 17};
int colPins[COLS] = {16, 0, 2, 15};

void configurarPines()
{
    for (int i = 0; i < ROWS; i++)
    {
        gpio_set_direction(rowPins[i], GPIO_MODE_OUTPUT);
        gpio_set_level(rowPins[i], 1);
    }
    for (int i = 0; i < COLS; i++)
    {
        gpio_set_direction(colPins[i], GPIO_MODE_INPUT);
        gpio_pullup_en(colPins[i]);
    }
}

char leerTecla()
{
    for (int i = 0; i < ROWS; i++)
    {
        gpio_set_level(rowPins[i], 0);
        for (int j = 0; j < COLS; j++)
        {
            if (gpio_get_level(colPins[j]) == 0)
            {
                while (gpio_get_level(colPins[j]) == 0);
                gpio_set_level(rowPins[i], 1);
                return keys[i][j];
            }
        }
        gpio_set_level(rowPins[i], 1);
    }
    return 0;
}

void tecladoTask(void *pvParameter)
{
    while (1)
    {
        char key = leerTecla();
        if (key)
        {
            printf("Tecla presionada: %c\n", key);
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void app_main()
{
    configurarPines();
    xTaskCreate(&tecladoTask, "tecladoTask", 2048, NULL, 5, NULL);
}
