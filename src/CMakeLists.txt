add_executable(app
        main.c
        led.c
        cpu_monitor.c
        adc_custom.c
        bttn.c
        Bluethooth.c
)

target_include_directories(app PUBLIC inc)

target_link_libraries(
    app
    pico_stdlib 
    lib 
    pico_stdio_usb
    hardware_timer
    hardware_adc
)

pico_add_extra_outputs(app)
