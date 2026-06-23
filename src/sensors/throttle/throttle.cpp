#include "throttle.h"

// Valores Privados
namespace {
    struct Config {
        float voltageMin = 0.0f;
        float voltageMax = 0.0f;
    };

    struct Data {
        float volts = 0.0f;
        float pct = 0.0f;
    };

    struct InternalData {
        float filtered = 0.0f;
    };

    Config config;
    Data data;
    InternalData internal;

    // Defaults
    constexpr float VOLTAGE_MIN = 1.10f; // Buscando valores!!!
    constexpr float VOLTAGE_MAX = 4.25f; // Buscando valores!!!

    // Fault
    constexpr float VOLTAGE_FAULT_LOW  = 0.30f;  // 0.05 é o limite físico
    constexpr float VOLTAGE_FAULT_HIGH = 4.90f;  // 4.95 é o limite físico

    float readFilteredVoltage(uint8_t pin) {
        int raw = analogRead(pin);

        // Voltar com filtragem após calibração
        // internal.filtered = (internal.filtered*3 + raw*5) / 8;
        float v_adc = (
            //internal.filtered 
            raw * HAL::Adc::VREF_VOLTS) / HAL::Adc::MAX_VALUE;
        return v_adc;
    }

    float voltageToPct(float v) {
        float pct = (v - config.voltageMin) / (config.voltageMax - config.voltageMin);

        if (!isfinite(pct))
            pct = 0.0f;

        return constrain(pct, 0.0f, 1.0f) * 100.0f;
    }

    void setData() {
        float voltage = readFilteredVoltage(Pins::THROTTLE);
        
        bool fault = (voltage < VOLTAGE_FAULT_LOW) || (voltage > VOLTAGE_FAULT_HIGH);
        float pedalPct = fault ? 0.0f : voltageToPct(voltage);
        
        data.volts = voltage;
        data.pct = pedalPct;
    }

    // ==================================================
    // ------------------- CALIBRAÇÃO -------------------
    // ==================================================

    const int MAX_SAMPLES = 500;

    // Média das samples
    int compararFloat(const void* a, const void* b) {
        float fa = *(const float*)a;
        float fb = *(const float*)b;
        return (fa > fb) - (fa < fb);
    }

    float calcularMediaAparada(float* samples, int tamanho, float percentualDescartar = 0.10f) {
        // Ordena as samples
        qsort(samples, tamanho, sizeof(float), compararFloat);
        
        // Descarta os 'percentualDescartar' % menores e maiores
        int descartar = (int)(tamanho * percentualDescartar);
        int inicio = descartar;
        int fim = tamanho - descartar;
        
        // Calcula a média dos valores centrais
        float soma = 0;
        for (int i = inicio; i < fim; i++) {
            soma += samples[i];
        }
        int n = fim - inicio;
        if (n <= 0) 
            return 0.0f;
        
        return soma / n;
    }

    float calibrarMinimo(int tempoMs = 3000){
        float samples[MAX_SAMPLES];
        int count = 0;
        
        Serial.println("Coletando valores mínimos.");
        Serial.println("Solte o pedal completamente.");
        uint32_t inicio = millis();

        while (millis() - inicio < tempoMs / 2 && count < MAX_SAMPLES) {
            samples[count++] = readFilteredVoltage(Pins::THROTTLE);
            delay(2); // 2ms entre samples → ~500 samples em 1s
        }
        
        float minCalibrado = calcularMediaAparada(samples, count);
        return minCalibrado + 0.01; // Offset contra ruído
    }

    float calibrarMaximo(int tempoMs = 3000){
        float samples[MAX_SAMPLES];
        int count = 0;
        
        Serial.println("Coletando valores máximos.");
        Serial.println("Pressione o pedal completamente.");
        uint32_t inicio = millis();

        while (millis() - inicio < tempoMs / 2 && count < MAX_SAMPLES) {
            samples[count++] = readFilteredVoltage(Pins::THROTTLE);
            delay(2); // 2ms entre samples → ~500 samples em 1s
        }
        
        float maxCalibrado = calcularMediaAparada(samples, count);
        return maxCalibrado - 0.01; // Offset contra ruído
    }

    // Calibração
    void calibrarPedal() {        
        // ZERA os valores antigos!
        config.voltageMin = 5.0f;
        config.voltageMax = 0.0f;
        
        Serial.println("=== CALIBRACAO DO PEDAL ===");

        // --- ETAPA 1: Calibrar Mínimo ---
        Serial.println("1. CERTIFIQUE-SE de que o pedal está TOTALMENTE SOLTO.");
        Serial.println("   Digite 'C' e pressione Enter para iniciar a coleta do MINIMO.");
        
        // Aguarda o comando 'C'
        while (!Serial.available()) {
            delay(100);
        }
        char cmd = Serial.read();
        // Limpa o buffer (descarta \r\n)
        while (Serial.available()) Serial.read();
        
        if (cmd != 'C' && cmd != 'c') {
            Serial.println("Comando invalido. Cancelando calibracao.");
            return;
        }
        
        // Executa a calibração do mínimo
        float minVal = calibrarMinimo();
        config.voltageMin = minVal;
        
        Serial.print("MINIMO calibrado: ");
        Serial.print(minVal, 3);
        Serial.println(" V\n");
        
        // --- ETAPA 2: Calibrar Máximo ---
        Serial.println("2. AGORA, pressione o pedal TOTALMENTE e mantenha.");
        Serial.println("   Digite 'C' e pressione Enter para iniciar a coleta do MAXIMO.");
        
        // Aguarda o comando 'C'
        while (!Serial.available()) {
            delay(100);
        }
        cmd = Serial.read();
        while (Serial.available()) Serial.read();
        
        if (cmd != 'C' && cmd != 'c') {
            Serial.println("Comando invalido. Cancelando calibracao.");
            return;
        }
        
        // Executa a calibração do máximo
        float maxVal = calibrarMaximo();
        config.voltageMax = maxVal;
        
        Serial.print("MAXIMO calibrado: ");
        Serial.print(maxVal, 3);
        Serial.println(" V\n");
        
        // --- ETAPA 3: Validação e relatório final ---
        float faixa = config.voltageMax - config.voltageMin;
        
        Serial.println("========================================");
        Serial.println("        CALIBRACAO CONCLUIDA!");
        Serial.println("========================================");
        Serial.print("MIN (pedal solto):       ");
        Serial.print(config.voltageMin, 3);
        Serial.println(" V");
        Serial.print("MAX (pedal pressionado): ");
        Serial.print(config.voltageMax, 3);
        Serial.println(" V");
        Serial.print("Faixa de trabalho:       ");
        Serial.print(faixa, 3);
        Serial.println(" V");
        
        // Verifica se a faixa é razoável
        if (faixa < 0.5f) {
            Serial.println("\nATENCAO: Faixa muito pequena! Verifique o pedal.");
            Serial.println("   Usando valores padrao como fallback.");
            Throttle::defaultValue();
        } else {
            Serial.println("\nFaixa OK. Os valores podem ser usados com seguranca.");
        }
        Serial.println("========================================\n");
    }
}

namespace Throttle {
    // Main
    void setup() {
        Throttle::defaultValue(); // Valores padrão iniciais
    
        Serial.println("Digite 'C' e pressione Enter para calibrar.");
        
        // Aguarda o comando do usuário
        while (!Serial.available()) {
            delay(100);
        }
        
        char cmd = Serial.read();
        if (cmd == 'C' || cmd == 'c') {
            calibrarPedal();
        }
    }

    void loop() {
        // Verifica se o usuário enviou um comando
        if (Serial.available()) {
            char cmd = Serial.read();
            // Limpa o buffer (descarta \r\n)
            while (Serial.available()) Serial.read();
            
            if (cmd == 'C' || cmd == 'c') {
                calibrarPedal(); // Chama a orquestradora
            } else {
                Serial.println("Comando invalido. Digite 'C' para calibrar.");
            }
        }
        
        // getData();
        
        delay(10);
    }

    // Getters
    float getVolts() {
        return data.volts;
    }

    float getPct() {
        return data.pct;
    }

    float getVoltageMin() {
        return config.voltageMin;
    }

    float getVoltageMax() {
        return config.voltageMax;
    }

    // Setters
    void setVoltageMin(float value) {
        config.voltageMin = value;
    }
    
    void setVoltageMax(float value) {
        config.voltageMax = value;
    }

    // Default
    void defaultValue() {
        setVoltageMin(VOLTAGE_MIN);
        setVoltageMax(VOLTAGE_MAX);
    }
}