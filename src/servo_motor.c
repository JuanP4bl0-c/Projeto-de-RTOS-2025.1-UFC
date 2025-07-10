#include "servo_motor.h"

void servo_init(uint gpio) {
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(gpio);
    pwm_set_wrap(slice, 20000); // 20ms período (50Hz)
    pwm_set_clkdiv(slice, 125.0f); // Ajusta clock para 1us por tick (16MHz/125 = 128kHz)
    pwm_set_enabled(slice, true);
    printf("Servo motor initialized on GPIO %d\n", gpio);
}

void servo_set_angle(uint gpio, uint angle) {
    if (angle > 180) angle = 180;
    uint slice = pwm_gpio_to_slice_num(gpio);
    // Pulso entre 0.5ms (500us) e 2.5ms (2500us) para 0° a 180°
    uint min_duty = 500;
    uint max_duty = 2500;
    uint duty = min_duty + (angle * (max_duty - min_duty)) / 180;
    pwm_set_gpio_level(gpio, duty);
    printf("Servo motor on GPIO %d set to angle %d° (duty cycle: %d)\n", gpio, angle, duty);
}

void servo_task_test(void *pvParameters) {
    uint gpio = (uint)pvParameters;
    uint angle = 0;

    servo_init(gpio);

    while (true) {

        for (angle = 0; angle < 180; angle += 10) {
            servo_set_angle(gpio, angle);
            vTaskDelay(pdMS_TO_TICKS(100)); // Espera meio segundo
        }

        for (angle = 180; angle > 0; angle -= 10) {
            servo_set_angle(gpio, angle);
            vTaskDelay(pdMS_TO_TICKS(100)); // Espera meio segundo
        }
        
    }
}