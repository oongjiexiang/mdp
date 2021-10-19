from RPI_MultiProcess import MultiProcess
import time
from colorama import *

init(autoreset=True)

def init():
    try:
        multi = MultiProcess()
        multi.start()
    except Exception as err:
        print(Fore.RED + '[Main.py ERROR] {}'.format(str(err)))


if __name__ == '__main__':
    init()
