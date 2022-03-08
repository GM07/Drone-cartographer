# __init__.py

from time import sleep
import socket
import os, os.path
import threading
from constants import COMMANDS

from services.communication.abstract_comm import AbstractComm
from services.data.drone_data import DroneData

nb_connections = 8
identifier = 's'


class CommSimulation(AbstractComm):

    INIT_TIMEOUT = 0.000001

    def __init__(self):
        self.is_connected = False
        self.servers: list[socket.socket] = []
        self.connections: list[socket.socket] = []
        self.receive_thread = threading.Thread(
            target=self.__receive,
            daemon=True,
            name='[Simulation] Receiving thread')

        for i in range(nb_connections):
            file_name = '/tmp/socket/{}{}'.format(identifier, i)
            if os.path.exists(file_name):
                os.remove(file_name)
            server = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
            server.bind(file_name)
            self.servers.append(server)

        self.attemptSocketConnection(self.INIT_TIMEOUT)

        print('Is connected: ', self.is_connected)
        self.receive_thread.start()

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

        self.is_connected = success

    def send_command(self, command: COMMANDS):

        if not self.is_connected:
            self.attemptSocketConnection(-1)

        if self.is_connected:
            print('Sending command ', command, ' to simulation')
            for conn in self.connections:
                conn.send(bytearray(command))

    def __receive(self):
        print('Receiving thread started')
        while True:

            if self.is_connected:
                for conn in self.connections:
                    self.send_command(COMMANDS.LOGS.value)
                    received = conn.recv(32)
                    if len(received) > 4:
                        data = DroneData(received)
                        #Add log here
                        print(data)
            sleep(AbstractComm.DELAY_RECEIVER_MS / 1000)
