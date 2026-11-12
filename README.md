# ðŸ“¡ Telemetria IoT PortÃ¡til: ESP32 (Deep Sleep) \& BeagleBone Black

Este projeto consiste em um sistema de telemetria IoT completo para monitoramento remoto de temperatura e umidade. A arquitetura utiliza um nÃ³ sensor sem fio alimentado por bateria de Ã­on-lÃ­tio (18650), um servidor web embarcado em uma **BeagleBone Black** e um tÃºnel HTTPS seguro via **Cloudflare Tunnel**.



\## Esquema de LigaÃ§Ã£o (ESP32 + SHT31)



!\[Diagrama ESP32 e Sensor SHT31](./image\_dbade220.jpg)



\---

## ðŸ—ï¸ Arquitetura do Sistema

```
\[ ESP32 + Bateria 18650 ] 
         â”‚  (HTTPS POST / SSL Insecure)
         â–¼
\[ Cloudflare Tunnel ] â”€â”€(TÃºnel Seguro)â”€â”€â–º \[ BeagleBone Black (Debian 12) ]
                                                   â”‚
                                     â”Œâ”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”´â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”
                                     â–¼                           â–¼
                            \[ Apache2 + PHP ]            \[ MariaDB Database ]
                          (inserir\_esp32.php)            (beagle\_db.leituras\_esp32)
```

\---

## ðŸš€ Funcionalidades

* **Gerenciamento de Energia:** O ESP32 realiza a leitura dos dados, efetua a requisiÃ§Ã£o HTTPS POST e entra em **Deep Sleep por 5 minutos (300s)**, reduzindo o consumo de corrente a microamperes ($\\mu\\text{A}$) e estendendo a autonomia da bateria 18650.
* **ComunicaÃ§Ã£o Segura:** Envio de dados criptografados via HTTPS para a BeagleBone Black atravÃ©s do endpoint `https://beagle.corechip.com.br/inserir\_esp32.php`.
* **Backend Robusto:** Script PHP responsÃ¡vel por validar payloads `x-www-form-urlencoded` ou `JSON` e persistir as leituras no banco de dados MariaDB.
* **Painel de VisualizaÃ§Ã£o:** Interface Web simples na BeagleBone Black para exibiÃ§Ã£o em tempo real do histÃ³rico de telemetria.

\---

## ðŸ› ï¸ Hardware Utilizado

* **NÃ³ Remoto:**

  * Placa de Desenvolvimento **ESP32-WROOM-32** com suporte integrado para 1 cÃ©lula 18650 e circuito de carga TP4056.
  * Bateria Ãon-LÃ­tio 18650 (3.7V).
* **Servidor Web / Gateway:**

  * Single Board Computer **BeagleBone Black Rev C** rodando Debian 12 (Linux 6.12).

\---

## ðŸ’» Stack de Software \& DependÃªncias

### ESP32 (Firmware)

* **IDE:** Arduino IDE 2.3.x
* **Pacote de Placas:** `esp32` por Espressif Systems
* **Bibliotecas Nativas:** `<WiFi.h>`, `<HTTPClient.h>`, `<WiFiClientSecure.h>`

### BeagleBone Black (Backend)

* **Servidor Web:** Apache2
* **Linguagem:** PHP 8.x
* **Banco de Dados:** MariaDB (`beagle\_db`)
* **TÃºnel DNS/SSL:** Cloudflare Tunnel (`cloudflared`)

\---

## ðŸ“‹ Estrutura da Tabela MariaDB

```sql
CREATE DATABASE IF NOT EXISTS beagle\_db;
USE beagle\_db;

CREATE TABLE IF NOT EXISTS leituras\_esp32 (
    id INT AUTO\_INCREMENT PRIMARY KEY,
    dispositivo VARCHAR(50) NOT NULL,
    temperatura FLOAT NOT NULL,
    umidade FLOAT NOT NULL,
    data\_registro TIMESTAMP DEFAULT CURRENT\_TIMESTAMP
);
```

\---

## âš™ï¸ ConfiguraÃ§Ã£o do ESP32 (Arduino IDE)

### 1\. ParÃ¢metros de CompilaÃ§Ã£o / Upload

* **Board:** `ESP32 Dev Module`
* **Upload Speed:** `115200` *(Recomendado para estabilidade no chip USB-Serial)*
* **Flash Mode:** `DIO`

### 2\. Trecho Principal do Firmware (`Deep Sleep`)

```cpp
// Tempo de hibernaÃ§Ã£o (5 minutos)
#define TIME\_TO\_SLEEP  300               
#define uS\_TO\_S\_FACTOR 1000000ULL        

void setup() {
  // InicializaÃ§Ã£o do Wi-Fi e leitura dos dados...
  
  // Envio HTTPS POST para o endpoint
  
  // Configura o timer para acordar em 5 minutos e entra em Deep Sleep
  esp\_sleep\_enable\_timer\_wakeup(TIME\_TO\_SLEEP \* uS\_TO\_S\_FACTOR);
  esp\_deep\_sleep\_start();
}

void loop() {
  // Vazio: o ESP32 reinicia pelo setup() ao acordar do Deep Sleep
}
```

\---

## ðŸ“Œ LicenÃ§a

Este projeto foi desenvolvido para fins didÃ¡ticos e de prototipagem rÃ¡pida em engenharia de sistemas embarcados e IoT.

