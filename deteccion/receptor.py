class Receptor:
    def __init__(self):
        self.block_size = 8  # Se fija en 8 bits

    def fletcher_checksum(self, data):
        sum1 = 0
        sum2 = 0
        for byte in data:
            sum1 = (sum1 + byte) % 255
            sum2 = (sum2 + sum1) % 255
        return [sum1, sum2]

    def binary_string_to_bytes(self, binary_str):
        return [int(binary_str[i:i + self.block_size], 2) for i in range(0, len(binary_str), self.block_size)]

    def decode_from_binary(self, binary_message):
        return ''.join(chr(int(binary_message[i:i + 8], 2)) for i in range(0, len(binary_message), 8))

    def run(self, original_message, binary_message_with_checksum):
        # Forzar block_size fijo de 8
        self.block_size = 8

        data_part = binary_message_with_checksum[:-2 * self.block_size]
        checksum_part = binary_message_with_checksum[-2 * self.block_size:]

        data = self.binary_string_to_bytes(data_part)
        received_checksum = self.binary_string_to_bytes(checksum_part)

        calculated_checksum = self.fletcher_checksum(data)

        if calculated_checksum == received_checksum:
            print("No se detectaron errores. Mensaje original:", original_message)
            print(" Mensaje decodificado:", self.decode_from_binary(original_message))
        else:
            print(" Se detectaron errores en el mensaje. Mensaje descartado.")
