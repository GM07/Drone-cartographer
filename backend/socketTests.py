from time import sleep
import socket
import os, os.path
import threading
from constants import COMMANDS

from services.communication.abstract_comm import AbstractComm
from services.data.drone_data import DroneData


server = None 
file_name = '/tmp/socket/{}{}'.format("test", 0)

def receive():
    sleep(1)
    print("thread")
    s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM) 
    s.connect(file_name)
    sleep(1)
    print("recv", s.recv(4))
    while(True):
        sleep(1) # work
        

if os.path.exists(file_name):
    os.remove(file_name)
server = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
server.bind(file_name)

fake_receive_thread = threading.Thread(target=receive, daemon=True, name='[Simulation] Receiving thread')
fake_receive_thread.start()

server.listen(1)
conn, addr = server.accept()
conn.send("hello".encode())
conn.send("hello".encode())
conn.send("hello".encode())

print("after")
print("afterrec ", conn.recv(4))


