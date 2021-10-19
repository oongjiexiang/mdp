from RPI_Algo import Algo
from RPI_Android import Android
from RPI_STM import STM
from colorama import *
from multiprocessing import Process, Value, Queue, Manager
import time
from datetime import datetime

from picamera import PiCamera
import socket
import cv2
import imagezmq

from picamera.array import PiRGBArray

init(autoreset=True)


class MultiProcess:
    def __init__(self):
        self.AND = Android()
        self.ALG = Algo()
        self.STM = STM()

        self.manager = Manager()

        self.to_AND_message_queue = self.manager.Queue()
        self.message_queue = self.manager.Queue()
    
        self.read_AND_process = Process(target=self._read_AND)
        self.read_ALG_process = Process(target=self._read_ALG)
        self.read_STM_process = Process(target=self._read_STM)

        self.write_AND_process = Process(target=self._write_AND)
        self.write_process = Process(target=self._write_target)
        print(Fore.LIGHTGREEN_EX + '[MultiProcess] MultiProcessing initialized')

        self.dropped_connection = Value('i', 0)

        self.sender = None

        self.image_process = Process(target = self._take_pic)
        self.image_queue = self.manager.Queue()

    def start(self):
        try:
            self.AND.connect_AND()
            self.ALG.connect_ALG()
            self.STM.connect_STM()
            
            self.read_AND_process.start()
            self.read_ALG_process.start()
            self.read_STM_process.start()
    
            self.write_AND_process.start()
            self.write_process.start()
            
            startComms_dt = datetime.now().strftime('%d-%b-%Y %H:%M%S')
            print(Fore.LIGHTGREEN_EX + str(startComms_dt) + '| [MultiProcess] Communications started. Reading from STM, Algorithm & Android')
            
            self.image_process.start()

        except Exception as e:
            print(Fore.RED + '[MultiProcess-START ERROR] %s' % str(e))
            raise e

    def _format_for(self, target, message):
        return {
            'target': target,
            'payload': message,
        }

    def _read_AND(self):
        while True:
            try:
                message = self.AND.read_from_AND()

                if message is None:
                    continue

                message_list = message.decode().splitlines()

                for msg in message_list:
                    if len(msg) != 0:

                        messages = msg.split('|', 1)

                        if messages[0] == 'ALG':
                            print(Fore.WHITE + 'AND > %s , %s' % (str(messages[0]), str(messages[1])))
                            assert isinstance(messages, object)
                            self.message_queue.put_nowait(self._format_for(messages[0], (messages[1] + '\n').encode()))
                            
                        else:
                            print(Fore.WHITE + 'AND > %s , %s' % (str(messages[0]), str(messages[1])))
                            self.message_queue.put_nowait(self._format_for(messages[0], messages[1].encode()))

            except Exception as e:
                print(Fore.RED + '[MultiProcess-READ-AND ERROR] %s' % str(e))
                break
    
    def _read_ALG(self):
        while True:
            try:
                message = self.ALG.read_from_ALG()
                if message is None:
                    continue
                message_list = message.decode().splitlines()
                
                for msg in message_list:
                    if len(msg) != 0:

                        messages = msg.split('|', 1)

                        # Message format for Image Rec: RPI|
                        if messages[0] == 'RPI':
                            print(Fore.LIGHTGREEN_EX + 'ALG > %s, %s' % (str(messages[0]), 'take pic'))
                            self.image_queue.put_nowait('take')

                        elif messages[0] == 'AND':
                            print(Fore.LIGHTGREEN_EX + 'ALG > %s , %s' % (str(messages[0]), str(messages[1])))
                            self.to_AND_message_queue.put_nowait(messages[1].encode())

                        else:
                            print(Fore.LIGHTGREEN_EX + 'ALG > %s , %s' % (str(messages[0]), str(messages[1])))
                            self.message_queue.put_nowait(self._format_for(messages[0], messages[1].encode()))

            except Exception as e:
                print(Fore.RED + '[MultiProcess-READ-ALG ERROR] %s' % str(e))
                break

    def _read_STM(self):
        while True:
            try:
                message = self.STM.read_from_STM()

                if message is None:
                    continue
                print(Fore.LIGHTCYAN_EX + "STM Message received " + message.decode())
                message_list = message.decode().splitlines()
                for msg in message_list:
                    if len(msg) != 0:

                        messages = msg.split('|', 1)

                        if messages[0] == 'AND':
                            print(Fore.LIGHTRED_EX + 'STM > %s , %s' % (str(messages[0]), str(messages[1])))
                            self.to_AND_message_queue.put_nowait(messages[1].encode())

                        elif messages[0] == 'ALG':
                            print(Fore.LIGHTRED_EX + 'STM > %s , %s' % (str(messages[0]), str(messages[1])))
                            self.message_queue.put_nowait(self._format_for(messages[0], (messages[1]).encode()))

                        else:
                            print(Fore.LIGHTBLUE_EX + '[Debug] Message from STM: %s' % str(messages))

            except Exception as e:
                print(Fore.RED + '[MultiProcess-READ-STM ERROR] %s' % str(e))
                break

    def _write_AND(self):
        while True:
            try:
                if not self.to_AND_message_queue.empty():
                    message = self.to_AND_message_queue.get_nowait()
                    self.AND.write_to_AND(message)
            except Exception as e:
                print(Fore.RED + '[MultiProcess-WRITE-AND ERROR] %s' % str(e))
                break

    def _write_target(self):
        while True:
            target = None
            try:
                if not self.message_queue.empty():
                    message = self.message_queue.get_nowait()
                    target, payload = message['target'], message['payload']
                    if target == 'ALG':
                        self.ALG.write_to_ALG(payload)
                        time.sleep(0.5)
                    elif target == 'STM':
                        print(Fore.LIGHTCYAN_EX + 'To STM: before write to STM method')
                        self.STM.write_to_STM(payload)
                        print(Fore.LIGHTCYAN_EX + 'To STM: after write to STM method')
                    elif target == 'AND':
                        time.sleep(1)
                        self.AND.write_to_AND(payload)

            except Exception as e:
                print(Fore.RED + '[MultiProcess-WRITE-%s ERROR] %s' % (str(target), str(e)))

                if target == 'STM':
                    self.dropped_connection.value = 0

                elif target == 'ALG':
                    self.dropped_connection.value = 1

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

