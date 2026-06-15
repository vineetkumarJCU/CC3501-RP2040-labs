#pragma once

#include "hardware/i2c.h"
#include "hardware/pio.h"

#define LED_PIN 14
#define NUM_LEDS 12
#define LED_PIO pio0
#define LED_SM 0
#define MIC_ADC_PIN 26
#define MIC_ADC_INPUT 0

#define BUTTON_PIN 15

// LIS3DH I2C configuration
#define LIS3DH_I2C_PORT i2c0
#define LIS3DH_PIN_SDA 16
#define LIS3DH_PIN_SCL 17
#define LIS3DH_ADDR 0x19