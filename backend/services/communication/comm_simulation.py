# __init__.py

from time import sleep
import socket
import os, os.path
import threading
from typing import List
from constants import COMMANDS
import queue

from services.communication.abstract_comm import AbstractComm
from services.data.drone_data import DroneData

nb_connections = 8
identifier = 's'

class CommSimulation(AbstractComm):

    def __init__(self):
        self.__COMMANDS_QUEUE = queue.Queue(1)
        self.__COMMANDS_THREAD = threading.Thread(
            target=self.__send_command_tasks_wrapper, daemon=True, name='[Simulation] Commands thread')
        self.__RECEIVE_THREAD = threading.Thread(
            target=self.__receive_data_tasks_wrapper, daemon=True, name='[Simulation] Receiving thread')

        self.command_servers: list[socket.socket] = []
        self.command_connections: list[socket.socket] = []
        self.data_servers: list[socket.socket] = []
        self.data_connections: list[socket.socket] = []

        for i in range(nb_connections):
            file_name = '/tmp/socket/{}{}'.format(identifier, i)
            self.command_servers.append(self.__init_server_bind(file_name))
            file_name = '/tmp/socket/data{}{}'.format(identifier, i)
            self.data_servers.append(self.__init_server_bind(file_name))

        self.__COMMANDS_THREAD.start()
        self.__RECEIVE_THREAD.start()

    def send_command(self, command: COMMANDS):
        try:
            self.__COMMANDS_QUEUE.get_nowait()
        except queue.Empty:
            pass
        self.__COMMANDS_QUEUE.put_nowait(command)

    def __init_server_bind(self, file_name: str):
        if os.path.exists(file_name):
            os.remove(file_name)
        server = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        server.bind(file_name)
        return server


    def __receive_data_tasks_wrapper(self):
        print('Receiving thread started')
        while True:
            self.data_connections = CommSimulation.__thread_attempt_socket_connection(self.data_servers)
            self.__receive_data()

    def __send_command_tasks_wrapper(self):
        print('Sending thread started')
        while True:
            self.command_connections = CommSimulation.__thread_attempt_socket_connection(self.command_servers)
            self.__thread_send_command()

    @staticmethod
    def __thread_attempt_socket_connection(server_list: List[socket.socket]):
        connections: list[socket.socket] = []
        for server in server_list:
            server.listen(1)
            server.settimeout(None)
            conn, addr = server.accept()
            connections.append(conn)
            
        return connections

    def __receive_data(self):
        while True:
            for conn in self.data_connections:
                received = conn.recv(32)
                if len(received) == 0:
                    return
                if (len(received) > 4):
                    data = DroneData(received)
                    print(data)


    def __thread_send_command(self):
        while True:
            command = self.__COMMANDS_QUEUE.get()
            print('Sending command ', command, ' to simulation')
            for conn in self.command_connections:
                try:
                    conn.send(bytearray(command))
                except BrokenPipeError:
                    print('Command could not be sent')
                    return

