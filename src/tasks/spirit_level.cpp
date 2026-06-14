#include <stdio.h>
#include "pico/stdlib.h"

#include "spirit_level.h"
#include "drivers/lis3dh/lis3dh.h"
#include "drivers/leds.h"
#include "boards.h"

#define TILT_THRESHOLD 0.10f

// Show spirit level status on LEDs based on X-axis tilt
static void show_level(float x)
{
    // Clear all previous LEDs first
    leds_clear();

    // If tilted right, light up right half in blue
    if (x > TILT_THRESHOLD) {
        for (int i = 6; i < NUM_LEDS; i++) {
            leds_set(i, 0, 0, 255);
        }
    }
    // If tilted left, light up left half in red
    else if (x < -TILT_THRESHOLD) {
        for (int i = 0; i < 6; i++) {
            leds_set(i, 255, 0, 0);
        }
    }
    // If level, light up middle LEDs in green
    else {
        leds_set(5, 0, 255, 0);
        leds_set(6, 0, 255, 0);
    }

    leds_commit();
}

void spirit_level_task(void)
{
    if (!lis3dh_init()) {
        while (true) {
            leds_clear();

            for (int i = 0; i < NUM_LEDS; i++) {
                leds_set(i, 255, 0, 0);
            }

            leds_commit();
            sleep_ms(300);

            leds_clear();
            leds_commit();
            sleep_ms(300);
        }
    }

    lis3dh_raw_t raw;
    lis3dh_data_t accel;

    while (true) {
        lis3dh_read_raw(&raw);
        lis3dh_convert(&raw, &accel);

        printf("Raw X=%d Y=%d Z=%d | g X=%.3f Y=%.3f Z=%.3f\n",
               raw.x, raw.y, raw.z,
               accel.x, accel.y, accel.z);

        show_level(accel.x);

        sleep_ms(200);
    }
}