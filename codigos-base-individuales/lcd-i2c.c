/*
    Objetivo:
    Mostrar mensaje en LCD por I2C.

    Librería basada del siguiente enlace:
    https://github.com/maxsydney/ESP32-HD44780

    Modificaciones con respecto a la librería origial:
    - Modificado para LCD de 16x2:
    - Reemplazo de portTICK_RATE_MS por portTICK_PERIOD_MS

    Simulación:
    https://wokwi.com/projects/363400665077389313
*/

#include <driver/i2c.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
#include "sdkconfig.h"
#include "HD44780.h"

#define LCD_ADDR 0x27
#define SDA_PIN  21
#define SCL_PIN  22
#define LCD_COLS 16
#define LCD_ROWS 2

static char tag[] = "LCD test";
void LCD_DemoTask(void* param);

void app_main(void)
{
    ESP_LOGI(tag, "Starting up application");
    LCD_init(LCD_ADDR, SDA_PIN, SCL_PIN, LCD_COLS, LCD_ROWS);
    xTaskCreate(&LCD_DemoTask, "Demo Task", 2048, NULL, 5, NULL);
}

void LCD_DemoTask(void* param)
{
    char txtBuf[8];
    while (true) {

        LCD_home();

        LCD_clearScreen();
        LCD_setCursor(0, 0);
        LCD_writeStr("Sistemas");
        LCD_setCursor(0, 1);
        LCD_writeStr("Embebidos 1");
        vTaskDelay(2000 / portTICK_PERIOD_MS);

        LCD_clearScreen();
        LCD_setCursor(0, 0);

        for (int i = 10; i >= 0; i--)
        {
            LCD_setCursor(0, 0);
            sprintf(txtBuf, "%02d", i);
            printf(txtBuf);
            LCD_writeStr(txtBuf);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }

        for (int i = 0; i < LCD_COLS; i++)
        {
            for (int j=0; j < LCD_ROWS; j++)
            {
                LCD_clearScreen();
                LCD_setCursor(i, j);
                LCD_writeChar('*');
                vTaskDelay(100 / portTICK_PERIOD_MS);
            }
        }
    }
}