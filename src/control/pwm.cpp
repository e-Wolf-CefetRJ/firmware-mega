#include "pwm.h"

namespace {
    constexpr uint16_t PWM_FREQ_HZ = 1000;

    struct Config {
        uint16_t frequency = 0; // 100 até 8000
    };
    Config config;
}

namespace PWM {
    // Reset
    void defaultValue() {
        config.frequency = PWM_FREQ_HZ;
    }

    // Helpers
    void setPct(float pct){
        pct=constrain(pct,0,100);

        uint16_t top = ICR1;  // Período total do PWM
        uint16_t val = (uint16_t) ((pct/100.0f)*(float)top);
        
        if(val>top) 
            val=top;    
        
        OCR1A=val;  // Define o valor que vai mudar de High para Low (gera a onda quadrada)
        // Quando não invertido, 0% é Low e 100% é High
    }

    void applyFrequency(uint16_t hz){
        setFrequency(hz);
        hz = getFrequency(); // Valor pós constrain

        uint16_t cs_bits=0x01;
        uint32_t top=(F_CPU/(1UL*hz))-1;    // F_CPU = 16MHz 

        if(top>65535UL) { 
            cs_bits=0x02; 
            top=(F_CPU/(8UL*hz))-1; 
        }
        if(top>65535UL) { 
            cs_bits=0x03; 
            top=(F_CPU/(64UL*hz))-1; 
        }
        if(top>65535UL) { 
            top=65535UL; 
        }

        TCCR1A=0; 
        TCCR1B=0;
        
        TCCR1A|=(1<<WGM11)|(1<<COM1A1);
        // Invertido:
        // TCCR1A |= (1 << WGM11) | (1 << COM1A1) | (1 << COM1A0);
        TCCR1B|=(1<<WGM12)|(1<<WGM13);
        
        ICR1=(uint16_t)top; 
        OCR1A=0;
        
        TCCR1B|=cs_bits;
    }

    // Setters
    void setFrequency(uint16_t hz) {
        config.frequency = constrain(hz, 100, 8000);
    }

    // Getters
    uint16_t getFrequency() {
        return config.frequency;
    }

    // Main
    void setup() {
        pinMode(Pins::PWM_OUT,OUTPUT);
        defaultValue();
        applyFrequency(getFrequency());
        setPct(0);
    }

    void loop(float dutyNowPct) {
        setPct(dutyNowPct);
    }
}