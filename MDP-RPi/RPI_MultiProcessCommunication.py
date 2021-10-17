from multiprocessing import Process, Value, Queue, Manager
import time
from datetime import datetime

from RPI_PC import PcComm
from RPI_Android import AndroidComm
from RPI_STM import STMComm
from config import MESSAGE_SEPARATOR, NEWLINE
from colorama import *

from picamera import PiCamera
import socket
import cv2
import imagezmq
from imutils.video import VideoStream

#run this program on each RPi to send an image to PC
from picamera.array import PiRGBArray

init(autoreset=True)


class MultiProcessCommunication:
    def __init__(self):

        self.STM = STMComm()
        self.algorithm = PcComm()
        self.android = AndroidComm()

        self.manager = Manager()

        # Messages from STM, Algorithm (PC) and Android are placed in this queue before being read
        self.message_queue = self.manager.Queue()
        self.to_android_message_queue = self.manager.Queue()

        self.read_STM_process = Process(target=self._read_STM)
        self.read_algorithm_process = Process(target=self._read_algorithm)
        self.read_android_process = Process(target=self._read_android)

        self.write_process = Process(target=self._write_target)
        self.write_android_process = Process(target=self._write_android)
        print(Fore.LIGHTGREEN_EX + '[MultiProcess] MultiProcessing initialized')

        self.dropped_connection = Value('i', 0)

        self.sender = None

        self.image_process = Process(target = self._take_pic)
        self.image_queue = self.manager.Queue()

    def start(self):
        try:
            # Connect to STM, Algorithm (PC) and Android
            self.STM.connect_STM()
            self.algorithm.connect_PC()
            self.android.connect_android()

            # Start the process to read and write to various modules (STM, Algorithm [PC] and Android)
            self.read_STM_process.start()
            self.read_algorithm_process.start()
            self.read_android_process.start()
            self.write_process.start()
            self.write_android_process.start()
            startComms_dt = datetime.now().strftime('%d-%b-%Y %H:%M%S')
            print(Fore.LIGHTGREEN_EX + str(startComms_dt) + '| [MultiProcess] Communications started. Reading from STM, Algorithm & Android')
            
            self.image_process.start()

        except Exception as e:
            print(Fore.RED + '[MultiProcess-START ERROR] %s' % str(e))
            raise e
        self._allow_reconnection()

    def _allow_reconnection(self):
        while True:
            try:
                if not self.read_STM_process.is_alive():
                    self._reconnect_STM()

                if not self.read_algorithm_process.is_alive():
                    self._reconnect_algorithm()

                if not self.read_android_process.is_alive():
                    self._reconnect_android()

                if not self.write_process.is_alive():
                    if self.dropped_connection.value == 0:
                        self._reconnect_STM()
                    elif self.dropped_connection.value == 1:
                        self._reconnect_algorithm()

                if not self.write_android_process.is_alive():
                    self._reconnect_android()

            except Exception as e:
                print(Fore.RED + '[MultiProcess-RECONN ERROR] %s' % str(e))
                raise e

    def _reconnect_STM(self):
        self.STM.disconnect_STM()

        self.read_STM_process.terminate()
        self.write_process.terminate()
        self.write_android_process.terminate()

        self.STM.connect_STM()

        self.read_STM_process = Process(target=self._read_STM)
        self.read_STM_process.start()

        self.write_process = Process(target=self._write_target)
        self.write_process.start()

        self.write_android_process = Process(target=self._write_android)
        self.write_android_process.start()

        print(Fore.LIGHTGREEN_EX + '[MultiProcess-RECONN] Reconnected to STM')

    def _reconnect_algorithm(self):
        self.algorithm.disconnect_PC()
        #print('right before terminating read_algorithm_process')
        self.read_algorithm_process.terminate()
        self.write_process.terminate()
        self.write_android_process.terminate()

        self.algorithm.connect_PC()

        self.read_algorithm_process = Process(target=self._read_algorithm)
        self.read_algorithm_process.start()

        self.write_process = Process(target=self._write_target)
        self.write_process.start()

        self.write_android_process = Process(target=self._write_android)
        self.write_android_process.start()

        print(Fore.LIGHTGREEN_EX + '[MultiProcess-RECONN] Reconnected to Algorithm')

    def _reconnect_android(self):
        self.android.disconnect_android()

        self.read_android_process.terminate()
        self.write_process.terminate()
        self.write_android_process.terminate()

        self.android.connect_android()

        self.read_android_process = Process(target=self._read_android)
        self.read_android_process.start()

        self.write_process = Process(target=self._write_target)
        self.write_process.start()

        self.write_android_process = Process(target=self._write_android)
        self.write_android_process.start()

        print(Fore.LIGHTGREEN_EX + '[MultiProcess-RECONN] Reconnected to Android')

    def _format_for(self, target, message):
        # Function to return a dictionary containing the target and the message
        return {
            'target': target,
            'payload': message,
        }

    def _read_STM(self):
        while True:
            try:
                raw_message = self.STM.read_from_STM()

                if raw_message is None:
                    continue
                print(Fore.LIGHTCYAN_EX + "STM Message received " + raw_message.decode())
                raw_message_list = raw_message.decode().splitlines()
                for pre_message_list in raw_message_list:
                    if len(pre_message_list) != 0:

                        message_list = pre_message_list.split(MESSAGE_SEPARATOR, 1)

                        if message_list[0] == 'AND':
                            print(Fore.LIGHTRED_EX + 'STM > %s , %s' % (str(message_list[0]), str(message_list[1])))
                            self.to_android_message_queue.put_nowait(message_list[1].encode())

                        elif message_list[0] == 'ALG':
                            print(Fore.LIGHTRED_EX + 'STM > %s , %s' % (str(message_list[0]), str(message_list[1])))
                            self.message_queue.put_nowait(self._format_for(message_list[0], (message_list[1]).encode()))

                        else:
                            # Printing message without proper message format on RPi terminal for STM sub-team to debug
                            print(Fore.LIGHTBLUE_EX + '[Debug] Message from STM: %s' % str(message_list))

            except Exception as e:
                print(Fore.RED + '[MultiProcess-READ-STM ERROR] %s' % str(e))
                break

    def _read_algorithm(self):
        while True:
            try:
                raw_message = self.algorithm.read_from_pc()
                if raw_message is None:
                    continue
                raw_message_list = raw_message.decode().splitlines()
                
                for pre_message_list in raw_message_list:
                    if len(pre_message_list) != 0:

                        message_list = pre_message_list.split(MESSAGE_SEPARATOR, 1)

                        # Message format for Image Rec: RPI|
                        if message_list[0] == 'RPI':
                            print(Fore.LIGHTGREEN_EX + 'Algo > %s, %s' % (str(message_list[0]), 'take pic'))
                            self.image_queue.put_nowait('take')

                        elif message_list[0] == 'AND':
                            print(Fore.LIGHTGREEN_EX + 'Algo > %s , %s' % (str(message_list[0]), str(message_list[1])))
                            self.to_android_message_queue.put_nowait(message_list[1].encode())

                        else:
                            print(Fore.LIGHTGREEN_EX + 'Algo > %s , %s' % (str(message_list[0]), str(message_list[1])))
                            self.message_queue.put_nowait(self._format_for(message_list[0], message_list[1].encode()))

            except Exception as e:
                print(Fore.RED + '[MultiProcess-READ-ALG ERROR] %s' % str(e))
                break

    def _read_android(self):
        while True:
            try:
                raw_message = self.android.read_from_android()

                if raw_message is None:
                    continue
                #print("Android Message received " + raw_message.decode())
                raw_message_list = raw_message.decode().splitlines()

                for pre_message_list in raw_message_list:
                    if len(pre_message_list) != 0:

                        message_list = pre_message_list.split(MESSAGE_SEPARATOR, 1)

                        if message_list[0] == 'ALG':
                            print(Fore.WHITE + 'Android > %s , %s' % (str(message_list[0]), str(message_list[1])))
                            assert isinstance(message_list, object)
                            self.message_queue.put_nowait(self._format_for(message_list[0], (message_list[1] + NEWLINE).encode()))
                            
                        else:
                            print(Fore.WHITE + 'Android > %s , %s' % (str(message_list[0]), str(message_list[1])))
                            self.message_queue.put_nowait(self._format_for(message_list[0], message_list[1].encode()))

            except Exception as e:
                print(Fore.RED + '[MultiProcess-READ-AND ERROR] %s' % str(e))
                break

    def _write_target(self):
        while True:
            target = None
            try:
                if not self.message_queue.empty():
                    message = self.message_queue.get_nowait()
                    target, payload = message['target'], message['payload']
                    #print('debug: writing out of queue to target')
                    if target == 'ALG':
                        self.algorithm.write_to_pc(payload)
                        time.sleep(0.5)
                    elif target == 'STM':
                        print(Fore.LIGHTCYAN_EX + 'To STM: before write to STM method')
                        self.STM.write_to_STM(payload)
                        print(Fore.LIGHTCYAN_EX + 'To STM: after write to STM method')
                    elif target == 'AND':
                        time.sleep(1)
                        self.android.write_to_android(payload)

            except Exception as e:
                print(Fore.RED + '[MultiProcess-WRITE-%s ERROR] %s' % (str(target), str(e)))

                if target == 'STM':
                    self.dropped_connection.value = 0

                elif target == 'ALG':
                    self.dropped_connection.value = 1

                break

    def _write_android(self):
        while True:
            try:
                if not self.to_android_message_queue.empty():
                    message = self.to_android_message_queue.get_nowait()
                    self.android.write_to_android(message)
            except Exception as e:
                print(Fore.RED + '[MultiProcess-WRITE-AND ERROR] %s' % str(e))
                break
    
    def _take_pic(self):
        # Start the Image Rec process
        self.sender = imagezmq.ImageSender(connect_to='tcp://192.168.25.8:5555') #Connection to Image Processing Server
        while True:
            try:
                if not self.image_queue.empty():
                    msg = self.image_queue.get_nowait()
                    #print(Fore.LIGHTGREEN_EX + 'Image Server connected')
            
                    self.rpi_name = socket.gethostname() # send RPi hostname with each image
                    #print('debug: image server rpi_name')
                    self.camera = PiCamera(resolution=(640, 640)) #max resolution 2592,1944
                    #print('debug: image server socket')
                    self.rawCapture = PiRGBArray(self.camera)
                    #print('debug: image server rawCapture')

                    #time.sleep(1)  # allow camera sensor to warm up

                    #while True:  # send images as stream until Ctrl-C
                        #time.sleep(5)
                    self.camera.capture(self.rawCapture, format="bgr")
                    self.image = self.rawCapture.array
                    self.rawCapture.truncate(0)
                    #sender.send_image(rpi_name, image)
                    #print('debug: send')
                    self.reply = self.sender.send_image(self.rpi_name, self.image)
                    #print('debug: reply')
                    self.reply = str(self.reply.decode())
                    print(Fore.LIGHTYELLOW_EX + 'Reply message: ' + self.reply)
                    if self.reply == 'n':
                        self.reply = 'RPI,-1'
                        self.message_queue.put_nowait(self._format_for('AND',self.reply.encode()))
                        print(Fore.LIGHTYELLOW_EX + 'Message send across to AND: ' + self.reply)
                        time.sleep(1)
                        self.reply = 'n'
                        self.message_queue.put_nowait(self._format_for('ALG',self.reply.encode()))
                        print(Fore.LIGHTYELLOW_EX + 'Message send across to ALG: ' + self.reply)
                        
                    else:
                        self.reply = 'RPI,' + self.reply
                        self.message_queue.put_nowait(self._format_for('AND',self.reply.encode()))
                        print(Fore.LIGHTYELLOW_EX + 'Message send across to AND: ' + self.reply)
                        time.sleep(1)
                        self.reply = 'y'
                        self.message_queue.put_nowait(self._format_for('ALG',self.reply.encode()))
                        print(Fore.LIGHTYELLOW_EX + 'Message send across to ALG: ' + self.reply)

                    #print('message sent out from Image Rec to ALG & Android:')
                    self.camera.close()
                    #print(reply)
            
            except Exception as e:
                print(Fore.RED + '[MultiProcess-PROCESS-IMG ERROR] %s' % str(e))

