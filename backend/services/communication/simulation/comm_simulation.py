import socket
import os, os.path

nConnections = 8
identifier = "s"

class CommSimulation: 


    def __init__(self):
        self.servers = []
        self.connections = []

        for i in range(nConnections):
            file_name = "/tmp/socket/{}{}".format(identifier, i)
            if os.path.exists(file_name):
                os.remove(file_name)
            server = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
            server.bind(file_name)
            self.servers.append(server)

        for server in self.servers:
            server.listen(1)
            conn, addr = server.accept()
            self.connections.append(conn)

    def send_command(self, command):
        for conn in self.connections:
            conn.send(bytearray(command))