from dataclasses import dataclass
from socket import SocketIO
import threading
from typing import Any
import unittest
from unittest import mock
from services.data.drone_data import DroneData, Point2D
import services.data.map as map


@dataclass
class SensorStub:
    front: Any
    right: Any
    back: Any
    left: Any


@dataclass
class DroneDataStub:
    position: Point2D
    sensors: SensorStub

    def update_sensors(self, *args):
        pass


@dataclass
class MapDataStub:
    drone_id: Any
    drone_data: DroneDataStub

    def to_socket_data(self, *args):
        return 'test'


class TestApplication(unittest.TestCase):

    def test_map_data_init(self):
        test_id = 1
        test_data = {}
        map_data = map.MapData(test_id, test_data)
        self.assertEqual(map_data.drone_data, test_data)
        self.assertEqual(map_data.drone_id, test_id)

    def test_to_socket_data(self):
        test_id = 1
        test_data = DroneDataStub(Point2D(0, 0), SensorStub(0, 1, 2, 3))
        map_data = map.MapData(test_id, test_data)
        socket_data = map_data.to_socket_data()
        self.assertEqual(socket_data['sensors']['front'], 0)
        self.assertEqual(socket_data['sensors']['back'], 2)
        self.assertEqual(socket_data['sensors']['right'], 1)
        self.assertEqual(socket_data['sensors']['left'], 3)
        self.assertEqual(socket_data['id'], map_data.drone_id)
        self.assertEqual(socket_data['position'], [0, 0])

    @mock.patch('services.data.map', SensorStub)
    @mock.patch('services.data.map.Map.mean_data_per_sensor')
    @mock.patch('services.data.map.Map.emit_data')
    @mock.patch('services.data.map.Map.filter_sensor_value')
    def test_add_data(self, filter_mock: mock.MagicMock,
                      emit_data_mock: mock.MagicMock,
                      mean_mock: mock.MagicMock):

        mean_mock.return_value = [0, 1, 2, 3]
        test = map.Map()
        test.add_data(
            MapDataStub('1', DroneDataStub(Point2D(0, 0),
                                           SensorStub(0, 1, 2, 3))), {})
        test.add_data(
            MapDataStub('1', DroneDataStub(Point2D(0, 0),
                                           SensorStub(0, 1, 2, 3))), {})
        test.add_data(
            MapDataStub('1', DroneDataStub(Point2D(0, 0),
                                           SensorStub(0, 1, 2, 3))), {})
        test.add_data(
            MapDataStub('1', DroneDataStub(Point2D(0, 0),
                                           SensorStub(0, 1, 2, 3))), {})

        self.assertEqual(filter_mock.call_count, 4)
        filter_mock.assert_called_with(3)

        emit_data_mock.assert_called_once()
        mean_mock.assert_called_once()

    def test_filter_sensor_value(self):
        test = map.Map()
        self.assertEqual(test.filter_sensor_value(2001), -1)
        self.assertEqual(test.filter_sensor_value(0), 0)

    def test_emit_data(self):
        mock_socket = mock.MagicMock()
        test = map.Map()
        test.emit_data(MapDataStub('1', {}), mock_socket)
        mock_socket.emit.assert_called_once_with(
            'getMapData',
            ['test'],
            namespace='/getAllMapData',
            broadcast=True,
        )

    def test_get_all_filtered_data(self):
        size = 10
        map.Map.filtered_data = [MapDataStub(0, {})] * size
        res = map.Map.get_all_filtered_data()
        self.assertEqual(res, ['test'] * size)

    def test_filter_outliers(self):
        initial_data = [0, 0, 0, 0, 0, 0, 0, 0, 0]
        counter_initial_data = 2
        self.assertEqual(
            map.Map().filter_outliers(initial_data, counter_initial_data), 0)

    @mock.patch('services.data.map.Map.filter_outliers')
    def test_mean_data_per_sensor(self, filter_mock: mock.MagicMock):
        drone_list = [DroneDataStub(Point2D(0, 0), SensorStub(1, 1, 1, 1))] * 4
        drone_list.append(
            DroneDataStub(Point2D(0, 0), SensorStub(-1, -1, -1, -1)))
        map.Map().mean_data_per_sensor(drone_list)
        self.assertEqual(filter_mock.call_count, 4)
        filter_mock.assert_has_calls(
            [
                mock.call([1, 1, 1, 1], 0),
                mock.call([1, 1, 1, 1], 0),
                mock.call([1, 1, 1, 1], 0),
                mock.call([1, 1, 1, 1], 0)
            ],
            any_order=False,
        )

    def test_set_drone_len(self):
        drone_len = 0
        test = map.Map()
        test.set_drone_len(0)
        self.assertEqual(0, test.drone_len)
