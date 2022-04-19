from os import access
import unittest
from unittest import mock
from services.status.access_status import get_mission_status

import services.status.mission_status as MissionStatus


class TestApplication(unittest.TestCase):

    def test_set_mission_started(self):
        MissionStatus.set_mission_started(True)
        self.assertTrue(MissionStatus.is_mission_started)
        MissionStatus.set_mission_started(False)
        self.assertFalse(MissionStatus.is_mission_started)

    def test_get_mission_started(self):
        MissionStatus.is_mission_started = False
        self.assertFalse(MissionStatus.get_mission_started())

    @mock.patch('services.status.mission_status.SocketIO', mock.MagicMock)
    @mock.patch('services.status.mission_status.update_specific_client')
    def test_client_connected(self, update_mock: mock.MagicMock):
        request = mock.Mock()
        request.sid = 'test'

        MissionStatus.client_connected(mock.MagicMock(), request)
        self.assertEqual(update_mock.call_count, 1)

    @mock.patch('services.status.mission_status.SocketIO', mock.MagicMock)
    @mock.patch('services.status.mission_status.update_all_clients')
    def test_launch_mission(self, update_mock: mock.MagicMock):
        MissionStatus.is_mission_started = False
        MissionStatus.launch_mission(mock.MagicMock())
        self.assertEqual(update_mock.call_count, 1)
        self.assertTrue(MissionStatus.is_mission_started)

    @mock.patch('services.status.mission_status.SocketIO', mock.MagicMock)
    @mock.patch('services.status.mission_status.update_all_clients')
    def test_terminate_mission(self, update_mock: mock.MagicMock):
        MissionStatus.is_mission_started = True
        MissionStatus.terminate_mission(mock.MagicMock())
        self.assertEqual(update_mock.call_count, 1)
        self.assertFalse(MissionStatus.is_mission_started)

    @mock.patch('services.status.mission_status.get_mission_status')
    @mock.patch('services.status.mission_status.SocketIO')
    def test_update_all_clients(self, socket: mock.Mock,
                                get_mission_status_mock: mock.MagicMock):

        socket.emit = mock.MagicMock(return_value=None)
        get_mission_status_mock.return_value = 'tests'

        MissionStatus.update_all_clients(socket, 'test')

        socket.emit.assert_called_once_with('update_status',
                                            'tests',
                                            namespace='/getMissionStatus',
                                            broadcast=True,
                                            include_self=False,
                                            skip_sid=['test'])
        get_mission_status_mock.assert_called_once_with()

    @mock.patch('services.status.mission_status.SocketIO')
    def test_update_p2p_gradient_value(
        self,
        socket: mock.Mock,
    ):

        MissionStatus.update_p2p_gradient_value(socket)

        socket.emit.assert_called_once_with(
            'updateP2PGradient',
            False,
            namespace='/limitedAccess',
            broadcast=True,
        )

    @mock.patch('services.status.mission_status.get_mission_status')
    @mock.patch('services.status.mission_status.SocketIO')
    def test_update_specific_client(self, socket: mock.Mock,
                                    get_mission_status_mock: mock.MagicMock):

        get_mission_status_mock.return_value = 'tests'

        socket.emit = mock.MagicMock(return_value=None)

        MissionStatus.update_specific_client(socket, 'test')

        socket.emit.assert_called_once_with('update_status',
                                            'tests',
                                            namespace='/getMissionStatus',
                                            room='test')
        get_mission_status_mock.assert_called_once_with()

    def test_get_mission_status(self):
        MissionStatus.is_mission_started = False
        self.assertFalse(MissionStatus.get_mission_status()['isMissionStarted'])
