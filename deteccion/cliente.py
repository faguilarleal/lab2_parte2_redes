import socket
from emisor import Sender

class Client:
    def __init__(self, address):
        self.address = address
        self.PORT = 1111
        self.sender = Sender()

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

    def send(self, message):
        sock = None

        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            payload = self.sender.build_payload(message)
            send_buffer = payload.encode()
            sock.sendto(send_buffer, (self.address, self.PORT))
            print("Mensaje enviado:", payload)
        except Exception as e:
            print("Error:", e)
        finally:
            if sock:
                sock.close()

        return 0

# Para pruebas directas
if __name__ == "__main__":
    client = Client("127.0.0.1")  # Cambia la IP si el server está en otro host
    msg = input("Ingrese mensaje para enviar: ")
    client.send(msg)
