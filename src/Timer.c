#include "Timer.h"

Sistema_t sistema;

//callback function for the timer
void timer_callback(TimerHandle_t xTimer) {
    xSemaphoreGive(sistema.semaphore);
}

void sleep_timer_init() {
    // Create a timer with a period of 1 second
    sleep_ms(MIN_timer);
}