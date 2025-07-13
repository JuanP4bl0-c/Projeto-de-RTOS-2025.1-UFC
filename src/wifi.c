#include "wifi.h"

void esp8266_uart_init(void) {
    uart_init(ESP8266_UART_ID, ESP8266_BAUDRATE);
    gpio_set_function(ESP8266_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(ESP8266_RX_PIN, GPIO_FUNC_UART);
    printf("ESP8266 UART initialized at %d baud\n", ESP8266_BAUDRATE);
}

void esp8266_send_cmd(const char *cmd) {
    printf("Sending command: %s\n", cmd);
    uart_puts(ESP8266_UART_ID, cmd);
    uart_puts(ESP8266_UART_ID, "\r\n");
}

void esp8266_read_response(char *buffer, size_t maxlen) {
    size_t idx = 0;
    while (idx < maxlen - 1) {
        if (uart_is_readable(ESP8266_UART_ID)) {
            char c = uart_getc(ESP8266_UART_ID);
            buffer[idx++] = c;
            if (c == '\n') break;
        }
    }
    buffer[idx] = '\0';
}   

void esp8266_connect_wifi(const char *ssid, const char *password) {
    char resp[128];

    esp8266_send_cmd("AT");
    vTaskDelay(1000);
    esp8266_read_response(resp, sizeof(resp));
    printf("Resposta: %s\n", resp);
    
    esp8266_send_cmd("AT+CWMODE=1"); // modo station
    vTaskDelay(1000);
    esp8266_read_response(resp, sizeof(resp));
    printf("Resposta: %s\n", resp);
    
    char cmd[128];
    sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"", ssid, password);
    esp8266_send_cmd(cmd);
    vTaskDelay(5000);
    esp8266_read_response(resp, sizeof(resp));
    printf("Resposta: %s\n", resp);
}

void esp8266_task(void *pvParameters) {
    vTaskDelay(pdMS_TO_TICKS(4000)); // Espera 2 segundos
    (void) pvParameters;
    esp8266_uart_init(); // Chama só uma vez!
    printf("Iniciando tarefa ESP8266...\n");
    esp8266_connect_wifi("Teste", "bolodechocolate"); // Substitua por suas credenciais Wi-Fi
    vTaskDelay(pdMS_TO_TICKS(2000)); // Espera 2 segundos
    printf("Conectado ao Wi-Fi\n");
    
    char response[128];
    while (true) {
        esp8266_read_response(response, sizeof(response));
        printf("ESP8266 Response: %s\n", response);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

#include "wifi.h"

// Task para criar AP e servir página HTML "Hello world"
void esp8266_ap_webserver_task(void *pvParameters) {
    char resp[256];
    
    vTaskDelay(pdMS_TO_TICKS(3000)); // Espera 2 segundos
    esp8266_uart_init();
    vTaskDelay(pdMS_TO_TICKS(2000));

    // 1. Reinicia o ESP8266
    esp8266_send_cmd("AT+RST");
    vTaskDelay(pdMS_TO_TICKS(3000));

    // 2. Configura modo AP
    esp8266_send_cmd("AT+CWMODE=2");
    vTaskDelay(pdMS_TO_TICKS(1000));
    esp8266_read_response(resp, sizeof(resp));
    printf("Resposta: %s", resp);

    // 3. Configura o SSID e senha do AP
    esp8266_send_cmd("AT+CWSAP=\"Helder_meuRei\",\"titiohelder\",5,3");
    vTaskDelay(pdMS_TO_TICKS(2000));
    esp8266_read_response(resp, sizeof(resp));
    printf("Resposta: %s", resp);

    // 4. Inicia servidor TCP na porta 80
    esp8266_send_cmd("AT+CIPMUX=1");
    vTaskDelay(pdMS_TO_TICKS(1000));
    esp8266_read_response(resp, sizeof(resp));
    printf("Resposta: %s", resp);

    esp8266_send_cmd("AT+CIPSERVER=1,80");
    vTaskDelay(pdMS_TO_TICKS(1000));
    esp8266_read_response(resp, sizeof(resp));
    printf("Resposta: %s", resp);
    
    printf("AP iniciado! SSID: MeuAP, senha: 12345678\n");

    // 5. Loop para responder conexões
    while (1) {
        esp8266_read_response(resp, sizeof(resp));
        // Procura por "+IPD" indicando nova conexão
        char *ipd = strstr(resp, "+IPD,");
        if (ipd) {
            // Extrai o canal de conexão
            int ch = 0;
            sscanf(ipd, "+IPD,%d,", &ch);

            // Monta resposta HTTP
            const char *http_response =
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/html\r\n"
                "Connection: close\r\n"
                "\r\n"
                "<html><body><h1>Hello world</h1><h2>Finalmete Funciona!!!!</h2></body></html>\r\n";

            char cmd[64];
            sprintf(cmd, "AT+CIPSEND=%d,%d", ch, (int)strlen(http_response));
            esp8266_send_cmd(cmd);
            vTaskDelay(pdMS_TO_TICKS(500));
            esp8266_read_response(resp, sizeof(resp));
            printf("Resposta: %s\n", resp);

            uart_puts(ESP8266_UART_ID, http_response);
            vTaskDelay(pdMS_TO_TICKS(500));

            sprintf(cmd, "AT+CIPCLOSE=%d", ch);
            esp8266_send_cmd(cmd);
            vTaskDelay(pdMS_TO_TICKS(200));
            esp8266_read_response(resp, sizeof(resp));
            printf("Resposta: %s\n", resp);
        }
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}