from dataclasses import dataclass
import socket
import os, os.path
import cflib.crtp
from cflib.crazyflie import Crazyflie

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

nConnections = 8
identifier = "s"

# CHANGE TO FALSE TO CONTROL REAL DRONE
is_simulation = False

servers = []
clients = []

# For Drones
URI = ['radio://0/80/2M/E7E7E7E762']
# CHANGE TO PROPER DRONE ADDRESS
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


while True:
  command = input("Send message through socket ")
  data = 0

  if command == "TAKEOFF":
    data = [Commands.kTakeOff]
  elif command == "LAND":
    data = [Commands.kLand]
  elif command == "IDENTIFY":
    data = [Commands.kIdentify]
  
  if is_simulation:
    for client in clients:
      client.send(bytearray(data))
  else:
    for uri in URI:
      COMM_CRAZYFLIE.send_command(data, uri)
  