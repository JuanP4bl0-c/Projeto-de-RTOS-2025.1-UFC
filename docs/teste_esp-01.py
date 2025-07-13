from machine import UART, Pin
import time

esp = UART(0, baudrate=115200, tx=Pin(0), rx=Pin(1))
esp.init(115200, bits=8, parity=None, stop=1)

def send_cmd(cmd, delay=1):
    esp.write((cmd + '\r\n').encode())
    time.sleep(delay)
    response = b""
    start = time.ticks_ms()
    while time.ticks_diff(time.ticks_ms(), start) < 2000:
        if esp.any():
            try:
                response += esp.read(64)
            except MemoryError:
                print("Buffer muito grande, esvaziando...")
                response = b""
            time.sleep(0.1)
    print(response.decode(errors='ignore'))
    return response


# Configuração inicial
print("Configurando ESP-01...")
send_cmd('AT')
send_cmd('AT+CWMODE=1')

# Troque pelas suas credenciais Wi-Fi
ssid = "SUA_REDE"
senha = "SUA_SENHA"
send_cmd(f'AT+CWJAP="{ssid}","{senha}"', 5)

send_cmd('AT+CIFSR')
send_cmd('AT+CIPMUX=1')
send_cmd('AT+CIPSERVER=1,80')

print("Servidor iniciado. Acesse o IP mostrado acima.")

while True:
    if esp.any():
        try:
            data = esp.readline()
            if data:
                data = data.decode(errors='ignore')
                print("Recebido:", data)

                if "+IPD" in data:
                    conn_id = data.split(',')[0][-1]  # extrai o ID da conexão

                    html = """\
HTTP/1.1 200 OK
Content-Type: text/html

<!DOCTYPE html>
<html>
<head><title>Hello</title></head>
<body><h1>Hello World</h1></body>
</html>
"""
                    send_cmd(f'AT+CIPSEND={conn_id},{len(html)}')
                    time.sleep(1)
                    esp.write(html.encode())
                    time.sleep(1)
                    send_cmd(f'AT+CIPCLOSE={conn_id}')
        except Exception as e:
            print("Erro ao processar dados:", e)
            esp.read()  # limpa buffer
