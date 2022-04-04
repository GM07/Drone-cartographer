"""This module tests the Drone Interface."""

import unittest
from services.communication.interface.drone import Drone


class TestApplication(unittest.TestCase):

    def test_drone(self):
        drone = Drone('test', 0, 1, 2)
        self.assertEqual(drone.name, 'test')
        self.assertEqual(drone.x_pos, 0)
        self.assertEqual(drone.y_pos, 1)
        self.assertEqual(drone.orientation, 2)
