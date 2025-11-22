# MedPulso - Monitor Cardíaco IoT

> Simulador de batimento cardíaco com controle remoto via MQTT e visualização em tempo real

[![Wokwi](https://img.shields.io/badge/Simular-Wokwi-green)](https://wokwi.com)
[![ESP32](https://img.shields.io/badge/ESP32-DevKit-blue)]()
[![MQTT](https://img.shields.io/badge/MQTT-HiveMQ-orange)]()
[![License](https://img.shields.io/badge/license-MIT-blue.svg)]()

---

## 📋 Sobre o Projeto

**MedPulso** é um monitor cardíaco IoT que simula batimentos cardíacos de forma realista através de LED e buzzer, com controle local via um medidor de pulso, e controle remoto via protocolo MQTT. Ideal para educação, demonstrações de IoT e conceitos de telemedicina.

### ✨ Funcionalidades

- 🫀 Simulação realista de batimento cardíaco (som "LUB-DUB")
- 📊 Monitoramento de BPM em tempo real (40-180 BPM)
- 🌐 Controle remoto via MQTT pela internet
- 💡 Feedback visual (LED) e sonoro (Buzzer)
- 📈 Classificação médica automática (bradicardia, normal, elevado, taquicardia)

---

## 🚀 Como Usar

### Reproduzir o Projeto

1. **Acesse o simulador Wokwi**
   - Clone este repositório
   - Abra o projeto no [Wokwi](https://wokwi.com)

2. **Carregue os arquivos**
   - `diagram.json` - Esquema de conexões
   - `sketch.ino` - Código fonte

3. **Inicie a simulação**
   - Clique em "Start Simulation"
   - Aguarde conexão Wi-Fi e MQTT

4. **Controle Local**
   - Acione o sensor para ajustar BPM (40-180)
   - Observe LED piscando e buzzer tocando

5. **Controle Remoto (MQTT)**
   - Acesse: [HiveMQ Web Client](http://www.hivemq.com/demos/websocket-client/)
   - Conecte ao broker: `broker.hivemq.com`
   - Publique comandos no tópico: `medpulso/comando`
     - `LIGAR` - Ativa o monitor
     - `DESLIGAR` - Desativa o monitor

---

## 💻 Software

### Estrutura do Código
```
MedPulso/
│
├── sketch.ino          # Código principal
├── diagram.json        # Configuração Wokwi
└── README.md          # Documentação
```

### Bibliotecas Utilizadas

| Biblioteca | Versão | Função |
|------------|--------|--------|
| `WiFi.h` | Nativa | Conexão Wi-Fi ESP32 |
| `PubSubClient.h` | 2.8.0 | Comunicação MQTT |

### Funções Principais
```cpp
setup()              // Inicialização do sistema
loop()               // Loop principal
conectarWiFi()       // Estabelece conexão Wi-Fi
reconectarMQTT()     // Mantém conexão MQTT
callback()           // Processa comandos MQTT
calcularBPM()        // Converte potenciômetro em BPM
simularBatimento()   // Gera padrão LUB-DUB
```

### Variáveis de Controle

- `monitorAtivo` (bool) - Estado ligado/desligado
- `bpm` (int) - Batimentos por minuto
- `intervaloBatimento` (int) - Tempo entre batimentos
- `ultimoBatimento` (unsigned long) - Timestamp

---

## 🔧 Hardware

### Plataforma de Desenvolvimento

- **ESP32 DevKit V1**
  - Microcontrolador: ESP32-WROOM-32
  - Clock: 240 MHz (dual-core)
  - Wi-Fi: 802.11 b/g/n
  - ADC: 12 bits (0-4095)
  - PWM: 16 canais

### Componentes

| Componente | Quantidade | Especificação |
|------------|------------|---------------|
| ESP32 | 1 | Placa de desenvolvimento |
| LED Vermelho | 1 | 5mm, difuso |
| Buzzer Piezoelétrico | 1 | Passivo, 3.3V |
| Sensor | 1 | 3.3V |

### Pinout
```
ESP32          Componente
─────────────────────────────
GPIO 5    →    LED (Anodo)
GND.1     →    LED (Catodo)

GPIO 18   →    Buzzer (+)
GND.2     →    Buzzer (-)

3V3       →    Sensor (VCC)
GPIO 35   →    Sensor (OUT0)
GND.1     →    Sensor (GND)
```

### Diagrama de Conexões
```
                  ╔═══════════════╗
                  ║    ESP32      ║
                  ╠═══════════════╣
    LED ────────→ ║ GPIO 5        ║
                  ║ GND.1         ║ ←──── LED (-)
                  ║               ║
    Buzzer ─────→ ║ GPIO 18       ║
                  ║ GND.2         ║ ←──── Buzzer (-)
                  ║               ║
    Sen (VCC) ──→ ║ 3V3           ║
    Sen (OUT0) ─→ ║ GPIO 35       ║
    Sen (GND) ──→ ║ GND.1         ║
                  ╚═══════════════╝
```

### Observações

- ⚠️ **Projeto virtual:** Simulado no Wokwi (sem hardware físico necessário)
- 💡 **Versão física:** Adicionar resistor 220Ω no LED

---

## 📡 Comunicação

### Arquitetura de Rede
```
[ESP32] ←─ Wi-Fi ─→ [Internet] ←─ TCP/IP ─→ [Broker MQTT] ←→ [Clientes]
```

### Pilha de Protocolos

| Camada | Protocolo | Descrição |
|--------|-----------|-----------|
| Aplicação | MQTT v3.1.1 | Pub/Sub de mensagens |
| Transporte | TCP | Conexão confiável |
| Rede | IPv4 | Endereçamento IP (DHCP) |
| Enlace | Wi-Fi 802.11n | Comunicação sem fio 2.4GHz |

### Configuração Wi-Fi
```cpp
SSID:     "Wokwi-GUEST"
Password: "" (aberto)
Channel:  6
Mode:     Station (STA)
IP:       DHCP automático
```

### Configuração MQTT
```cpp
Broker:   broker.hivemq.com
Porta:    1883 (TCP)
QoS:      0 (Fire and Forget)
ClientID: "MedPulso_" + random(HEX)
```

### Tópicos MQTT

| Tópico | Tipo | Direção | Descrição |
|--------|------|---------|-----------|
| `medpulso/comando` | SUB | Cliente → ESP32 | Recebe comandos de controle |

### Mensagens

**Comandos (Subscribe):**
```
Topic: medpulso/comando
Payloads aceitos:
  - "LIGAR" / "ON" / "START"
  - "DESLIGAR" / "OFF" / "STOP"

```

**Status (Publish):**
```
Topic: medpulso/status
Exemplos:
  - "Monitor ATIVADO"
  - "Monitor DESATIVADO"
  - "ESP32 Online"
```

### Interface Serial
```cpp
Protocolo: UART
Baudrate:  115200
Pinos:     TX0, RX0
Uso:       Debug e monitoramento
```

### Interface ADC
```cpp
Pino:       GPIO 35 (ADC1_CH5)
Resolução:  12 bits (0-4095)
Tensão:     0-3.3V
Conversão:  0-4095 → 40-180 BPM
```

### Interface PWM
```cpp
Pino:        GPIO 18
Frequências: 150Hz (LUB), 100Hz (DUB)
Duty Cycle:  50%
Função:      tone() / noTone()
```

---

## 🎯 Classificação de BPM

| Faixa | Classificação | Descrição |
|-------|---------------|-----------|
| < 60 | BRADICARDIA | Frequência baixa (atletas/problema) |
| 60-100 | NORMAL | Repouso saudável |
| 100-140 | ELEVADO | Exercício leve/moderado |
| > 140 | TAQUICARDIA | Exercício intenso/alteração |

```
