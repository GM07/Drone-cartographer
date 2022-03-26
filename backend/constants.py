""""File containing constants for Drone Communication"""
from enum import Enum

# Max timeout for the connection with the simulation
MAX_TIMEOUT = 10


class COMMANDS(Enum):
    """Enum for all possible drone commands"""
    IDENTIFY = 0x1,
    LAUNCH = 0x2,
    LAND = 0x3,


# Objects to communicate with Crazyflie

URI = ['radio://0/80/2M/E7E7E7E761', 'radio://0/80/2M/E7E7E7E762']

RECOMPILE_SIMULATION_COMMAND = "docker exec embedded sh -c 'cd /workspaces/INF3995-106/embedded/simulation && if test -d build; then rm -rf build; fi && mkdir build && cd build && cmake -GNinja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ../../ && ninja'"
RECOMPILE_EMBEDDED_COMMAND = "docker exec embedded sh -c 'cd /workspaces/INF3995-106/embedded/embedded-firmware && make clean && make -j`nproc`'"