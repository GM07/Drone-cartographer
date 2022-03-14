from enum import Enum

# Max timeout for the connection with the simulation
MAX_TIMEOUT = 10


class COMMANDS(Enum):
    IDENTIFY = 0x1,
    LAUNCH = 0x2,
    LAND = 0x3,
    LOGS = 0x4,  # Do not remove comma



# Objects to communicate with Crazyflie
# URI = ['radio://0/80/2M/E7E7E7E761', 'radio://0/80/2M/E7E7E7E762']
URI = ['radio://0/80/2M/E7E7E7E762']
