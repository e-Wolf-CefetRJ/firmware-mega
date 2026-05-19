#pragma once

#include <Arduino.h>

// Constantes compartilhadas
namespace HAL::Adc {
    inline constexpr int   MAX_VALUE  = 1023;
    inline constexpr float VREF_VOLTS = 5.0f;
}