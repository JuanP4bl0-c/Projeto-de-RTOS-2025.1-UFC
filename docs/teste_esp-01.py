from machine import Pin, Timer, UART
import machine
import _thread
import time

uart = UART(1,baudrate=115200, tx=Pin(4), rx=Pin(5)) # add your own values

print('-- UART Serial --')
print('>', end='')

def uartSerialLogger():
    while True:
        command = uart.read(1)
        
        if command:
            recv=str(command.decode("utf-8"))
            print(recv, end='')

_thread.start_new_thread(uartSerialLogger, ())

while True:
    send = input()
    uart.write(send+'\r\n')