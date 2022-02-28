from time import sleep
import socket
import os, os.path
import threading
from constants import COMMANDS

from services.communication.abstract_comm import AbstractComm
from services.communication.comm_simulation import CommSimulation
from services.data.drone_data import DroneData


test = CommSimulation()


while(True):
    sleep(1)

