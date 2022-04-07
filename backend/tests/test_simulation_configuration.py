import math
import random
import unittest
from unittest import mock
import services.communication.simulation_configuration as Config
from services.data.starting_drone_data import StartingDroneData


class TestApplication(unittest.TestCase):

    def test_box_dataclass(self):
        width = 3
        length = 1
        box = Config.Box(0, 0, length, width, 0)
        expected_width = width + Config.WALL_SPACING
        expected_height = length + Config.WALL_SPACING
        expected_real_x_pos = 0 - expected_width / 2
        expected_real_y_pos = 0 - expected_height / 2
        expected_orientation = 0

        self.assertEqual(box.x_pos_argos, 0)
        self.assertEqual(box.y_pos_argos, 0)
        self.assertEqual(box.width, expected_width)
        self.assertEqual(box.height, expected_height)
        self.assertEqual(box.x_pos, expected_real_x_pos)
        self.assertEqual(box.y_pos, expected_real_y_pos)
        self.assertEqual(box.orientation, expected_orientation)

    @mock.patch('services.communication.simulation_configuration.pathlib.Path')
    @mock.patch(
        'services.communication.simulation_configuration.shutil.copyfile')
    def test_simulation_configuration_init(self, path_mock: mock.MagicMock,
                                           shutil_mock: mock.MagicMock):
        Config.SimulationConfiguration()
        path_mock.assert_called()
        shutil_mock.assert_called()

    @mock.patch('services.communication.simulation_configuration.pathlib.Path')
    @mock.patch('services.communication.simulation_configuration.ET')
    @mock.patch(
        'services.communication.simulation_configuration.shutil.copyfile')
    def test_add_drone(
        self,
        shutil_mock: mock.MagicMock,
        et_mock: mock.MagicMock,
        path_mock: mock.MagicMock,
    ):
        config = Config.SimulationConfiguration()

        et_mock.parse = mock.MagicMock()
        et_mock.SubElement = mock.MagicMock()

        config.add_drone(
            StartingDroneData({
                'name': 'test',
                'startingXPos': 1,
                'startingYPos': 2,
                'startingOrientation': 127
            }))
        path_mock.assert_called()
        et_mock.parse.assert_called()
        et_mock.SubElement.assert_called()
        shutil_mock.assert_called()

    @mock.patch('services.communication.simulation_configuration.ET.Element')
    @mock.patch('services.communication.simulation_configuration.pathlib.Path')
    @mock.patch('services.communication.simulation_configuration.ET')
    @mock.patch(
        'services.communication.simulation_configuration.shutil.copyfile')
    def test_add_obstacles(self, shutil_mock: mock.MagicMock,
                           et_mock: mock.MagicMock, path_mock: mock.MagicMock,
                           element: mock.MagicMock):

        test = mock.MagicMock()

        test.x_pos = 2.01

        config = Config.SimulationConfiguration()
        et_mock.parse = mock.MagicMock()
        et_mock.SubElement = mock.MagicMock()
        config.add_obstacles([
            StartingDroneData({
                'name': 'test',
                'startingXPos': 1,
                'startingYPos': 2,
                'startingOrientation': 0
            })
        ])

        self.assertEqual(path_mock.call_count, 2)
        self.assertEqual(et_mock.parse.call_count, 1)
        self.assertEqual(et_mock.SubElement.call_count, 10)

    @mock.patch('services.communication.simulation_configuration.ET.Element')
    @mock.patch('services.communication.simulation_configuration.pathlib.Path')
    @mock.patch('services.communication.simulation_configuration.ET')
    @mock.patch(
        'services.communication.simulation_configuration.shutil.copyfile')
    def test_is_colliding(self, shut: mock.MagicMock, et: mock.MagicMock,
                          path: mock.MagicMock, element: mock.MagicMock):
        config = Config.SimulationConfiguration()
        box1 = Config.Box(1, 1, 2, 1, 0)
        box2 = Config.Box(1, 1, 2, 1, 0)
        box3 = Config.Box(0, 0, 0.5, 0, 0)
        self.assertTrue(
            config._SimulationConfiguration__is_colliding(box1, box2))
        self.assertFalse(
            config._SimulationConfiguration__is_colliding(box1, box3))

    @mock.patch('services.communication.simulation_configuration.pathlib.Path')
    @mock.patch('services.communication.simulation_configuration.os.system')
    @mock.patch(
        'services.communication.simulation_configuration.subprocess.call')
    @mock.patch(
        'services.communication.simulation_configuration.shutil.copyfile')
    def test_launch(self, shut: mock.MagicMock, sub: mock.MagicMock,
                    os: mock.MagicMock, path_mock: mock.MagicMock):

        os.return_value = 0
        sub.return_value = 0

        config = Config.SimulationConfiguration()
        config.launch()
        self.assertEqual(sub.call_count, 1)
        self.assertEqual(os.call_count, 2)
        path_mock.assert_called()
