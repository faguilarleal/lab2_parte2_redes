import socket
from crc32 import emitir_crc32, añadir_ruido, verificar_crc32

class Client:
    def __init__(self, address):
        self.address = address
        self.PORT = 1111

    def recv(self):
        received_data = ""
        sock = None

        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            sock.bind(('', self.PORT))
            data, addr = sock.recvfrom(1024)
            received_data = data.decode()
            print(f"Received: {received_data} from {addr}")
        except Exception as e:
            print("Error:", e)
        finally:
            if sock:
                sock.close()

        return received_data

    def send(self, message, ruido):
        sock = None

        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            payload = emitir_crc32(message)
            if ruido == 1: 
                payload = añadir_ruido(payload, cantidad_bits=3)
            send_buffer = payload.encode()
            sock.sendto(send_buffer, (self.address, self.PORT))
            print("Mensaje enviado:", payload)
        except Exception as e:
            print("Error:", e)
        finally:
            if sock:
                sock.close()

        return 0

if __name__ == "__main__":
    client = Client("127.0.0.1")  
    msg = input("Ingrese mensaje para enviar: ")
    ruido = input("Ingrese cantidad de bits para agregar ruido, 0 si no: ")
    if ruido == "1":
        client.send(msg,1)
    else:
        client.send(msg,0)
