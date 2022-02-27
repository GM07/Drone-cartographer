import struct
from time import sleep
import socket
import os, os.path
import threading
from constants import COMMANDS

from services.communication.abstract_comm import AbstractComm
from services.data.drone_data import DroneData


server = None 
file_name = '/tmp/socket/{}{}'.format("data", "s0")

if os.path.exists(file_name):
    os.remove(file_name)
server = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
server.bind(file_name)

print("bound")
server.listen(1)
conn, addr = server.accept()

print("accept")

while(True):
    received = conn.recv(32)
    if (len(received) > 4):
        data = DroneData(received)
        print(data)


