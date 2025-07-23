#include "servo_motor.h"

uint16_t angle_atual = 0; // Variável global para armazenar o ângulo atual do servo motor

void servo_init(uint gpio) {
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(gpio);
    pwm_set_wrap(slice, 20000); // 20ms período (50Hz)
    pwm_set_clkdiv(slice, 125.0f); // Ajusta clock para 1us por tick (16MHz/125 = 128kHz)
    pwm_set_enabled(slice, true);
    printf("Servo motor initialized on GPIO %d\n", gpio);
    angle_atual = 0; // Inicializa o ângulo atual
}

void servo_move_to_angle(uint gpio, uint y, uint delay_ms) {
    
    int x = angle_atual;

    if (x == y) return;
    int step = (y > x) ? 1 : -1;
    int angle;
    for (angle = x; angle != (int)y; angle += step) {
        servo_set_angle(gpio, angle);
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
    // Garante que chega exatamente em y
    servo_set_angle(gpio, y);
}

void servo_set_angle(uint gpio, uint angle) {
    if (angle > 180+45) angle = 180;
    uint slice = pwm_gpio_to_slice_num(gpio);
    // Pulso entre 0.5ms (500us) e 2.5ms (2500us) para 0° a 180°
    uint min_duty = 500;
    uint max_duty = 2500;
    uint duty = min_duty + (angle * (max_duty - min_duty)) / 180;
    pwm_set_gpio_level(gpio, duty);
    angle_atual = angle; // Atualiza o ângulo atual
    //printf("Servo motor on GPIO %d set to angle %d° (duty cycle: %d)\n", gpio, angle, duty);
}

void reset_servo(uint gpio) {
    servo_set_angle(gpio, 0); // Reseta o servo motor para 0°
    angle_atual = 0; // Reseta o ângulo atual
    printf("Servo motor on GPIO %d reset to 0°\n", gpio);
}

void close_servo(uint gpio) {

    for (;angle_atual > 0; angle_atual -= 1) {
        servo_set_angle(SERVO_PIN, angle_atual);
        vTaskDelay(pdMS_TO_TICKS(50)); // Espera meio segundo
    }
    angle_atual = 0; // Reseta o ângulo atual
}

void open_servo(uint gpio) {
    uint angle;

    for (angle = angle_atual;angle < 90; angle += 1) {
        servo_set_angle(SERVO_PIN, angle);
        vTaskDelay(pdMS_TO_TICKS(50)); // Espera meio segundo
    }
}

void servo_task_test(void *pvParameters) {
    uint angle = 0;

    servo_init(SERVO_PIN);

    while (true) {

        for (angle = 0; angle < 90; angle += 1) {
            servo_set_angle(SERVO_PIN, angle);
            vTaskDelay(pdMS_TO_TICKS(10)); // Espera meio segundo
        }

        
        for (angle = 90; angle > 0; angle -= 1) {
            servo_set_angle(SERVO_PIN, angle);
            vTaskDelay(pdMS_TO_TICKS(10)); // Espera meio segundo
        }
        vTaskDelay(pdMS_TO_TICKS(1000)); // Espera 1 segundo
        
    }
}