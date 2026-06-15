#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define BUTTON_PIN 15

int main()
{
    stdio_init_all();

    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);

    while (true)
    {
        printf("Button = %d\n", gpio_get(BUTTON_PIN));
        sleep_ms(200);
    }
}