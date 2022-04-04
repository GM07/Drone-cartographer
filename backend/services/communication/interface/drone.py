"""Module that holds the Drone dataclass that designates
the name and position of the drone"""
from dataclasses import dataclass


@dataclass
class Drone:
    name: str
    x_pos: float
    y_pos: float
    orientation: float
