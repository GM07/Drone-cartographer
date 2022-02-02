from dataclasses import dataclass
import socket
import os, os.path

@dataclass
class Commands:
  kIdentify = 0x01
  kTakeoff = 0x02
  kLand = 0x03


if os.path.exists("/tmp/socket"):
  os.remove("/tmp/socket")

server = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
server.bind("/tmp/socket")

#Wait for connection
server.listen(1)
conn, addr = server.accept()
while True:
  command = input("Send message through socket ")
  data = 0

  if command == "TAKEOFF":
    data = [Commands.kTakeoff]
  elif command == "LAND":
    data = [Commands.kLand]
  elif command == "IDENTIFY":
    data = [Commands.kIdentify]

  conn.send(bytearray(data))
  