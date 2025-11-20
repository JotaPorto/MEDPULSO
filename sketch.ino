#include <WiFi.h>
#include <PubSubClient.h>

// Pinos
const int LED = 5;
const int BUZZER = 18;
const int POT = 33;

// Configurações Wi-Fi
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""
#define WIFI_CHANNEL 6

// Configurações MQTT
const char* mqtt_server = "broker.hivemq.com";
const char* topic_comando = "medpulso/comando";
const char* topic_status = "medpulso/status";
const char* topic_bpm = "medpulso/bpm";

WiFiClient espClient;
PubSubClient client(espClient);

// Variáveis do monitor cardíaco
int bpm = 0;                    // Batimentos por minuto
unsigned long ultimoBatimento = 0;
bool monitorAtivo = true;       // Liga/desliga o monitor
int intervaloBatimento = 1000;  // Intervalo entre batimentos em ms

// Simula o padrão do batimento cardíaco (som "LUB-DUB")
void simularBatimento() {
  // Primeira parte do batimento (LUB)
  digitalWrite(LED, HIGH);
  tone(BUZZER, 150);  // Tom grave
  delay(100);
  noTone(BUZZER);
  digitalWrite(LED, LOW);
  delay(80);
  
  // Segunda parte do batimento (DUB)
  digitalWrite(LED, HIGH);
  tone(BUZZER, 100);  // Tom mais grave
  delay(120);
  noTone(BUZZER);
  digitalWrite(LED, LOW);
}

// Calcula BPM baseado no potenciômetro
void calcularBPM() {
  int potValue = analogRead(POT);
  
  // Mapeia o potenciômetro para faixa de BPM realista
  // 0-4095 -> 40-180 BPM (faixa normal a exercício intenso)
  bpm = map(potValue, 0, 4095, 40, 180);
  
  // Calcula intervalo entre batimentos em milissegundos
  // BPM = batimentos por minuto, então intervalo = 60000ms / BPM
  intervaloBatimento = 60000 / bpm;
  
  // Classifica o BPM
  String classificacao = "";
  if (bpm < 60) {
    classificacao = "BRADICARDIA (Baixo)";
  } else if (bpm >= 60 && bpm <= 100) {
    classificacao = "NORMAL";
  } else if (bpm > 100 && bpm <= 140) {
    classificacao = "ELEVADO";
  } else {
    classificacao = "TAQUICARDIA (Alto)";
  }
  
  // Exibe informações no Serial Monitor
  Serial.print("BPM: ");
  Serial.print(bpm);
  Serial.print(" | Status: ");
  Serial.print(classificacao);
  Serial.print(" | Potenciômetro: ");
  Serial.println(potValue);
  
  // Publica BPM via MQTT a cada 5 segundos
  static unsigned long ultimaPublicacao = 0;
  if (millis() - ultimaPublicacao > 5000) {
    String msg = String(bpm) + " BPM - " + classificacao;
    client.publish(topic_bpm, msg.c_str());
    ultimaPublicacao = millis();
  }
}

// Função de callback MQTT
void callback(char* topic, byte* payload, unsigned int length) {
  String comando = "";
  
  for (int i = 0; i < length; i++) {
    comando += (char)payload[i];
  }
  
  Serial.print("Mensagem MQTT [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(comando);
  
  if (String(topic) == topic_comando) {
    if (comando == "LIGAR" || comando == "ON" || comando == "START") {
      monitorAtivo = true;
      Serial.println("Monitor Cardíaco ATIVADO");
      client.publish(topic_status, "Monitor ATIVADO");
    }
    else if (comando == "DESLIGAR" || comando == "OFF" || comando == "STOP") {
      monitorAtivo = false;
      digitalWrite(LED, LOW);
      noTone(BUZZER);
      Serial.println("Monitor Cardíaco DESATIVADO");
      client.publish(topic_status, "Monitor DESATIVADO");
    }
    else if (comando == "STATUS") {
      String statusMsg = monitorAtivo ? "ATIVO - " : "INATIVO - ";
      statusMsg += String(bpm) + " BPM";
      client.publish(topic_status, statusMsg.c_str());
      Serial.println("Status enviado via MQTT");
    }
  }
  Serial.println();
}

// Conectar ao WiFi
void conectarWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);
  Serial.print("Conectando ao WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  
  Serial.println("WiFi Conectado!");
  Serial.print("   IP: ");
  Serial.println(WiFi.localIP());
}

// Reconectar ao MQTT
void reconectarMQTT() {
  while (!client.connected()) {
    Serial.print("Conectando ao MQTT...");
    
    String clientId = "MedPulso_";
    clientId += String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str())) {
      Serial.println("Conectado!");
      
      client.subscribe(topic_comando);
      Serial.print("   Inscrito em: ");
      Serial.println(topic_comando);
      
      client.publish(topic_status, "MedPulso Online");
      
      // Sinal de conexão MQTT (3 beeps rápidos)
      for (int i = 0; i < 3; i++) {
        tone(BUZZER, 2000);
        digitalWrite(LED, HIGH);
        delay(100);
        noTone(BUZZER);
        digitalWrite(LED, LOW);
        delay(100);
      }
      
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" | Tentando em 5s");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  pinMode(LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  
  Serial.println("\n╔════════════════════════════════════╗");
  Serial.println("║   MedPulso - Monitor Cardíaco      ║");
  Serial.println("║   Simulador de Batimento Cardíaco  ║");
  Serial.println("╚════════════════════════════════════╝");
  
  conectarWiFi();
  
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
  // Sinal de inicialização (batimento cardíaco)
  simularBatimento();
  delay(500);
  simularBatimento();
  
  Serial.println("\n╔════════════════════════════════════╗");
  Serial.println("║ Comandos MQTT:                     ║");
  Serial.println("║ Tópico: medpulso/comando           ║");
  Serial.println("║  • LIGAR / ON / START              ║");
  Serial.println("║  • DESLIGAR / OFF / STOP           ║");
  Serial.println("╚════════════════════════════════════╝\n");
  
  Serial.println("Sistema Pronto! Gire o potenciômetro para ajustar o BPM\n");
}

void loop() {
  // Mantém conexões
  if (!client.connected()) {
    reconectarMQTT();
  }
  client.loop();
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi desconectado! Reconectando...");
    conectarWiFi();
  }
  
  // Se o monitor está ativo, calcula BPM e simula batimentos
  if (monitorAtivo) {
    calcularBPM();
    
    unsigned long tempoAtual = millis();
    
    // Verifica se é hora de um novo batimento
    if (tempoAtual - ultimoBatimento >= intervaloBatimento) {
      simularBatimento();
      ultimoBatimento = tempoAtual;
    }
    
    delay(100);  // Pequeno delay para estabilidade
  } else {
    // Monitor desativado - apenas mantém conexões sem registrar
    delay(500);
  }
}