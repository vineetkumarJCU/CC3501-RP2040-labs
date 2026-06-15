#pragma once

#include <cstdint>

void microphone_init();
void microphone_read(std::uint16_t *buffer, std::uint32_t sample_count);