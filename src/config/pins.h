#pragma once

#include <Arduino.h>

namespace Pins {
    // Sensores
    inline constexpr uint8_t THROTTLE     = A0;   // aceleração, Mega novo, podemos usar A0 novamente
    inline constexpr uint8_t RPM          = A8;   // rotação por minuto, PCINT2 / PK0
    inline constexpr uint8_t IBAT         = A2;   // corrente da bateria
    inline constexpr uint8_t IMOT         = A3;   // corrente do motor
    inline constexpr uint8_t DHT22        = 4;    // temperatura/umidade
    
    // Controlador do Motor
    inline constexpr uint8_t PWM_OUT      = 11;   // Controle do motor, Timer1 
    
    // Relacionados ao SD
    inline constexpr uint8_t SD_CS        = 10;   // Logger
    inline constexpr uint8_t SPI_MISO     = 50;   // Lê dados
    inline constexpr uint8_t SPI_MOSI     = 51;   // Escreve dados
    inline constexpr uint8_t SPI_SCK      = 52;   // Clock do SD
    inline constexpr uint8_t SPI_SS       = 53;   // Configuração do barramento, Deve ser OUTPUT para modo mestre SPI


    // RTC DS1302
    inline constexpr uint8_t RTC_DATA     = 22;
    inline constexpr uint8_t RTC_CLOCK    = 23;
    inline constexpr uint8_t RTC_RESET    = 24;

    // PCINT (Relacionados ao RPM, não são pinos físicos)
    inline constexpr uint8_t PCINT_PIN    = PK0;           // Bit 0 do registrador PINK
    inline constexpr uint8_t PCINT_MASK   = _BV(PCINT16);  // Constante que identifica interrupções
    inline constexpr uint8_t PCIE_NUM     = PCIE2;         // Bit 2 do registrador PCICR
}