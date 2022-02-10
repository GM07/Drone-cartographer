# __init__.py

from time import sleep
import socket
import os, os.path
import threading

from services.communication.abstract_comm import AbstractComm

nConnections = 8
identifier = "s"

class CommSimulation(AbstractComm): 

    INIT_TIMEOUT = 0.000001
    DELAY_RECEIVER_MS = 1000

    def __init__(self):
        self.isConnected = False
        self.servers: list[socket.socket] = []
        self.connections: list[socket.socket] = []
        self.receive_thread = threading.Thread(target=self.__receive)
        self.stopThreads = False

        for i in range(nConnections):
            file_name = "/tmp/socket/{}{}".format(identifier, i)
            if os.path.exists(file_name):
                os.remove(file_name)
            server = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
            server.bind(file_name)
            self.servers.append(server)

        self.attemptSocketConnection(self.INIT_TIMEOUT)

        print("Is connected: ", self.isConnected)
        # self.receive_thread.start()


    def __del__(self):
        self.stopThreads = True

    def attemptSocketConnection(self, timeout: float = 0.5):
        success = True
        self.connections = []
        for index, server in enumerate(self.servers):
            try:
                server.listen(1)
                server.settimeout(timeout if timeout > 0 else None)
                conn, addr = server.accept()
                self.connections.append(conn)
            except socket.timeout as err:
                print('Error with socket ', index, ': ', err)
                success = False
        
        print('Connection accepted')
        self.isConnected = success


    def send_command(self, command):

        if not self.isConnected:
            self.attemptSocketConnection(-1)
        
        if self.isConnected:
            print('Sending command ', command, ' to simulation')
            for conn in self.connections:
                conn.send(bytearray(command))

    def __receive(self):
        print('Receiving thread started')
        while not self.stopThreads:

            print('Drone data receiver running every ', CommSimulation.DELAY_RECEIVER_MS, ' ms, isConnected', self.isConnected)
            if self.isConnected:
                for conn in self.connections:
                    pass
                    # data:bytes = conn.recv(32)
                    # print(data.decode())
            sleep(CommSimulation.DELAY_RECEIVER_MS / 1000)

