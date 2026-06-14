#include <stdio.h>
#include "pico/stdlib.h"

#include "boards.h"
#include "drivers/leds.h"
#include "tasks/spirit_level.h"

int main()
{
    stdio_init_all();

    // Initialize LEDs before starting the spirit level task
    leds_init(LED_PIO, LED_SM, LED_PIN);

    spirit_level_task();

    return 0;
}