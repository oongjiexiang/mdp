import serial
import time

from config import SERIAL_PORT, BAUD_RATE
from colorama import *

init(autoreset=True)


class STM:
    def __init__(self):
        self.port = SERIAL_PORT
        self.baud_rate = BAUD_RATE
        self.STM_connection = None

    def connect_STM(self):
        print(Fore.LIGHTYELLOW_EX + '[STM-CONN] Waiting for serial connection...')
        while True:
            retry = False

            try:
                self.STM_connection = serial.Serial(self.port, self.baud_rate)

                if self.STM_connection is not None:
                    print(Fore.LIGHTGREEN_EX + '[STM-CONN] Successfully connected with STM:')
                    retry = False

            except Exception as e:
                print(Fore.RED + '[STM-CONN ERROR] %s' % str(e))
                retry = True

            if not retry:
                break

            print(Fore.LIGHTYELLOW_EX + '[STM-CONN] Retrying connection with STM...')
            time.sleep(1)

    def disconnect_STM(self):
        try:
            if self.STM_connection is not None:
                self.STM_connection.close()
                self.STM_connection = None
                print(Fore.LIGHTWHITE_EX + '[STM-DCONN ERROR] Successfully closed connection')

        except Exception as e:
            print(Fore.RED + '[STM-DCONN ERROR] %s' % str(e))

    def read_from_STM(self):
        try:
            self.STM_connection.flush()
            get_message = self.STM_connection.readline().strip()
            get_message = get_message[-5:]
            print('Transmission from STM:')
            print('\t %s' % get_message)

            if len(get_message) > 0:
                return get_message

            return None

        except Exception as e:
            print(Fore.RED + '[STM-READ ERROR] %s' % str(e))
            raise e

    def write_to_STM(self, message):
        try:
            if self.STM_connection is None:
                print(Fore.LIGHTYELLOW_EX + '[STM-CONN] STM is not connected. Trying to connect...')
                self.connect_STM()

            print('Transmitted to STM:')
            print('\t %s' % message)
            self.STM_connection.write(message)

        except Exception as e:
            print(Fore.RED + '[STM-WRITE Error] %s' % str(e))
            raise e


if __name__ == '__main__':
    ser = STM()
    ser.__init__()
    ser.connect_STM()
    while True:
        try:
            msg = input("Enter message to send to STM: ")
            ser.write_to_STM(msg.encode())

        except KeyboardInterrupt:
            print('Serial Communication Interrupted.')
            ser.disconnect_STM()
            break
