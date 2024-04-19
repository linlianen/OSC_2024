from pickle import NONE
import time
from types import NoneType

import serial
import sys
import numpy as np

def echo_lines_until_timeout(ser: serial.Serial, search_for=None):
    # ser.reset_input_buffer()

    line = ser.readline()
    # lines = line.decode()
    print("print:",line)
    while line:
        print(f'Response from UART:{line}')
        line = ser.readline()
        if search_for != None:
            index = str(line).find(search_for)
            if index != -1:
                return index
    return -1

def uart_file_size_handshake(ser: serial.Serial, file_size, imagePath):

    ser.write('reload\n'.encode())
    string = file_size.to_bytes(4,"big")
    time.sleep(1)
    # print(f'size in byte : {(string[0])} {hex(string[1])} {hex(string[2])} {hex(string[3])}')
    ser.write(string)
    # ser.write('\n'.encode())
    ser.flush()
    print(f'{imagePath} size={file_size}\n')


if __name__ == '__main__':
    arg_cnt = len(sys.argv)
    print(arg_cnt)
    if arg_cnt == 1:
        print("Please specify file to send.")
        exit()
    elif arg_cnt == 2 :
        IMAGE_PATH = sys.argv[1]
        COM_NAME = '/dev/ttyUSB0'
    else: 
        IMAGE_PATH = sys.argv[1]
        COM_NAME = sys.argv[2]

    ser = serial.Serial()
    ser.port = COM_NAME
    ser.baudrate = 115200
    
    ser.timeout = 1
    ser.open()

    ser.flush()
    # arr_to_send = b''
    with open(IMAGE_PATH, 'rb') as img_file:
        arr_to_send = img_file.read()
      
    file_size = len(arr_to_send)
    print(f'file size:{file_size}')
    uart_file_size_handshake(ser,file_size,IMAGE_PATH)
    # ser.write(f"{file_size}".encode())
    # print(f"{file_size}".encode())
    # ser.flush()
    # line = ser.readline()
    # print(f"Line:{line}")
    # time.sleep(1)
    print(f"File transmitting...")
    ser.write(arr_to_send)
    
    # ser.write(arr_to_send)
    ser.flush()
    print(f'File transmitted...')
    # echo_lines_until_timeout(ser)

    arr_int = [x for x in arr_to_send]
    print(f'First 20 bytes of {IMAGE_PATH} (HEX)= ', end='')
    for b in arr_int[:20]:
        print("{:02} ".format(b), end='')
    print('')

    # Receive and print remaining character
    # echo_lines_until_timeout(ser)
    print("File successfully transmitted.")
    print("Python script exits here.")
