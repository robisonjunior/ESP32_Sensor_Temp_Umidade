# 📡 Telemetria IoT Portátil: ESP32 (Deep Sleep) & BeagleBone Black

Este projeto consiste em um sistema de telemetria IoT completo para monitoramento remoto de temperatura e umidade. A arquitetura utiliza um nó sensor sem fio alimentado por bateria de íon-lítio (18650), um servidor web embarcado em uma **BeagleBone Black** e um túnel HTTPS seguro via **Cloudflare Tunnel**.

---

## 🏗️ Arquitetura do Sistema

```
[ ESP32 + Bateria 18650 ] 
         │  (HTTPS POST / SSL Insecure)
         ▼
[ Cloudflare Tunnel ] ──(Túnel Seguro)──► [ BeagleBone Black (Debian 12) ]
                                                   │
                                     ┌─────────────┴─────────────┐
                                     ▼                           ▼
                            [ Apache2 + PHP ]            [ MariaDB Database ]
                          (inserir_esp32.php)            (beagle_db.leituras_esp32)
```

---

## 🚀 Funcionalidades

- **Gerenciamento de Energia:** O ESP32 realiza a leitura dos dados, efetua a requisição HTTPS POST e entra em **Deep Sleep por 5 minutos (300s)**, reduzindo o consumo de corrente a microamperes ($\mu\text{A}$) e estendendo a autonomia da bateria 18650.
- **Comunicação Segura:** Envio de dados criptografados via HTTPS para a BeagleBone Black através do endpoint `https://beagle.corechip.com.br/inserir_esp32.php`.
- **Backend Robusto:** Script PHP responsável por validar payloads `x-www-form-urlencoded` ou `JSON` e persistir as leituras no banco de dados MariaDB.
- **Painel de Visualização:** Interface Web simples na BeagleBone Black para exibição em tempo real do histórico de telemetria.

---

## 🛠️ Hardware Utilizado

- **Nó Remoto:**
  - Placa de Desenvolvimento **ESP32-WROOM-32** com suporte integrado para 1 célula 18650 e circuito de carga TP4056.
  - Bateria Íon-Lítio 18650 (3.7V).
- **Servidor Web / Gateway:**
  - Single Board Computer **BeagleBone Black Rev C** rodando Debian 12 (Linux 6.12).

---

## 💻 Stack de Software & Dependências

### ESP32 (Firmware)
- **IDE:** Arduino IDE 2.3.x
- **Pacote de Placas:** `esp32` por Espressif Systems
- **Bibliotecas Nativas:** `<WiFi.h>`, `<HTTPClient.h>`, `<WiFiClientSecure.h>`

### BeagleBone Black (Backend)
- **Servidor Web:** Apache2
- **Linguagem:** PHP 8.x
- **Banco de Dados:** MariaDB (`beagle_db`)
- **Túnel DNS/SSL:** Cloudflare Tunnel (`cloudflared`)

---

## 📋 Estrutura da Tabela MariaDB

```sql
CREATE DATABASE IF NOT EXISTS beagle_db;
USE beagle_db;

CREATE TABLE IF NOT EXISTS leituras_esp32 (
    id INT AUTO_INCREMENT PRIMARY KEY,
    dispositivo VARCHAR(50) NOT NULL,
    temperatura FLOAT NOT NULL,
    umidade FLOAT NOT NULL,
    data_registro TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
```

---

## ⚙️ Configuração do ESP32 (Arduino IDE)

### 1. Parâmetros de Compilação / Upload
- **Board:** `ESP32 Dev Module`
- **Upload Speed:** `115200` *(Recomendado para estabilidade no chip USB-Serial)*
- **Flash Mode:** `DIO`

### 2. Trecho Principal do Firmware (`Deep Sleep`)
```cpp
// Tempo de hibernação (5 minutos)
#define TIME_TO_SLEEP  300               
#define uS_TO_S_FACTOR 1000000ULL        

void setup() {
  // Inicialização do Wi-Fi e leitura dos dados...
  
  // Envio HTTPS POST para o endpoint
  
  // Configura o timer para acordar em 5 minutos e entra em Deep Sleep
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}

void loop() {
  // Vazio: o ESP32 reinicia pelo setup() ao acordar do Deep Sleep
}
```

---

## 📌 Licença

Este projeto foi desenvolvido para fins didáticos e de prototipagem rápida em engenharia de sistemas embarcados e IoT.