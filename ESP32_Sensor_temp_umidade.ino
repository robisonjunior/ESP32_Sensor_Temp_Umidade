#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Wire.h>
#include <Adafruit_SHT31.h>

// Credenciais da rede Wi-Fi
const char* ssid = "RJC_NEW_OFFICE";
const char* password = "AGy72AWSa9#";

// Endpoint no servidor BeagleBone
const char* serverUrl = "https://beagle.corechip.com.br/inserir_esp32.php";

// Nome do dispositivo no banco de dados MariaDB
const char* nomeDispositivo = "ESP32_SHT31_18650";

// Instância do sensor SHT31
Adafruit_SHT31 sht31 = Adafruit_SHT31();

// Configuração do Deep Sleep para 20 MINUTOS
// 20 minutos * 60 segundos * 1.000.000 microsegundos
#define TIME_TO_SLEEP  1200              
#define uS_TO_S_FACTOR 1000000ULL        

void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println("\n----------------------------------");
  Serial.println("ESP32 acordou! Inicializando sensor SHT31...");
  
  // Inicialização I2C (SDA = GPIO 21, SCL = GPIO 22)
  Wire.begin(21, 22);

  float temperatura = 0.0;
  float umidade = 0.0;
  bool leituraValida = false;

  // Inicializa o SHT31 no endereço padrão 0x44 (pino AD desconectado)
  if (!sht31.begin(0x44)) {
    Serial.println("ERRO: Sensor SHT31 nao encontrado! Verifique Vin(3.3V), GND, SDA(21) e SCL(22).");
  } else {
    // Garante que o aquecedor interno (heater) esteja desligado para não distorcer a leitura
    sht31.heater(false);
    delay(50);

    temperatura = sht31.readTemperature();
    umidade = sht31.readHumidity();

    if (!isnan(temperatura) && !isnan(umidade)) {
      leituraValida = true;
      Serial.printf("Leitura REAL SHT31 -> Temp: %.1f °C | Umidade: %.1f %%\n", temperatura, umidade);
    } else {
      Serial.println("ERRO: Falha ao ler dados do SHT31 (NaN).");
    }
  }

  // Só conecta no Wi-Fi e envia o POST se a leitura do sensor foi bem-sucedida
  if (leituraValida) {
    WiFi.begin(ssid, password);
    Serial.print("Conectando ao Wi-Fi");
    
    int tentativas = 0;
    while (WiFi.status() != WL_CONNECTED && tentativas < 20) {
      delay(500);
      Serial.print(".");
      tentativas++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nWi-Fi Conectado!");

      WiFiClientSecure *client = new WiFiClientSecure;
      if (client) {
        client->setInsecure(); // SSL via Cloudflare Tunnel

        HTTPClient http;
        if (http.begin(*client, serverUrl)) {
          http.addHeader("Content-Type", "application/x-www-form-urlencoded");

          String httpRequestData = "dispositivo=" + String(nomeDispositivo) +
                                   "&temperatura=" + String(temperatura, 1) + 
                                   "&umidade=" + String(umidade, 1);

          Serial.print("Enviando POST para: ");
          Serial.println(serverUrl);
          
          int httpResponseCode = http.POST(httpRequestData);

          if (httpResponseCode > 0) {
            Serial.printf("Resposta do Servidor (HTTP %d):\n", httpResponseCode);
            String response = http.getString();
            Serial.println(response);
          } else {
            Serial.printf("Erro no POST: %s (Código: %d)\n", http.errorToString(httpResponseCode).c_str(), httpResponseCode);
          }

          http.end();
        } else {
          Serial.println("Falha ao iniciar conexão HTTP Client.");
        }
        delete client;
      }
    } else {
      Serial.println("\nFalha na conexao Wi-Fi.");
    }
  }

  // Programar o RTC para acordar em 20 minutos
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  
  Serial.println("Entrando em Deep Sleep por 20 minutos...");
  Serial.flush(); 
  esp_deep_sleep_start();
}

void loop() {
  // Vazio no modo Deep Sleep
}