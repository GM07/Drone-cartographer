"""This module is used to declare the Abstract communication class that
is used to communicate """
from abc import abstractmethod
from flask_socketio import SocketIO
from constants import COMMANDS


class AbstractComm:
    DELAY_RECEIVER_MS = 1000

    def __init__(self, socketIO: SocketIO):
        self.SOCKETIO = socketIO
        self.drone_list = []

    def set_drone(self, drone_list):
        self.drone_list = drone_list

    def get_drones(self):
        return self.drone_list

    @abstractmethod
    def send_command(self, command: COMMANDS, links=[]) -> None:
        pass

    def shutdown(self):
        pass
