class Receptor:
    def __init__(self):
        self.block_size = 8  # fijo en 8 bits

    def fletcher_checksum(self, data):
        sum1 = 0
        sum2 = 0
        for byte in data:
            sum1 = (sum1 + byte) % 255
            sum2 = (sum2 + sum1) % 255
        return [sum1, sum2]

    def binary_string_to_bytes(self, binary_str):
        return [int(binary_str[i:i + self.block_size], 2) for i in range(0, len(binary_str), self.block_size)]

    def bytes_to_binary_string(self, byte_list):
        return ''.join(format(b, '08b') for b in byte_list)

    def run(self, full_message):
        # Divide el mensaje completo en bloques de 8
        all_bytes = self.binary_string_to_bytes(full_message)

        # Extrae el checksum recibido (últimos dos bloques)
        received_checksum = all_bytes[-2:]
        data_bytes = all_bytes[:-2]

        # Calcula el checksum sobre los datos
        calculated_checksum = self.fletcher_checksum(data_bytes)

        print("Mensaje recibido:", self.bytes_to_binary_string(data_bytes))
        print("Checksum recibido:", self.bytes_to_binary_string(received_checksum))
        print("Checksum calculado:", self.bytes_to_binary_string(calculated_checksum))

        if received_checksum == calculated_checksum:
            print(" El mensaje está correcto.")
        else:
            print(" Se detectaron errores en el mensaje.")
