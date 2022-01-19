//Display temperature and humidity data example

#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <hd44780.h>
#include <dht.h>

static const dht_sensor_type_t sensor_type = DHT_TYPE_AM2301;
#if defined(CONFIG_IDF_TARGET_ESP8266)
static const gpio_num_t dht_gpio = 4;
#else
static const gpio_num_t dht_gpio = 22;
#endif


static const uint8_t char_data[] = {
    0x04, 0x0e, 0x0e, 0x0e, 0x1f, 0x00, 0x04, 0x00,
    0x1f, 0x11, 0x0a, 0x04, 0x0a, 0x11, 0x1f, 0x00
};

void lcd_test(void *pvParameters)
{
    int16_t temperature = 0;
    int16_t humidity = 0;

    hd44780_t lcd = {
        .write_cb = NULL,
        .font = HD44780_FONT_5X8,
        .lines = 2,
        .pins = {
            .rs = GPIO_NUM_19,
            .e  = GPIO_NUM_23,
            .d4 = GPIO_NUM_18,
            .d5 = GPIO_NUM_17,
            .d6 = GPIO_NUM_16,
            .d7 = GPIO_NUM_0,
            .bl = HD44780_NOT_USED
        }
    };

    ESP_ERROR_CHECK(hd44780_init(&lcd));
    hd44780_upload_character(&lcd, 0, char_data);
    hd44780_upload_character(&lcd, 1, char_data + 8); 
  

    while (1)
    {
        if (dht_read_data(sensor_type, dht_gpio, &humidity, &temperature) == ESP_OK)
            printf("Humidity: %d%% Temp: %dC\n", humidity / 10, temperature / 10);

        else
            printf("Could not read data from sensor\n");

        // If you read the sensor data too often, it will heat up
        // http://www.kandrsmith.org/RJS/Misc/Hygrometers/dht_sht_how_fast.html
        char str[100];
        char str2[100];
        sprintf(str, "%s%d%s", "Humidity: ", humidity/10, "%");
        sprintf(str2, "%s%d%s","Temperature: ", temperature/10, "%");
        hd44780_gotoxy(&lcd, 0, 0);
        hd44780_puts(&lcd, str);
        hd44780_gotoxy(&lcd, 0, 1);
        hd44780_puts(&lcd, str2);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

void app_main()
{
    xTaskCreate(lcd_test, "lcd_test", configMINIMAL_STACK_SIZE * 3, NULL, 5, NULL);
}
