#include "drivers/leds.h"
#include "WS2812.pio.h"

static PIO led_pio;
static uint led_sm;

static uint32_t led_buffer[NUM_LEDS];
static bool dirty = false;

static uint32_t make_colour(uint8_t red, uint8_t green, uint8_t blue)
{
    return ((uint32_t)red << 24) |
           ((uint32_t)green << 16) |
           ((uint32_t)blue << 8);
}

void leds_init(PIO pio, uint sm, uint pin)
{
    led_pio = pio;
    led_sm = sm;

    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, pin, 800000, false);

    leds_clear();
    leds_commit();
}

void leds_set(uint index, uint8_t red, uint8_t green, uint8_t blue)
{
    if (index >= NUM_LEDS) {
        return;
    }

    led_buffer[index] = make_colour(red, green, blue);
    dirty = true;
}

void leds_commit()
{
    for (uint i = 0; i < NUM_LEDS; i++) {
        pio_sm_put_blocking(led_pio, led_sm, led_buffer[i]);
    }

    dirty = false;
}

void leds_clear()
{
    for (uint i = 0; i < NUM_LEDS; i++) {
        led_buffer[i] = 0;
    }

    dirty = true;
}

bool leds_dirty()
{
    return dirty;
}