#include "oled.h"

// Envia comando para o OLED
void oled_send_command(uint8_t cmd) {
    uint8_t data[2] = {0x00, cmd};
    i2c_write(OLED_ADDR, data, 2);
}

// Envia dados para o OLED
void oled_send_data(uint8_t *data, size_t len) {
    uint8_t buf[OLED_WIDTH + 1];
    buf[0] = 0x40; // 0x40 indica dados
    for (size_t i = 0; i < len; i++) buf[i+1] = data[i];
    i2c_write(OLED_ADDR, buf, len + 1);
}

// Inicializa o OLED
void oled_init() {

    i2c_init_custom(); // Inicializa o I2C

    oled_send_command(0xAE); // Display OFF
    oled_send_command(0xD5); // Set display clock div
    oled_send_command(0x80);
    oled_send_command(0xA8); // Set multiplex
    oled_send_command(0x3F);
    oled_send_command(0xD3); // Set display offset
    oled_send_command(0x00);
    oled_send_command(0x40); // Set start line
    oled_send_command(0x8D); // Charge pump
    oled_send_command(0x14);
    oled_send_command(0x20); // Memory mode
    oled_send_command(0x00);
    oled_send_command(0xA1); // Seg remap
    oled_send_command(0xC8); // COM scan dec
    oled_send_command(0xDA); // Set compins
    oled_send_command(0x12);
    oled_send_command(0x81); // Set contrast
    oled_send_command(0xCF);
    oled_send_command(0xD9); // Set precharge
    oled_send_command(0xF1);
    oled_send_command(0xDB); // Set vcom detect
    oled_send_command(0x40);
    oled_send_command(0xA4); // Resume RAM content
    oled_send_command(0xA6); // Normal display
    oled_send_command(0xAF); // Display ON
}

void oled_fill_screen() {
    for (uint8_t page = 0; page < 8; page++) {
        oled_send_command(0xB0 + page); // Set page address
        oled_send_command(0x00);        // Set lower column
        oled_send_command(0x10);        // Set higher column
        uint8_t line[OLED_WIDTH];
        for (int i = 0; i < OLED_WIDTH; i++) line[i] = 0xFF; // Todos pixels ON
        oled_send_data(line, OLED_WIDTH);
    }
}

// Escreve uma string na posição (x, page) usando font5x7
void oled_write_string(uint8_t x, uint8_t page, const char *str) {
    oled_send_command(0xB0 + page); // Seleciona a página (linha de 8px)
    oled_send_command(0x00 + (x & 0x0F)); // Coluna baixa
    oled_send_command(0x10 + ((x >> 4) & 0x0F)); // Coluna alta

    while (*str) {
        uint8_t c = *str;
        if (c < 32 || c > 126) c = 32; // Caracteres fora do range: espaço
        oled_send_data((uint8_t*)font5x7[c - 32], 5); // 5 bytes do caractere
        uint8_t space = 0x00;
        oled_send_data(&space, 1); // Espaço entre caracteres
        str++;
    }
}