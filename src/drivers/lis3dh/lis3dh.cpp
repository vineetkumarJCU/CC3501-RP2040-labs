#include <stdio.h>
#include "lis3dh.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"

static void lis3dh_write_reg(uint8_t reg, uint8_t value)
{
    uint8_t buf[2] = {reg, value};
    i2c_write_blocking(LIS3DH_I2C_PORT, LIS3DH_ADDR, buf, 2, false);
}

static uint8_t lis3dh_read_reg(uint8_t reg)
{
    uint8_t value = 0;

    i2c_write_blocking(LIS3DH_I2C_PORT, LIS3DH_ADDR, &reg, 1, true);
    i2c_read_blocking(LIS3DH_I2C_PORT, LIS3DH_ADDR, &value, 1, false);

    return value;
}

bool lis3dh_init(void)
{
    // Initialize I2C at 400 kHz
    i2c_init(LIS3DH_I2C_PORT, 400000);

    // Configure GPIO pins for I2C (SDA and SCL)
    gpio_set_function(LIS3DH_PIN_SDA, GPIO_FUNC_I2C);
    gpio_set_function(LIS3DH_PIN_SCL, GPIO_FUNC_I2C);

    // Enable pull-up resistors on I2C lines
    gpio_pull_up(LIS3DH_PIN_SDA);
    gpio_pull_up(LIS3DH_PIN_SCL);

    sleep_ms(10);

    // Read WHO_AM_I register to verify communication
    uint8_t who = lis3dh_read_reg(LIS3DH_WHO_AM_I);

    if (who != LIS3DH_WHO_AM_I_VALUE) {
        printf("LIS3DH FAILED. Expected 0x%02X, got 0x%02X\n",
               LIS3DH_WHO_AM_I_VALUE, who);
        return false;
    }

    printf("LIS3DH OK. WHO_AM_I = 0x%02X\n", who);

    lis3dh_write_reg(LIS3DH_CTRL_REG1, LIS3DH_CTRL_REG1_VAL);
    lis3dh_write_reg(LIS3DH_CTRL_REG4, LIS3DH_CTRL_REG4_VAL);

    return true;
}

void lis3dh_read_raw(lis3dh_raw_t *out)
{
    uint8_t reg = LIS3DH_OUT_X_L | LIS3DH_AUTO_INC;
    uint8_t buf[6];

    i2c_write_blocking(LIS3DH_I2C_PORT, LIS3DH_ADDR, &reg, 1, true);
    i2c_read_blocking(LIS3DH_I2C_PORT, LIS3DH_ADDR, buf, 6, false);

    int16_t raw_x = (int16_t)((buf[1] << 8) | buf[0]);
    int16_t raw_y = (int16_t)((buf[3] << 8) | buf[2]);
    int16_t raw_z = (int16_t)((buf[5] << 8) | buf[4]);

    out->x = raw_x >> 4;
    out->y = raw_y >> 4;
    out->z = raw_z >> 4;
}

void lis3dh_convert(const lis3dh_raw_t *raw, lis3dh_data_t *out)
{
    out->x = raw->x * 0.001f;
    out->y = raw->y * 0.001f;
    out->z = raw->z * 0.001f;
}