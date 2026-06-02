#include <stdio.h>
#include "pico/stdlib.h"

#include "drivers/logging/logging.h"
#include "drivers/leds.h"

#define LED_PIN 14

int main()
{
    stdio_init_all();

    leds_init(pio0, 0, LED_PIN);

    while (true)
    {
        log(LogLevel::INFORMATION, "Lab 2 LED Driver Demo");

        // Requirement 3: turn all LEDs off
        leds_clear();
        leds_commit();
        sleep_ms(1000);

        // Requirement 1:
        // Driver remembers previous LED states
        for (uint i = 0; i < NUM_LEDS; i++)
        {
            switch (i % 6)
            {
                case 0:
                    leds_set(i, 255, 0, 0);     // Red
                    break;

                case 1:
                    leds_set(i, 0, 255, 0);     // Green
                    break;

                case 2:
                    leds_set(i, 0, 0, 255);     // Blue
                    break;

                case 3:
                    leds_set(i, 255, 255, 0);   // Yellow
                    break;

                case 4:
                    leds_set(i, 255, 0, 255);   // Purple
                    break;

                case 5:
                    leds_set(i, 0, 255, 255);   // Cyan
                    break;
            }

            leds_commit();
            sleep_ms(500);
        }

        // Requirement 2:
        // Multiple changes before one commit
        leds_set(0, 255, 255, 255);
        leds_set(1, 255, 255, 255);
        leds_set(2, 255, 255, 255);
        leds_set(3, 255, 255, 255);

        // Nothing changes until this commit
        leds_commit();

        sleep_ms(2000);

        // Requirement 3 again
        leds_clear();
        leds_commit();

        sleep_ms(2000);
    }

    return 0;
}