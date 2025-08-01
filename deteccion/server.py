import socket
from crc32 import verificar_crc32, decodificar_crc

class Server:
    def __init__(self, port=1111):
        self.port = port

    def start(self):
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.bind(('', self.port))
        print(f"Esperando mensaje...\n")

        try:
            while True:
                data, addr = sock.recvfrom(1024)
                mensaje_bin = data.decode()

                print(f"Mensaje recibido: {mensaje_bin}")

                if verificar_crc32(mensaje_bin):
                    print("Verificación CRC32: Mensaje correcto.\n")
                    print("Mensaje decodificado: ", decodificar_crc(mensaje_bin))
                else:
                    print("Verificación CRC32: Error detectado en la transmisión.\n")

        except KeyboardInterrupt:
            print("\nServidor detenido manualmente.")
        finally:
            sock.close()

if __name__ == "__main__":
    server = Server()
    server.start()
