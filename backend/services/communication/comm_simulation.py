# __init__.py

from errno import EINVAL, EWOULDBLOCK
from time import sleep
import socket
import os, os.path
import threading
from typing import Dict, List
from constants import COMMANDS
import queue

from flask_socketio import SocketIO
from services.communication.abstract_comm import AbstractComm
from services.data.drone_data import DroneData
from services.communication.database.mongo_interface import Mission, Database
from time import perf_counter
from datetime import datetime


class CommSimulation(AbstractComm):

    __SOCKET_COMMAND_PATH = '/tmp/socket/{}'
    __SOCKET_DATA_PATH = '/tmp/socket/data{}'

    def __init__(self, socket_io: SocketIO, drone_list=[]):
        super().__init__(socket_io)
        self.nb_connections = len(drone_list)

        self.threadActive = True
        self.__COMMANDS_QUEUE = queue.Queue(10)
        self.__COMMANDS_THREAD = threading.Thread(
            target=self.__send_command_tasks_wrapper,
            daemon=True,
            name='[Simulation] Commands thread')
        self.__RECEIVE_THREAD = threading.Thread(
            target=self.__receive_data_tasks_wrapper,
            daemon=True,
            name='[Simulation] Receiving thread')

        self.command_servers: Dict[socket.socket,
                                   socket.socket] = {}  # (server, connection)
        self.data_servers: Dict[socket.socket,
                                socket.socket] = {}  # (server, connection)

        for i in range(self.nb_connections):
            file_name = self.__SOCKET_COMMAND_PATH.format(drone_list[i]['name'])
            self.command_servers[self.__init_server_bind(file_name)] = None
            file_name = self.__SOCKET_DATA_PATH.format(drone_list[i]['name'])
            self.data_servers[self.__init_server_bind(file_name)] = None

        self.__COMMANDS_THREAD.start()
        self.__RECEIVE_THREAD.start()
        self.mission_start_time = perf_counter()
        self.mission_end_time = 0
        self.current_mission = Mission(0, len(drone_list), True, 0, [[]])

    def shutdown(self):

        self.mission_end_time = perf_counter()
        self.current_mission.flight_duration = self.mission_start_time - self.mission_end_time
        self.current_mission.logs = self.logs.copy()
        self.threadActive = False
        for server, connection in self.command_servers.items():
            server.shutdown(socket.SHUT_RDWR)
            if connection is not None:
                connection.shutdown(socket.SHUT_RDWR)

        for server, connection in self.data_servers.items():
            server.shutdown(socket.SHUT_RDWR)
            if connection is not None:
                connection.shutdown(socket.SHUT_RDWR)

        database = Database()
        database.upload_mission_info(self.current_mission)
        return super().shutdown()

    def send_command(self, command: COMMANDS):
        try:
            self.__COMMANDS_QUEUE.put_nowait(command)
        except queue.Full:
            self.send_log([(datetime.now().isoformat(), "Command queue full")])
            pass

    def __init_server_bind(self, file_name: str):
        if os.path.exists(file_name):
            os.remove(file_name)
        server = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        server.bind(file_name)
        server.listen(1)
        return server

    def __receive_data_tasks_wrapper(self):
        print('Receiving thread started')
        while self.threadActive:
            CommSimulation.__thread_attempt_data_socket_connection(
                self.data_servers)
            self.__receive_data()

    def __send_command_tasks_wrapper(self):
        print('Sending thread started')
        while self.threadActive:
            CommSimulation.__thread_attempt_socket_connection(
                self.command_servers)
            self.__thread_send_command()

    @staticmethod
    def __thread_attempt_socket_connection(server_dict: Dict[socket.socket,
                                                             socket.socket]):
        for server, connection in server_dict.items():
            if connection is None:
                try:
                    server.setblocking(True)
                    server.settimeout(None)
                    conn, addr = server.accept()
                    server_dict[server] = conn
                except socket.error as socket_error:
                    if socket_error.errno == EINVAL:
                        return
                    else:
                        raise

    @staticmethod
    def __thread_attempt_data_socket_connection(
            server_dict: Dict[socket.socket, socket.socket]):
        at_least_one_connected = False
        for server, connection in server_dict.items():
            if connection is None:
                try:
                    server.setblocking(not at_least_one_connected)
                    server.settimeout(None)
                    conn, addr = server.accept()
                    server_dict[server] = conn
                    at_least_one_connected = True
                except socket.error as socket_error:
                    if socket_error.errno == EINVAL:
                        return
                    elif socket_error.errno != EWOULDBLOCK:
                        raise

    def __receive_data(self):
        ACK = 0x01

        at_least_one_connected = True
        while at_least_one_connected and self.threadActive:
            at_least_one_connected = False
            count: int = 0
            for server in self.data_servers:

                if not self.data_servers[server] is not None:
                    try:
                        server.setblocking(False)
                        new_conn, addr = server.accept()
                        self.data_servers[server] = new_conn
                    except socket.error as socket_error:
                        if socket_error.errno == EINVAL:
                            return
                        elif socket_error.errno != EWOULDBLOCK:
                            raise

                if self.data_servers[server] is not None:
                    at_least_one_connected = True
                    is_socket_broken = False
                    try:
                        received = self.data_servers[server].recv(
                            DroneData.DATA_SIZE)
                    except socket.error:
                        is_socket_broken = True

                    if len(received) == 0:
                        is_socket_broken = True
                    else:
                        data = DroneData(received)
                        self.send_log([(datetime.now().isoformat(),
                                        f'Drone {count}' + data.__str__())])
                        print(data)
                    if is_socket_broken:
                        self.send_log([(datetime.now().isoformat(),
                                        f'Broken Socket no {count}')])
                        print("Socket broken")
                        self.data_servers[server] = None
                    count += 1

            sleep(0.500)

            for server in self.data_servers:
                if self.data_servers[server] is not None:
                    try:
                        self.data_servers[server].send(ACK.to_bytes(1, 'big'))
                    except (BrokenPipeError, socket.error):
                        self.data_servers[server] = None

    def __thread_send_command(self):
        missing_connection = False
        while not missing_connection and self.threadActive:
            command = self.__COMMANDS_QUEUE.get()
            print('Sending command ', command, ' to simulation')
            for server, conn in self.command_servers.items():
                try:
                    conn.send(bytearray(command))

                    self.send_log([(datetime.now().isoformat(), command)])
                except BrokenPipeError:
                    print('Command could not be sent, BrokenPipeError')
                    self.send_log([(datetime.now().isoformat(), 'Broken Pipe')])
                    self.command_servers[server] = None
                    missing_connection = True
                except socket.error:
                    self.send_log([(datetime.now().isoformat(), 'Socket error')
                                  ])
                    return
