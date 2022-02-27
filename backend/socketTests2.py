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

server.listen(1)
conn, addr = server.accept()
recieved = conn.recv(4)

print("after", struct.unpack('f', recieved))
print("afterrec ", conn.recv(4))


