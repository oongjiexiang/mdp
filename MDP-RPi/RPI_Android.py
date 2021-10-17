###################################################
#
# Note:
# To test RPI_Android code without MultiProcess, 
# uncomment all the commented lines in the code
#
###################################################

import socket
from bluetooth import *
import os

from config import RFCOMM_CHANNEL, UUID, ANDROID_SOCKET_BUFFER_SIZE
from colorama import *

init(autoreset=True)


class AndroidComm:
    def __init__(self):
        self.server = None
        self.client = None

        os.system('sudo hciconfig hci0 piscan')
        self.server = BluetoothSocket(RFCOMM)
        self.server.bind(('', RFCOMM_CHANNEL))
        self.server.listen(RFCOMM_CHANNEL)
        self.port = self.server.getsockname()[1]

        advertise_service(
            self.server, 'MDPGrp25_BT',
            service_id=UUID,
            service_classes=[UUID, SERIAL_PORT_CLASS],
            profiles=[SERIAL_PORT_PROFILE],
        )
        print(Fore.LIGHTYELLOW_EX + '[BT] Waiting for BT connection on RFCOMM channel %d' % self.port)

    def connect_android(self):
        while True:
            retry = False

            try:
                print(Fore.LIGHTYELLOW_EX + '[AND-CONN] Connecting to Android...')

                if self.client is None:
                    self.client, address = self.server.accept()
                    print(Fore.LIGHTGREEN_EX + '[AND-CONN] Successful connected with Android: %s ' % str(address))
                    retry = False

            except Exception as e:
                print(Fore.RED + '[AND-CONN ERROR] %s' % str(e))

                if self.client is not None:
                    self.client.close()
                    self.client = None

                retry = True

            if not retry:
                break

            print(Fore.LIGHTYELLOW_EX + '[AND-CONN] Retrying connection with Android...')

    def disconnect_android(self):
        try:
            if self.client is not None:
                self.client.shutdown(socket.SHUT_RDWR)
                self.client.close()
                self.client = None
                print(Fore.LIGHTWHITE_EX + '[AND-DCONN] Disconnecting Client Socket')

        except Exception as e:
            print(Fore.RED + '[AND-DCONN ERROR] %s' % str(e))

    def disconnect_all(self):
        try:
            if self.client is not None:
                self.client.shutdown(socket.SHUT_RDWR)
                self.client.close()
                self.client = None
                print(Fore.LIGHTWHITE_EX + '[AND-DCONN] Disconnecting Client Socket')

            if self.server is not None:
                self.server.shutdown(socket.SHUT_RDWR)
                self.server.close()
                self.server = None
                print(Fore.LIGHTWHITE_EX + '[AND-DCONN] Disconnecting Server Socket')

        except Exception as e:
            print(Fore.RED + '[AND-DCONN ERROR] %s' % str(e))

    def read_from_android(self):
        try:
            msg = self.client.recv(ANDROID_SOCKET_BUFFER_SIZE).strip()
            #print('Transmission from Android:')
            #print('\t %s' % msg.decode())

            if msg is None:
                return None

            if len(msg) > 0:
                return msg

            return None

        except BluetoothError as e:
            print(Fore.RED + '[AND-READ ERROR] %s' % str(e))
            raise e

    def write_to_android(self, message):
        try:
            #print('Transmitted to Android:')
            #print('\t %s' % message)
            self.client.send(message)

        except BluetoothError as e:
            print(Fore.RED + '[AND-WRITE ERROR] %s' % str(e))
            raise e


# if __name__ == '__main__':
#     ser = AndroidComm()
#     ser.__init__()
#     ser.connect_android()
#     while True:
#         try:
#             print('In loop')
#             ser.read_from_android()
#             ser.write_to_android('from rpi')
#         except KeyboardInterrupt:
#             print('Android communication interrupted.')
#             ser.disconnect_android()
#             break

