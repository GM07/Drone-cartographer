"""This module has the CommSimulation class that is used to
communicate with the simulation """

from errno import EBADF, EINVAL, EWOULDBLOCK
import socket
import re
import os
import os.path
import threading
from typing import Dict, List
from constants import COMMANDS
import queue
from flask_socketio import SocketIO
from time import sleep
from services.communication.interface.drone import Drone
from services.data.simulation_wrappers import DroneSimulationSocket, CommandWrapper

from services.data.drone_data import DroneData
from services.data.map import Map, MapData
from services.communication.abstract_comm import AbstractComm
from services.data.starting_drone_data import StartingDroneData

DELAY = 0.1


class CommSimulation(AbstractComm):
    """This class is used to communicate with the crazyflie
    drones and inherits from the AbstractComm class
    An example use is
    comm=CommSimulation()
    comm.send_command('Launch')"""
    SOCKET_COMMAND_PATH = '/tmp/socket/{}'
    SOCKET_DATA_PATH = '/tmp/socket/data{}'

    def __init__(self,
                 socketIO: SocketIO,
                 drone_list: List[StartingDroneData] = []):
        super().__init__(socketIO, drone_list)
        print('Drone list: ', drone_list)
        Map().set_drone_len(len(drone_list))
        for drone in drone_list:
            drone.name = self.validate_name(drone.name)

        self.set_drone(drone_list)

        self.thread_active = True
        self.__COMMANDS_QUEUE = queue.Queue(20)
        self.__COMMANDS_THREAD = threading.Thread(
            target=self.__send_command_tasks_wrapper,
            daemon=True,
            name='[Simulation] Commands thread')
        self.__RECEIVE_THREAD = threading.Thread(
            target=self.__receive_data_tasks_wrapper,
            daemon=True,
            name='[Simulation] Receiving thread')

        self.command_servers: Dict[str, DroneSimulationSocket] = {}
        self.data_servers: Dict[socket.socket,
                                socket.socket] = {}  # (server, connection)

        for i in range(self.nb_connections):
            file_name = self.SOCKET_COMMAND_PATH.format(self.drone_list[i].name)
            self.command_servers[
                self.drone_list[i].name] = DroneSimulationSocket(
                    None, self.__init_server_bind(file_name))

            file_name = self.SOCKET_DATA_PATH.format(self.drone_list[i].name)
            self.data_servers[self.__init_server_bind(file_name)] = None

        if self.nb_connections > 0:
            self.__COMMANDS_THREAD.start()
            self.__RECEIVE_THREAD.start()
        self.mission_start_time = 0
        self.mission_end_time = 0

    def shutdown(self):
        self.thread_active = False
        try:
            self.__COMMANDS_QUEUE.get_nowait()
        except queue.Empty:
            pass
        self.__COMMANDS_QUEUE.put_nowait(None)

        for _, drone_simulation_socket in self.command_servers.items():
            drone_simulation_socket.server.shutdown(socket.SHUT_RDWR)
            if drone_simulation_socket.conn is not None:
                drone_simulation_socket.conn.shutdown(socket.SHUT_RDWR)

        for server, connection in self.data_servers.items():
            server.shutdown(socket.SHUT_RDWR)
            if connection is not None:
                connection.shutdown(socket.SHUT_RDWR)

        return super().shutdown()

    def send_command(self, command: COMMANDS, links=[], args: bytes = None):
        try:
            for link in links:
                command_to_send = CommandWrapper(link, command, args)
                self.__COMMANDS_QUEUE.put_nowait(command_to_send)
        except queue.Full:
            self.send_log("Command queue full")
            print("Command queue is full")
            pass

    def __init_server_bind(self, file_name: str):
        if os.path.exists(file_name):
            os.remove(file_name)
        server = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        server.bind(file_name)
        server.listen(1)
        return server

    def __receive_data_tasks_wrapper(self):
        if self.nb_connections <= 0:
            return
        print('Receiving thread started')
        while self.thread_active:
            CommSimulation.__thread_attempt_data_socket_connection(
                self.data_servers)
            if self.thread_active:
                self.__receive_data()

    def __send_command_tasks_wrapper(self):
        if self.nb_connections <= 0:
            return
        print('Sending thread started')
        while self.thread_active:
            CommSimulation.__thread_attempt_socket_connection(
                self.command_servers)
            if self.thread_active:
                self.__thread_send_command()

    @staticmethod
    def __thread_attempt_socket_connection(
            sockets: Dict[str, DroneSimulationSocket]):
        for _, drone_simulation_socket in sockets.items():
            if drone_simulation_socket.conn is None:
                try:
                    drone_simulation_socket.server.setblocking(True)
                    drone_simulation_socket.server.settimeout(None)
                    conn, _ = drone_simulation_socket.server.accept()
                    drone_simulation_socket.conn = conn
                except socket.error as socket_error:
                    if socket_error.errno == EINVAL or socket_error.errno == EBADF:
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
                    conn, _ = server.accept()
                    server_dict[server] = conn
                    at_least_one_connected = True
                except socket.error as socket_error:
                    if socket_error.errno == EINVAL or socket_error.errno == EBADF:
                        return
                    elif socket_error.errno != EWOULDBLOCK:
                        raise

    def __receive_data(self):
        ack = 0x01

        at_least_one_connected = True
        while at_least_one_connected and self.thread_active:
            at_least_one_connected = False

            count: int = 0
            for server, other_server in self.data_servers.items():
                can_gather_data = other_server is not None

                if not can_gather_data:
                    try:
                        server.setblocking(False)
                        new_conn, _ = server.accept()
                        self.data_servers[server] = new_conn
                        print(other_server, self.data_servers[server])
                        can_gather_data = True
                    except socket.error as socket_error:
                        if socket_error.errno == EINVAL or socket_error.errno == EBADF:
                            return
                        elif socket_error.errno != EWOULDBLOCK:
                            raise

                if can_gather_data:
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
                        data = DroneData(self.drone_list[count].name, received)
                        Map().add_data(MapData(str(data.name), data),
                                       self.SOCKETIO)
                        self.send_log(f'Drone {data.name}' + data.__str__())
                        self.send_drone_status([data.to_dict()])
                        self.set_drone_data(data)

                    if is_socket_broken:
                        self.send_log(f'Broken Socket no {count}')
                        print("Socket broken")
                        self.data_servers[server] = None
                    count += 1

            sleep(DELAY)

            for server in self.data_servers:
                if self.data_servers[server] is not None:
                    try:
                        self.data_servers[server].send(ack.to_bytes(1, 'big'))
                    except (BrokenPipeError, socket.error):
                        self.data_servers[server] = None

    def __thread_send_command(self):
        missing_connection = False
        while not missing_connection and self.thread_active:
            command_wrapper: CommandWrapper = self.__COMMANDS_QUEUE.get()

            if command_wrapper is None:
                return

            full_command = command_wrapper.command + tuple(command_wrapper.args)

            if command_wrapper.command == COMMANDS.LAUNCH.value:
                self.mission_manager.start_current_mission(
                    self.nb_connections, True)
                self.logs = []

            print('Sending command ', full_command, ' to simulation')

            try:
                self.command_servers[command_wrapper.link].conn.send(
                    bytearray(full_command))

                self.send_log(COMMANDS(command_wrapper.command).name)
            except BrokenPipeError:
                print('Command could not be sent, BrokenPipeError')
                self.send_log('Broken Pipe')
                self.command_servers[command_wrapper.link].conn = None
                missing_connection = True
            except socket.error:
                self.send_log('Socket error')
                return

            if command_wrapper.command == COMMANDS.LAND.value:
                self.mission_manager.end_current_mission(self.logs)

    def validate_name(self, name: str) -> str:
        return re.sub(r'[\/:]', '', name)
