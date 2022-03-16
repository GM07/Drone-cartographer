from dataclasses import dataclass
import socket
import os, os.path
import cflib.crtp
from cflib.crazyflie import Crazyflie
from tkinter import *

class CommCrazyflie: 

    def __init__(self):
        cflib.crtp.init_drivers()
        self.crazyflie = Crazyflie()
    
    def send_command(self, command, link_uri):
        self.crazyflie.open_link(link_uri)
        packet = bytearray(command) # Command must be an array of numbers
        print(packet)
        self.crazyflie.appchannel.send_packet(packet)
        self.crazyflie.close_link()

@dataclass
class Commands:
  kIdentify = 0x01
  kTakeOff = 0x02
  kLand = 0x03

nConnections = 9
identifier = "s"

# CHANGE TO FALSE TO CONTROL REAL DRONE
is_simulation = False

servers = []
clients = []

# For Drones
# CHANGE TO PROPER DRONE ADDRESS
URI = ['radio://0/80/2M/E7E7E7E761']
COMM_CRAZYFLIE = CommCrazyflie()

if is_simulation:
  for i in range(nConnections):
    file_name = "/tmp/socket/{}{}".format(identifier, i)
    if os.path.exists(file_name):
      os.remove(file_name)
    server = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    server.bind(file_name)
    servers.append(server)

  for server in servers:
    server.listen(1)
    client, addr = server.accept()
    clients.append(client)


def takeOff():
  data = [Commands.kTakeOff]
  send_command(data)

def land():
  data = [Commands.kLand]
  send_command(data)

def identify():
  data = [Commands.kIdentify]
  send_command(data)

def send_command(data):
  if is_simulation:
    for client in clients:
      client.send(bytearray(data))
  else:
    for uri in URI:
      COMM_CRAZYFLIE.send_command(data, uri)

window = Tk()

window.title('Server stub - Dream team')
window.geometry('300x50')
takeOffButton = Button(window, text='Take off', command=takeOff)
takeOffButton.grid(column=0, row=0)
landButton = Button(window, text='Land', command=land)
landButton.grid(column=1, row=0)
identifyButton = Button(window, text='Identify', command=identify)
identifyButton.grid(column=2, row=0)
window.mainloop()

# while True:
#   command = input("Send message through socket ")
#   data = 0

#   if command == "TAKEOFF":
#     data = [Commands.kTakeOff]
#   elif command == "LAND":
#     data = [Commands.kLand]
#   elif command == "IDENTIFY":
#     data = [Commands.kIdentify]
  
#   if is_simulation:
#     for client in clients:
#       client.send(bytearray(data))
#   else:
#     for uri in URI:
#       COMM_CRAZYFLIE.send_command(data, uri)
  