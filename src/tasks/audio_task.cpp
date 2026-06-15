#include "audio_task.h"

#include <stdio.h>
#include <stdint.h>

#include "arm_math.h"
#include "pico/stdlib.h"

#include "boards.h"
#include "drivers/microphone/microphone.h"
#include "drivers/leds.h"

#define FFT_LENGTH 1024
#define FFT_BINS 512

static uint16_t adc_samples[FFT_LENGTH];
static q15_t time_signal[FFT_LENGTH];
static q15_t fft_output[FFT_LENGTH];
static q15_t mag_squared[FFT_BINS];

static arm_rfft_instance_q15 fft_instance;

static const uint16_t band_edges[13] = {
    6, 8, 11, 16, 24, 35, 51, 75, 110, 161, 237, 349, 513
};

static void apply_simple_hanning_window()
{
    for (int i = 0; i < FFT_LENGTH; i++) {
        int32_t ramp;

        if (i < FFT_LENGTH / 2) {
            ramp = (i * 32767) / (FFT_LENGTH / 2);
        } else {
            ramp = ((FFT_LENGTH - 1 - i) * 32767) / (FFT_LENGTH / 2);
        }

        int32_t value = (int32_t)time_signal[i] * ramp;
        time_signal[i] = (q15_t)(value >> 15);
    }
}

static void display_audio_on_leds()
{
    leds_clear();

    for (int led = 0; led < NUM_LEDS; led++) {
        uint16_t start_bin = band_edges[led];
        uint16_t end_bin = band_edges[led + 1];

        int32_t energy = 0;

        for (uint16_t bin = start_bin; bin < end_bin; bin++) {
            energy += mag_squared[bin];
        }

        if (energy > 20) {
            if (led < 4) {
                leds_set(led, 0, 0, 255);       // low frequencies blue
            } else if (led < 8) {
                leds_set(led, 0, 255, 0);       // middle frequencies green
            } else {
                leds_set(led, 255, 0, 0);       // high frequencies red
            }
        }
    }

    leds_commit();
}

void audio_task(void)
{
    microphone_init();

    arm_status status = arm_rfft_init_q15(&fft_instance, FFT_LENGTH, 0, 1);

    if (status != ARM_MATH_SUCCESS) {
        printf("FFT init failed\n");
        while (true) {
            leds_clear();
            leds_commit();
            sleep_ms(500);
        }
    }

    printf("Audio FFT task started\n");

    while (true) {
        microphone_read(adc_samples, FFT_LENGTH);

        int32_t sum = 0;

        for (int i = 0; i < FFT_LENGTH; i++) {
            sum += adc_samples[i];
        }

        int32_t dc_bias = sum / FFT_LENGTH;

        for (int i = 0; i < FFT_LENGTH; i++) {
            int32_t centred = (int32_t)adc_samples[i] - dc_bias;

            centred = centred << 5;

            if (centred > 32767) {
                centred = 32767;
            }

            if (centred < -32768) {
                centred = -32768;
            }

            time_signal[i] = (q15_t)centred;
        }

        apply_simple_hanning_window();

        arm_rfft_q15(&fft_instance, time_signal, fft_output);

        arm_cmplx_mag_squared_q15(fft_output, mag_squared, FFT_BINS);

        display_audio_on_leds();

        printf("Audio FFT updated. DC bias=%ld\n", dc_bias);

        sleep_ms(100);
    }
}