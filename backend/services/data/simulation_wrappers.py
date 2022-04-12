from dataclasses import dataclass
import socket
from typing import Any

from constants import COMMANDS


@dataclass
class DroneSimulationSocket:
    conn: socket
    server: socket


@dataclass
class CommandWrapper:
    link: str
    command: COMMANDS
    args: Any
