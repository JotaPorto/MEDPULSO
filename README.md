# MedPulso - Monitor Cardíaco IoT

[![Wokwi](https://img.shields.io/badge/Simula%C3%A7%C3%A3o-Wokwi-green)](https://wokwi.com)
[![ESP32](https://img.shields.io/badge/Plataforma-ESP32-blue)](https://www.espressif.com/)
[![MQTT](https://img.shields.io/badge/Protocolo-MQTT-orange)](https://mqtt.org/)

**MedPulso** é um simulador de monitor cardíaco baseado em ESP32 que reproduz batimentos cardíacos de forma visual e sonora, com controle local e remoto via protocolo MQTT.


---

## 🎯 Sobre o Projeto

O **MedPulso** foi desenvolvido como um projeto educacional de IoT (Internet das Coisas) que simula um monitor de frequência cardíaca. O sistema permite:

- 📊 Simulação realista de batimentos cardíacos (som "LUB-DUB")
- 🎛️ Controle local através de potenciômetro, simulando o medidor de pulso (40-180 BPM)
- 🌐 Controle remoto via internet usando protocolo MQTT
- 💡 Feedback visual (LED) e sonoro (Buzzer)
- 📡 Telemetria em tempo real via MQTT

### 🎓 Objetivo Educacional

Demonstrar conceitos de:
- Programação de microcontroladores (ESP32)
- Comunicação Wi-Fi e protocolos TCP/IP
- Arquitetura pub/sub com MQTT
- Sensores analógicos e atuadores
- Internet das Coisas (IoT)

---

## ✨ Funcionalidades

### Controle Local
- ✅ Ajuste de BPM via potenciômetro (40-180 BPM)
- ✅ LED piscante sincronizado com batimentos
- ✅ Buzzer reproduzindo som cardíaco realista

### Controle Remoto (MQTT)
- ✅ Comandos: `LIGAR`, `DESLIGAR`
- ✅ Feedback de status em tempo real

### Classificação Médica
- **< 60 BPM**: Bradicardia (Baixo)
- **60-100 BPM**: Normal (Repouso)
- **100-140 BPM**: Elevado (Exercício)
- **> 140 BPM**: Taquicardia (Alto)

---

## 🎬 Demonstração

### Serial Monitor
```
╔════════════════════════════════════╗
║   MedPulso - Monitor Cardíaco      ║
║   Simulador de Batimento Cardíaco  ║
╚════════════════════════════════════╝

✓ WiFi Conectado!
  IP: 192.168.1.100

📡 Conectando ao MQTT...Conectado!
   Inscrito em: medpulso/comando

BPM: 72 | Status: NORMAL | Potenciômetro: 1800
BPM: 75 | Status: NORMAL | Potenciômetro: 1950
BPM: 78 | Status: NORMAL | Potenciômetro: 2100
```

### Comandos MQTT
```bash
# Ligar monitor
mosquitto_pub -h broker.hivemq.com -t "medpulso/comando" -m "LIGAR"

# Desligar monitor
mosquitto_pub -h broker.hivemq.com -t "medpulso/comando" -m "DESLIGAR"

```

---

## 🛠️ Hardware Necessário

### Componentes

| Componente | Quantidade | Especificação |
|------------|------------|---------------|
| ESP32 | 1 | Qualquer versão com Wi-Fi |
| LED Vermelho 5mm | 1 | Tensão: 2-3V |
| Buzzer Piezoelétrico | 1 | Passivo (controlável por PWM) |
| Potenciômetro ou Sensor de Pulso | 1 | 10kΩ linear (B10K) |

> *Não necessário na simulação Wokwi

### Requisitos de Software

- **Arduino IDE** 1.8.x ou superior (ou PlatformIO)
- **Biblioteca**: PubSubClient v2.8.0
- **Placa**: ESP32 Board Package

---

## 🔌 Diagrama de Conexões

```
                    ╔═══════════════════════════════╗
                    ║      ESP32 DevKit V1          ║
                    ║                               ║
    LED (Vermelho)  ║  GPIO 5       ●───────────●  ║  Anodo (+)
                    ║                               ║
                    ║  GND.1        ●───────────●  ║  Catodo (-)
                    ║                               ║
    BUZZER          ║  GPIO 18      ●───────────●  ║  Pino 1 (+)
                    ║                               ║
                    ║  GND.2        ●───────────●  ║  Pino 2 (-)
                    ║                               ║
    POTENCIÔMETRO   ║  3V3          ●───────────●  ║  VCC
                    ║                               ║
                    ║  GPIO 33      ●───────────●  ║  SIG (Wiper)
                    ║                               ║
                    ║  GND.1        ●───────────●  ║  GND
                    ║                               ║
                    ╚═══════════════════════════════╝
```

### Tabela de Conexões

| ESP32 | → | Componente |
|-------|---|------------|
| GPIO 5 | → | LED Anodo (+) |
| GND.1 | → | LED Catodo (-) |
| GPIO 18 | → | Buzzer Positivo |
| GND.2 | → | Buzzer Negativo |
| 3V3 | → | Potenciômetro VCC |
| GPIO 33 | → | Potenciômetro SIG |
| GND.1 | → | Potenciômetro GND |

---

## 🚀 Como Usar

### Modo Local (Potenciômetro)

1. Ligue o ESP32
2. Aguarde conexão Wi-Fi e MQTT (2 beeps de confirmação)
3. Gire o potenciômetro para ajustar BPM
4. Observe LED piscando e buzzer tocando

### Modo Remoto (MQTT)

#### Via HiveMQ Web Client

1. Acesse: [HiveMQ Web Client](http://www.hivemq.com/demos/websocket-client/)
2. Conecte ao broker: `broker.hivemq.com` (porta 8000 WebSocket)
3. **Publish** comandos no tópico: `medpulso/comando`

**Comandos disponíveis:**
- `LIGAR` ou `ON` ou `START` → Ativa o monitor
- `DESLIGAR` ou `OFF` ou `STOP` → Desativa o monitor

```

