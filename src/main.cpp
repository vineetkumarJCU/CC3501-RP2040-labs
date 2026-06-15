#include <stdio.h>
#include "pico/stdlib.h"

#include "drivers/microphone/microphone.h"

#define SAMPLE_COUNT 1024

int main()
{
    stdio_init_all();
    sleep_ms(2000);

    microphone_init();

    static uint16_t samples[SAMPLE_COUNT];

    while (true)
    {
        microphone_read(samples, SAMPLE_COUNT);

        uint32_t sum = 0;
        uint16_t min_value = 4095;
        uint16_t max_value = 0;

        for (int i = 0; i < SAMPLE_COUNT; i++)
        {
            sum += samples[i];

            if (samples[i] < min_value) {
                min_value = samples[i];
            }

            if (samples[i] > max_value) {
                max_value = samples[i];
            }
        }

        uint32_t mean = sum / SAMPLE_COUNT;

        printf("Mic mean=%lu min=%u max=%u\n",
               mean, min_value, max_value);

        sleep_ms(500);
    }

    return 0;
}