#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

// Credenciais da rede Wi-Fi
const char* ssid = "RJC_NEW_OFFICE";
const char* password = "AGy72AWSa9#";

// Endpoint configurado no servidor BeagleBone
const char* serverUrl = "https://beagle.corechip.com.br/inserir_esp32.php";

// Nome para identificar este ESP32 no banco de dados MariaDB
const char* nomeDispositivo = "ESP32_18650_Sleep";

// Configuração do tempo de Deep Sleep (5 minutos)
// 5 minutos * 60 segundos * 1.000.000 microsegundos
#define TIME_TO_SLEEP  300               
#define uS_TO_S_FACTOR 1000000ULL        

void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println("\n----------------------------------");
  Serial.println("ESP32 acordou! Inicializando leitura...");
  
  // Conexão Wi-Fi com timeout para não esgotar a bateria se o Wi-Fi cair
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

    // Gerar valores aleatórios simulados (Temp: 20-35°C | Umidade: 40-80%)
    float temperatura = random(200, 351) / 10.0;
    float umidade = random(400, 801) / 10.0;

    Serial.printf("Leitura -> Temp: %.1f °C | Umidade: %.1f %%\n", temperatura, umidade);

    WiFiClientSecure *client = new WiFiClientSecure;
    if (client) {
      client->setInsecure(); // Conexão HTTPS via Cloudflare Tunnel

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
    Serial.println("\nFalha ao conectar ao Wi-Fi. Entrando em modo de espera...");
  }

  // Configurar o timer para acordar após 5 minutos
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  
  Serial.println("Entrando em Deep Sleep por 5 minutos...");
  Serial.flush(); 
  
  // Desliga o ESP32 e entra em hibernação profunda
  esp_deep_sleep_start();
}

void loop() {
  // O loop fica vazio pois o ESP32 nunca chega aqui no modo Deep Sleep
}