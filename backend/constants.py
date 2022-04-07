""""File containing constants for Drone Communication"""
from enum import Enum

# Max timeout for the connection with the simulation
MAX_TIMEOUT = 10


class COMMANDS(Enum):
    """Enum for all possible drone commands"""
    IDENTIFY = 0x1,
    LAUNCH = 0x2,
    LAND = 0x3,
    START_P2P = 0x4,
    END_P2P = 0x5,


# Objects to communicate with Crazyflie

URI = ['radio://0/80/2M/E7E7E7E761', 'radio://0/80/2M/E7E7E7E762']
