###################################################
#
# Note:
# To test RPI_PC code without MultiProcess, 
# uncomment all the commented lines in the code
#
###################################################

import socket
import time

from config import WIFI_IP, WIFI_PORT, ALGORITHM_SOCKET_BUFFER_SIZE
from colorama import *

init(autoreset=True)


class PcComm:
    def __init__(self, ip=WIFI_IP, port=WIFI_PORT):
        self.ip = ip
        self.port = port

        self.connect = None
        self.client = None
        self.address = None

        self.connect = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        self.connect.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.connect.bind((self.ip, self.port))
        self.connect.listen(1)

    def connect_PC(self):
        while True:
            retry = False

            try:
                print(Fore.LIGHTYELLOW_EX + '[ALG-CONN] Listening for PC connections...')

                if self.client is None:
                    self.client, self.address = self.connect.accept()
                    print(Fore.LIGHTGREEN_EX + '[ALG-CONN] Successfully connected with PC: %s' % str(self.address))
                    retry = False

            except Exception as e:
                print(Fore.RED + '[ALG-CONN ERROR] %s' % str(e))

                if self.client is not None:
                    self.client.close()
                    self.client = None
                retry = True

            if not retry:
                break

            print(Fore.LIGHTYELLOW_EX + '[ALG-CONN] Retrying connection with PC...')
            time.sleep(1)

    def disconnect_all(self):
        try:
            if self.connect is not None:
                self.connect.shutdown(socket.SHUT_RDWR)
                self.connect.close()
                self.connect = None
                print(Fore.LIGHTWHITE_EX + '[ALG-DCONN] Disconnecting Server Socket')

            if self.client is not None:
                self.client.shutdown(socket.SHUT_RDWR)
                self.client.close()
                self.client = None
                print(Fore.LIGHTWHITE_EX + '[ALG-DCONN] Disconnecting Client Socket')

        except Exception as e:
            print(Fore.RED + '[ALG-DCONN ERROR] %s' % str(e))

    def disconnect_PC(self):
        try:
            if self.client is not None:
                self.client.shutdown(socket.SHUT_RDWR)
                self.client.close()
                self.client = None
                print(Fore.LIGHTWHITE_EX + '[ALG-DCONN] Disconnecting Client Socket')

        except Exception as e:
            print(Fore.RED + '[ALG-DCONN ERROR] %s' % str(e))

    def read_from_pc(self):
        try:
            data = self.client.recv(ALGORITHM_SOCKET_BUFFER_SIZE).strip()
            #print('Transmission from PC:')
            #print('\t %s' % data)

            if len(data) > 0:
                return data

            return None

        except Exception as e:
            print(Fore.RED + '[ALG-READ ERROR] %s' % str(e))
            raise e

    def write_to_pc(self, message):
        try:
            #print('Transmitted to PC:')
            #print('\t %s' % message)
            self.client.send(message)

        except Exception as e:
            print(Fore.RED + '[ALG-WRITE ERROR] %s' % str(e))
            raise e


# if __name__ == '__main__':
#     ser = PcComm()
#     ser.__init__()
#     ser.connect_PC()
#     time.sleep(3)
#     print('Connection established')
#     while True: 
#         try:
#             ser.read_from_pc()
#             ser.write_to_pc('Received input!'.encode())
#         except KeyboardInterrupt:
#             print('Communication interrupted')
#             ser.disconnect_PC()
#             break
