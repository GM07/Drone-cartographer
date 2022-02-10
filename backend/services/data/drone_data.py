from enum import Enum


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

    @staticmethod
    def fromDataPacket(packet: bytearray) -> DroneData:
        pass

    def __init__(self, sensors: DroneSensors, position: Point2D, state: DroneState):
        self.sensors = sensors
        self.position = position
        self.state = state

    def updateWithHeightPacket(packet: bytearray):
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
