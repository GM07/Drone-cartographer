from typing import Any, Dict
import unittest
from unittest.mock import MagicMock, patch
from constants import COMMANDS

from services.communication.abstract_comm import AbstractComm
from services.communication.comm_tasks import DRONE_STATUS_QUEUE
from services.data.drone_data import DroneData
from services.data.starting_drone_data import StartingDroneData

starting_drone_data = StartingDroneData({
    'name': 'test',
    'startingXPos': 0,
    'startingYPos': 0,
    'startingOrientation': 0
})


class AbstractCommMock(AbstractComm):

    def __init__(self, drone_list=[starting_drone_data]):
        self.socket_mock = MagicMock()
        self.drone_list = drone_list
        super().__init__(self.socket_mock, self.drone_list)
        self.send_command_mock = MagicMock()
        self.validate_name_mock = MagicMock()
        self.shutdown_mock = MagicMock()
        self.stop_logs_mock = MagicMock()
        self.start_logs_mock = MagicMock()

    def send_command(self,
                     command: COMMANDS,
                     links=...,
                     args: bytes = None) -> None:
        self.send_command_mock(command, links, args)

    def validate_name(self, name: str) -> str:
        self.validate_name_mock(name)

    def shutdown(self):
        return self.shutdown_mock()

    def stop_logs(self):
        self.stop_logs_mock()

    def start_logs(self):
        self.start_logs_mock()


class TestAbstractComm(unittest.TestCase):

    @patch('services.communication.abstract_comm.AbstractComm.set_drone')
    def test_init(self, set_drone_mock: MagicMock):
        comm = AbstractCommMock()
        set_drone_mock.assert_called()

    def test_init_drone_data(self):
        comm = AbstractCommMock()
        comm._init_drone_data()
        assert len(comm.drone_data_list) == len(comm.drone_list)

    @patch('services.communication.abstract_comm.AbstractComm._init_drone_data')
    def test_set_drone(self, init_mock: MagicMock):
        comm = AbstractCommMock()
        comm.set_drone([starting_drone_data, starting_drone_data])
        assert comm.drone_list == [starting_drone_data, starting_drone_data]
        init_mock.assert_called()

    def test_get_drones(self):
        comm = AbstractCommMock()
        assert comm.get_drones() == [starting_drone_data]

    def test_send_log(self):
        comm = AbstractCommMock()
        comm.send_log('test')
        assert len(comm.logs) == 1

    def test_send_drone_status(self):
        comm = AbstractCommMock()
        comm.send_drone_status({'test': 1})
        assert DRONE_STATUS_QUEUE.peek() == {'test': 1}

    def test_set_drone_data(self):
        comm = AbstractCommMock()
        comm._init_drone_data()
        mock_data = DroneData('test', bytes(), empty=True)
        comm.set_drone_data(mock_data)
        assert comm.drone_data_list[mock_data.name] == mock_data

    def test_get_full_drone_data(self):
        comm = AbstractCommMock()
        returned_dict = comm.get_full_drone_data()
        assert (returned_dict[-1].get('name') == 'test' and
                returned_dict[-1].get('startingXPos') == 0 and
                returned_dict[-1].get('startingYPos') == 0 and
                returned_dict[-1].get('startingOrientation') == 0)

    def test_send_command(self):
        comm = AbstractCommMock()
        comm.send_command(COMMANDS.IDENTIFY, ['a'], 'test'.encode('UTF-8'))
        comm.send_command_mock.assert_called_with(COMMANDS.IDENTIFY, ['a'],
                                                  'test'.encode('UTF-8'))

    def test_validate_name(self):
        comm = AbstractCommMock()
        comm.validate_name('test')
        comm.validate_name_mock.assert_called_with('test')

    def test_shutdown(self):
        comm = AbstractCommMock()
        comm.shutdown()
        comm.shutdown_mock.assert_called()

    def test_stop_logs(self):
        comm = AbstractCommMock()
        comm.stop_logs()
        comm.stop_logs_mock.assert_called()

    def test_start_logs(self):
        comm = AbstractCommMock()
        comm.start_logs()
        comm.start_logs_mock.assert_called()