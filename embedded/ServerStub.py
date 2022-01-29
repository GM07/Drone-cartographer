import socket
import os, os.path

if os.path.exists("/tmp/socket"):
  os.remove("/tmp/socket")

server = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
server.bind("/tmp/socket")

#Wait for connection
server.listen(1)
conn, addr = server.accept()
while True:
  message = input("Send message through socket ")
  conn.send(message.encode())
  