add_executable(app
        main.c
        led.c
        #cpu_monitor.c
        #adc_custom.c
        Timer.c
        bttn.c
        Bluethooth.c
        i2c.c
        oled.c
        fonts.c
        dht11.c
        wifi.c
        servo_motor.c
)

target_include_directories(app PUBLIC inc)

target_link_libraries(
    app
    pico_stdlib 
    lib 
    pico_stdio_usb
    hardware_timer
    hardware_adc
    hardware_i2c
    hardware_pwm
)
target_link_options(app PRIVATE -u _printf_float)

pico_add_extra_outputs(app)
