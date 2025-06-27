add_executable(app
        main.c
        led.c
        bttn.c
)

target_link_libraries(
    app
    pico_stdlib 
    lib 
    pico_stdio_usb
    #hardware_timer
)

pico_add_extra_outputs(app)
