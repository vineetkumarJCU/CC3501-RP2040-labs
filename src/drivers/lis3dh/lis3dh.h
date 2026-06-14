#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "boards.h"

// Register addresses and values

#define LIS3DH_WHO_AM_I       0x0F
#define LIS3DH_WHO_AM_I_VALUE 0x33

#define LIS3DH_CTRL_REG1      0x20
#define LIS3DH_CTRL_REG4      0x23

#define LIS3DH_OUT_X_L        0x28
#define LIS3DH_AUTO_INC       0x80

#define LIS3DH_CTRL_REG1_VAL  0x57
#define LIS3DH_CTRL_REG4_VAL  0x88

// Data structures and function prototypes

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} lis3dh_raw_t;

typedef struct {
    float x;
    float y;
    float z;
} lis3dh_data_t;

// Internal helper functions

bool lis3dh_init(void);
void lis3dh_read_raw(lis3dh_raw_t *out);
void lis3dh_convert(const lis3dh_raw_t *raw, lis3dh_data_t *out);