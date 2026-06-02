#pragma once

#include <stdint.h>
#include "hardware/pio.h"

#define NUM_LEDS 12

void leds_init(PIO pio, uint sm, uint pin);
void leds_set(uint index, uint8_t red, uint8_t green, uint8_t blue);
void leds_commit();
void leds_clear();
bool leds_dirty();