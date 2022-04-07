from dataclasses import dataclass
import socket
from typing import Any


@dataclass
class DroneSimulationSocket:
    conn: socket
    server: socket


@dataclass
class CommandWrapper:
    link: str
    command: Any
