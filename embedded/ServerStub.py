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
        packet = bytearray(command)  # Command must be an array of numbers
        print(packet)
        self.crazyflie.appchannel.send_packet(packet)
        self.crazyflie.close_link()


@dataclass
class Commands:
    kIdentify = 0x01
    kTakeOff = 0x02
    kLand = 0x03
    kStartGradient = 0x04
    kEndGradient = 0x05
    kReturnToBase = 0x06


nConnections = 1
identifier = "s"

# CHANGE TO FALSE TO CONTROL REAL DRONE
is_simulation = True

servers = []
clients = []

# For Drones
# CHANGE TO PROPER DRONE ADDRESS
URI = ['radio://0/80/2M/E7E7E7E762', 'radio://0/80/2M/E7E7E7E761']
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
  orientation = 180
  data = [Commands.kTakeOff]
  for byte in orientation.to_bytes(4, byteorder = 'little'):
    data.append(byte)
  send_command(data)

def land():
    data = [Commands.kLand]
    send_command(data)


def identify():
    data = [Commands.kIdentify]
    send_command(data)


def returnToBase():
    data = [Commands.kReturnToBase]
    send_command(data)


def startGradient():
    data = [Commands.kStartGradient]
    send_command(data)


def endGradient():
    data = [Commands.kEndGradient]
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
window.geometry('500x50')
takeOffButton = Button(window, text='Take off', command=takeOff)
takeOffButton.grid(column=0, row=0)
landButton = Button(window, text='Land', command=land)
landButton.grid(column=1, row=0)
identifyButton = Button(window, text='Identify', command=identify)
identifyButton.grid(column=2, row=0)
returnToBaseButton = Button(window, text='Return to base', command=returnToBase)
returnToBaseButton.grid(column=3, row=0)
identifyButton = Button(window, text='Start Gradient', command=startGradient)
identifyButton.grid(column=4, row=0)
identifyButton = Button(window, text='End Gradient', command=endGradient)
identifyButton.grid(column=5, row=0)
window.mainloop()
