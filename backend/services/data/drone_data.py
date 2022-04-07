"""This module provides classes such as DroneSensors,
DroneState and DroneData to be able to regroup the information
taken from the drones"""
from enum import Enum
from typing import Any, Dict
from services.communication.byte_decoder import ByteDecoder


class DroneSensors:
    """This class provides an interface for the sensors on the drones """

    def __init__(self, front: float, left: float, back: float, right: float):
        self.front = front
        self.left = left
        self.back = back
        self.right = right


class Point2D:
    """This class provides an interface for coordinates"""

    def __init__(self, x: float, y: float, z=0):
        self.x = x
        self.y = y
        self.z = z


class DroneState(Enum):
    IDLE = 0
    TAKING_OFF = 1
    LANDING = 2
    DEAD = 3
    EXPLORING = 4
    CRASHED = 5


def log_data_to_drone_data(name: str, log_data):
    drone_data: DroneData = DroneData(name, bytes(), True)
    drone_data.position.x = log_data['kalman.stateX']
    drone_data.position.y = log_data['kalman.stateY']
    drone_data.position.z = log_data['kalman.stateZ']
    drone_data.sensors.front = log_data['range.front']
    drone_data.sensors.back = log_data['range.back']
    drone_data.sensors.left = log_data['range.left']
    drone_data.sensors.right = log_data['range.right']
    drone_data.battery_level = log_data['pm.batteryLevel']
    drone_data.state = DroneState(log_data['custom.droneCustomState'])

    return drone_data


class DroneData:
    """This class regroups all the data taken from the drones"""

    DATA_SIZE: int = 29

    def __init__(self, name: str, data: bytes, empty=False):
        self.name = name
        self.position = Point2D(0, 0)
        self.sensors = DroneSensors(0, 0, 0, 0)
        self.battery_level = 100
        self.state = DroneState(0)
        if not empty:
            self.__from_bytes(data)

    def update_sensors(self, sensors: DroneSensors):
        self.sensors = sensors

    def to_dict(self) -> Dict[str, Any]:
        return {
            'name': self.name,
            'sensorFront': self.sensors.front,
            'sensorLeft': self.sensors.left,
            'sensorBack': self.sensors.back,
            'sensorRight': self.sensors.right,
            'xPos': self.position.x,
            'yPos': self.position.y,
            'zPos': self.position.z,
            'batteryLevel': self.battery_level,
            'state': self.state.name
        }

    def __from_bytes(self, data: bytes):
        decoder: ByteDecoder = ByteDecoder(
            data, ['f', 'f', 'f', 'f', 'f', 'f', 'f', 'b'])
        values = decoder.get_values()
        self.sensors = DroneSensors(values[0], values[1], values[2], values[3])
        self.position = Point2D(values[4], values[5])
        self.battery_level = values[6]
        self.state = DroneState(values[7])

    def __str__(self) -> str:
        return 'Drone Information :\n Sensor (f, l, b, r) : ' \
            + f'{self.sensors.front:.3f}' + ', '\
            + f'{self.sensors.left:.3f}' + ', ' \
            + f'{self.sensors.back:.3f}' + ', ' \
            + f'{self.sensors.right:.3f}' + ', ' \
            + ' \nPosition (x, y, z) : ' \
            + f'{self.position.x:.3f}' + ', ' \
            + f'{self.position.y:.3f}' + ', ' \
            + f'{self.position.z:.3f}' + ', ' \
            + ' \nBattery level ' \
            + f'{self.battery_level:.3f}' \
            + ' \nState ' \
            + str(self.state.name) \
            + '\n\n'


def log_entry_to_drone_data(log_entry):
    pass


"""
Drone GetData (0x04)

0   Front   
1
2
3
4   Left
5
6
7
8   Back
9
10
11
12  Right
13
14
15
16  X
17
18
19
20  Y
21
22
23
24  Battery
25
26
27
28  State - Crash, Take Off, Exploration, Landing, Idle
29  
30  
31  
32  

Drone GetHeight (0x05)

0   Height
1
2
3

Drone GetLogs (0x06)

String with \0 at the end. If string is too long (> 32 bytes). 'Invalid String Length' will be sent

"""
