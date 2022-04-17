"""This module is used to declare the TestStartingDroneData
class that is used to test the StartingDroneData class"""

import unittest

from numpy import equal
from services.data.starting_drone_data import StartingDroneData


class TestStartingDroneData(unittest.TestCase):
    """This class is used to test the StartingDroneData class"""

    def test_init(self):
        drone_data = {
            'name': 'test',
            'startingOrientation': 0,
            'startingXPos': 0,
            'startingYPos': 0
        }
        starting_drone_data = StartingDroneData(drone_data)
        assert starting_drone_data.name is 'test'

    def test_to_string(self):
        drone_data = {
            'name': 'test',
            'startingOrientation': 0,
            'startingXPos': 0,
            'startingYPos': 0
        }
        starting_drone_data = StartingDroneData(drone_data)
        drone_to_string = starting_drone_data.__str__()
        self.assertEqual(
            drone_to_string,
            'name: test starting_x_pos: 0 starting_y_pos: 0 starting_orientation: 0'
        )
