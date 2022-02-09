import socket
import os, os.path

nConnections = 8
identifier = "s"

class CommSimulation: 

    INIT_TIMEOUT = 0.000001

    def __init__(self):
        self.isConnected = False
        self.servers: list[socket.socket] = []
        self.connections: list[socket.socket] = []

        for i in range(nConnections):
            file_name = "/tmp/socket/{}{}".format(identifier, i)
            if os.path.exists(file_name):
                os.remove(file_name)
            server = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
            server.bind(file_name)
            self.servers.append(server)

        self.attemptSocketConnection(self.INIT_TIMEOUT)

        print("Is connected: ", self.isConnected)

    def attemptSocketConnection(self, timeout: float = 0.5):
        success = True
        self.connections = []
        for server in self.servers:
            try:
                server.listen(1)
                if timeout > 0:
                    server.settimeout(timeout)
                else:
                    server.settimeout(None)
                conn, addr = server.accept()
                self.connections.append(conn)
            except socket.timeout:
                success = False
        
        self.isConnected = success


    def send_command(self, command):
        if not self.isConnected:
            self.attemptSocketConnection(-1)

        if self.isConnected:
            for conn in self.connections:
                conn.send(bytearray(command))
