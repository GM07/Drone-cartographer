import socket
import os, os.path

class CommSimulation: 

    def __init__(self, timeout):
        if os.path.exists("/tmp/socket"):
            os.remove("/tmp/socket") # To prevent unwanted communications for new connections

        server = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        server.bind("/tmp/socket")

        # Wait for connection
        server.settimeout(timeout)
        server.listen(1)
        try:
            self.conn, addr = server.accept()
        except socket.timeout:
            print('Socket connection to simulation timeout')

    def send_command(self, command):
        message = input(command)
        self.conn.send(message.encode())
