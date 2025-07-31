class Sender:
    def __init__(self, block_size=8):
        self.block_size = block_size

    def fletcher_checksum(self, data):
        sum1 = 0
        sum2 = 0
        for b in data:
            sum1 = (sum1 + (b & 0xFF)) % 255
            sum2 = (sum2 + sum1) % 255
        return [sum1, sum2]

    def binary_string_to_list(self, binary_str):
        result = []
        for i in range(0, len(binary_str), self.block_size):
            byte_str = binary_str[i:i+self.block_size]
            byte_val = int(byte_str, 2)
            result.append(byte_val)
        return result

    def encode_to_binary(self, ascii_message):
        binary_message = ""
        for c in ascii_message:
            binary_message += format(ord(c), '08b')
        return binary_message

    def build_payload(self, message):
        binary_message = self.encode_to_binary(message)
        original_message = binary_message

        # Auto-ajustar el block_size si es divisible entre 32 o 16
        if len(binary_message) % 32 == 0:
            self.block_size = 32
        elif len(binary_message) % 16 == 0:
            self.block_size = 16
        else:
            self.block_size = 8

        while len(binary_message) % self.block_size != 0:
            binary_message += "0"

        data = self.binary_string_to_list(binary_message)
        checksum = self.fletcher_checksum(data)

        message_with_checksum = binary_message
        for sum_val in checksum:
            message_with_checksum += format(sum_val, '08b')

        payload = original_message + ";" + message_with_checksum
        return payload
