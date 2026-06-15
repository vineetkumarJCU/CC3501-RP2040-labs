#include "microphone.h"

#include "boards.h"
#include "hardware/adc.h"
#include "pico/stdlib.h"

#include <cstdint>

#define ADC_CLOCK_HZ 48000000.0f
#define SAMPLE_RATE_HZ 44100.0f

void microphone_init()
{
    adc_init();

    adc_gpio_init(MIC_ADC_PIN);
    adc_select_input(MIC_ADC_INPUT);

    adc_fifo_setup(
        true,
        false,
        1,
        false,
        false
    );

    float clkdiv = (ADC_CLOCK_HZ / SAMPLE_RATE_HZ) - 1.0f;
    adc_set_clkdiv(clkdiv);
}

void microphone_read(std::uint16_t *buffer, std::uint32_t sample_count)
{
    adc_fifo_drain();

    adc_run(true);

    for (std::uint32_t i = 0; i < sample_count; i++)
    {
        buffer[i] = adc_fifo_get_blocking();
    }

    adc_run(false);
    adc_fifo_drain();
}