import binascii
import random 

def mensaje_a_binario(mensaje: str) -> str:
    return ''.join(f'{ord(c):08b}' for c in mensaje)

def crc32_bits(data: bytes) -> str:
    crc = binascii.crc32(data) & 0xFFFFFFFF
    return f'{crc:032b}'  # lo representa como 32 bits

def emitir_crc32(mensaje: str) -> str:
    data_bytes = mensaje.encode('utf-8')
    mensaje_bin = mensaje_a_binario(mensaje)
    crc_bin = crc32_bits(data_bytes)
    return mensaje_bin + crc_bin  # mensaje + CRC en binario
    

def verificar_crc32(mensaje_crc_bin: str) -> bool:
    # Separar mensaje y CRC (últimos 32 bits)
    mensaje_bin = mensaje_crc_bin[:-32]
    crc_recibido = mensaje_crc_bin[-32:]

    # Convertir binario a bytes para volver a calcular el CRC
    mensaje_bytes = bytes(int(mensaje_bin[i:i+8], 2) for i in range(0, len(mensaje_bin), 8))
    crc_calculado = crc32_bits(mensaje_bytes)

    return crc_calculado == crc_recibido

def decodificar_crc(mensaje: str) -> str:
    
    if not verificar_crc32(mensaje):
        return "Error de transmisión: CRC no válido."

    # Extraer los bits del mensaje (sin los 32 del CRC)
    mensaje_sin_crc = mensaje[:-32]

    # Convertir binario a texto
    chars = [chr(int(mensaje_sin_crc[i:i+8], 2)) for i in range(0, len(mensaje_sin_crc), 8)]
    return ''.join(chars)


def añadir_ruido(mensaje: str, cantidad_bits: int = 1) -> str:
    mensaje_lista = list(mensaje)
    indices = random.sample(range(len(mensaje)), k=min(cantidad_bits, len(mensaje)))
    
    for i in indices:
        mensaje_lista[i] = '1' if mensaje_lista[i] == '0' else '0'  # voltea el bit
    
    return ''.join(mensaje_lista)

# mensaje_crc = emitir_crc32("hola")
# mensaje_con_ruido = añadir_ruido(mensaje_crc, cantidad_bits=3)

# print("Mensaje original + CRC :", mensaje_crc)
# print("Mensaje con ruido      :", mensaje_con_ruido)
# print("Verificación:", "Correcto" if verificar_crc32(mensaje_con_ruido) else "Error detectado")