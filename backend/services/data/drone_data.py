from enum import Enum
import pprint
from services.communication.byte_decoder import ByteDecoder


class DroneSensors:
    def __init__(self, front: float, left: float, back: float, right: float):
        self.front = front
        self.left = left
        self.back = back
        self.right = right

class Point2D:
    def __init__(self, x: float, y: float, z: float = 0):
        self.x = x
        self.y = y
        self.z = z

class DroneState(Enum):
    Idle = 0
    Landing = 1
    Exploration = 2
    TakingOff = 3
    Crashed = 4

class DroneData:

    def __init__(self, sensors: DroneSensors, position: Point2D,
        battery_level: float, state: DroneState):
        self.sensors = sensors
        self.position = position
        self.battery_level = battery_level
        self.state = state

    def __init__(self, data: bytes):
        self.__from_bytes(data)

    def __from_bytes(self, data: bytes):
        decoder: ByteDecoder = ByteDecoder(data,
            ['f', 'f', 'f', 'f', 'f', 'f', 'f', 'i'])
        values = decoder.get_values()
        self.sensors = DroneSensors(values[0], values[1], values[2], values[3])
        self.position = Point2D(values[4], values[5])
        self.battery_level = values[6]
        self.state = DroneState(values[7])

    def __str__(self) -> str:
        return 'Drone Information :\n Sensor (f, l, b, r) : ' \
            + str(self.sensors.front) + ', '\
            + str(self.sensors.left) + ', ' \
            + str(self.sensors.back) + ', ' \
            + str(self.sensors.right) + ', ' \
            + ' \nPosition (x, y, z) : ' \
            + str(self.position.x) + ', ' \
            + str(self.position.y) + ', ' \
            + str(self.position.z) + ', ' \
            + ' \nBattery level ' \
            + str(self.battery_level) \
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
