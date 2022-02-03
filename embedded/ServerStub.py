from dataclasses import dataclass
import socket
import os, os.path

@dataclass
class Commands:
  kIdentify = 0x01
  kTakeoff = 0x02
  kLand = 0x03

nConnections = 8
identifier = "s"

servers = []
clients = []

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
    data = [Commands.kTakeoff]
  elif command == "LAND":
    data = [Commands.kLand]
  elif command == "IDENTIFY":
    data = [Commands.kIdentify]

  for client in clients:
    client.send(bytearray(data))
  