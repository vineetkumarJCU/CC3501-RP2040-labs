#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include "boards.h"
#include "drivers/leds.h"
#include "tasks/spirit_level.h"
#include "tasks/audio_task.h"

enum Mode {
    MODE_LED_DEMO = 0,
    MODE_SPIRIT_LEVEL = 1,
    MODE_AUDIO_FFT = 2
};

static Mode current_mode = MODE_LED_DEMO;

static bool button_pressed()
{
    static bool last_state = false;
    bool current_state = gpio_get(BUTTON_PIN);

    if (current_state && !last_state) {
        sleep_ms(50);
        last_state = current_state;
        return true;
    }

    last_state = current_state;
    return false;
}

// Lab 2 LED demo mode, cycles through different colors on the LEDs
static void lab2_led_demo_step()
{
    static int stage = 0;
    static uint led = 0;

    if (stage == 0)
    {
        leds_clear();
        leds_commit();
        sleep_ms(1000);

        led = 0;
        stage = 1;
        return;
    }

    if (stage == 1)
    {
        switch (led % 6)
        {
            case 0: leds_set(led, 255, 0, 0); break;
            case 1: leds_set(led, 0, 255, 0); break;
            case 2: leds_set(led, 0, 0, 255); break;
            case 3: leds_set(led, 255, 255, 0); break;
            case 4: leds_set(led, 255, 0, 255); break;
            case 5: leds_set(led, 0, 255, 255); break;
        }

        leds_commit();
        sleep_ms(500);

        led++;

        if (led >= NUM_LEDS) {
            stage = 2;
        }

        return;
    }

    if (stage == 2)
    {
        leds_set(0, 255, 255, 255);
        leds_set(1, 255, 255, 255);
        leds_set(2, 255, 255, 255);
        leds_set(3, 255, 255, 255);

        leds_commit();
        sleep_ms(2000);

        stage = 3;
        return;
    }

    if (stage == 3)
    {
        leds_clear();
        leds_commit();
        sleep_ms(2000);

        stage = 0;
        return;
    }
}

// Function to change the current mode when the button is pressed

static void change_mode()
{
    current_mode = (Mode)((current_mode + 1) % 3);

    leds_clear();
    leds_commit();

    if (current_mode == MODE_LED_DEMO) {
        printf("Mode 0: Lab 2 LED Demo\n");
    }
    else if (current_mode == MODE_SPIRIT_LEVEL) {
        printf("Mode 1: Lab 3 Spirit Level\n");
        spirit_level_init();
    }
    else if (current_mode == MODE_AUDIO_FFT) {
        printf("Mode 2: Lab 4 Audio FFT\n");
        audio_init();
    }

    sleep_ms(300);
}

int main()
{
    stdio_init_all();

    // Initialize the LEDs and button GPIO
    leds_init(LED_PIO, LED_SM, LED_PIN);

    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);

    sleep_ms(1000);

    printf("Mode 0: Lab 2 LED Demo\n");

    while (true)
    {
        if (button_pressed()) {
            change_mode();
        }

        if (current_mode == MODE_LED_DEMO) {
            lab2_led_demo_step();
        }
        else if (current_mode == MODE_SPIRIT_LEVEL) {
            spirit_level_step();
        }
        else if (current_mode == MODE_AUDIO_FFT) {
            audio_step();
        }
    }

    return 0;
}