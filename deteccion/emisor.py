class Sender:
    def __init__(self):
        self.block_size = 8  # fijo en 8 bits

    def fletcher_checksum(self, data):
        sum1 = 0
        sum2 = 0
        for byte in data:
            sum1 = (sum1 + byte) % 255
            sum2 = (sum2 + sum1) % 255
        return [sum1, sum2]

    def encode_to_binary(self, message):
        return ''.join(format(ord(c), '08b') for c in message)

    def binary_string_to_bytes(self, binary_str):
        return [int(binary_str[i:i + self.block_size], 2) for i in range(0, len(binary_str), self.block_size)]

    def bytes_to_binary_string(self, byte_list):
        return ''.join(format(b, '08b') for b in byte_list)

    def run(self, message):
        binary_message = self.encode_to_binary(message)
        data_bytes = self.binary_string_to_bytes(binary_message)

        checksum = self.fletcher_checksum(data_bytes)
        checksum_binary = self.bytes_to_binary_string(checksum)

        full_message = binary_message + checksum_binary
        print("Mensaje enviado:", full_message)
        return binary_message+';'+full_message
