import socket
import random
from receptor import run


server = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
ADRESS = '127.0.0.1'
PORT = 1111

def main():
    global adresses, server
    server.bind((ADRESS, PORT))

    sent = False 
    while True:
        print("Waiting for message")
        data, addr = server.recvfrom(1024)
        data = data.decode()
        message, mssg_chsm = clean_message(data)
        print("Recieved message: ", message)
        print("Recieved checksum: ", mssg_chsm)

        run(message, mssg_chsm)



def format_inforcer(message: list, identifier: int)-> list:
    formated_str = ""

    formated_str += str(identifier) + ";"

    for i in message:
        formated_str += str(i) + ","
    formated_str = formated_str[:-1]

    return formated_str


def clean_message(user_message: str)-> list:
    message, message_chsum = user_message.split(";")
    return message, message_chsum
    
    


if __name__ == "__main__":
    main()