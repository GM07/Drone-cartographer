import unittest
from unittest import mock

import services.data.drone_data as Drone


class FakeByteDecoder():

    def __init__(
        self,
        data=None,
    ) -> None:
        pass

    def get_values(self):
        data = [0, 1, 2, 3, 4, 5, 6, 4]
        return data


class TestApplication(unittest.TestCase):

    def test_drone_sensors(self):
        sensors = Drone.DroneSensors(0, 1, 2, 3)
        self.assertEqual(sensors.front, 0)
        self.assertEqual(sensors.left, 1)
        self.assertEqual(sensors.back, 2)
        self.assertEqual(sensors.right, 3)

    def test_point(self):
        points = Drone.Point2D(0, 1)
        self.assertEqual(points.x, 0)
        self.assertEqual(points.y, 1)

    def test_drone_state(self):
        self.assertEqual(Drone.DroneState(0), Drone.DroneState.IDLE)
        self.assertEqual(Drone.DroneState(1), Drone.DroneState.TAKING_OFF)
        self.assertEqual(Drone.DroneState(2), Drone.DroneState.LANDING)
        self.assertEqual(Drone.DroneState(3), Drone.DroneState.DEAD)
        self.assertEqual(Drone.DroneState(4), Drone.DroneState.EXPLORING)
        self.assertEqual(Drone.DroneState(5),
                         Drone.DroneState.RETURNING_TO_BASE)
        self.assertEqual(Drone.DroneState(6), Drone.DroneState.CRASHED)

    @mock.patch('services.data.drone_data.DroneData._DroneData__from_bytes')
    def test_drone_data(self, byte: mock.MagicMock):
        Drone.DroneData('', {})
        byte.assert_called()

    @mock.patch('services.data.drone_data.DroneState', mock.MagicMock)
    @mock.patch('services.data.drone_data.Point2D', mock.MagicMock)
    @mock.patch('services.data.drone_data.DroneSensors', mock.MagicMock)
    @mock.patch('services.data.drone_data.ByteDecoder')
    def test_drone_from_bytes(self, byte_decoder: mock.MagicMock):

        data = [0, 1, 2, 3, 4, 5, 6, 4]

        byte_decoder.return_value = FakeByteDecoder()
        drone_data = Drone.DroneData('', {})
        drone_data._DroneData__from_bytes(data)
        self.assertEqual(data[6], drone_data.battery_level)

    @mock.patch('services.data.drone_data.ByteDecoder')
    def test__str__(self, byte_decoder: mock.MagicMock):

        byte_decoder.return_value = FakeByteDecoder()
        drone_data = Drone.DroneData('', {})

        self.assertTrue(drone_data.__str__().find('Exploring'))

    def test_log_entry(self):
        self.assertEqual(None, Drone.log_entry_to_drone_data({}))
