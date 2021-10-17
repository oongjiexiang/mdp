from RPI_MultiProcessCommunication import MultiProcessCommunication
import time
from colorama import *

init(autoreset=True)

def init():
    try:
        multi = MultiProcessCommunication()
        multi.start()
    except Exception as err:
        print(Fore.RED + '[Main.py ERROR] {}'.format(str(err)))


if __name__ == '__main__':
    init()
