#include "wifi.h"

void esp8266_uart_init(void) {
    uart_init(ESP8266_UART_ID, ESP8266_BAUDRATE);
    gpio_set_function(ESP8266_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(ESP8266_RX_PIN, GPIO_FUNC_UART);
}

void esp8266_send_cmd(const char *cmd) {
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
    sleep_ms(1000);
    esp8266_read_response(resp, sizeof(resp));

    esp8266_send_cmd("AT+CWMODE=1"); // modo station
    sleep_ms(1000);
    esp8266_read_response(resp, sizeof(resp));

    char cmd[128];
    sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"", ssid, password);
    esp8266_send_cmd(cmd);
    sleep_ms(5000);
    esp8266_read_response(resp, sizeof(resp));
}