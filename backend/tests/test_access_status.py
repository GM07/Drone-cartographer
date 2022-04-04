import unittest
from unittest import mock

import services.status.access_status as AccessStatus


class TestApplication(unittest.TestCase):

    def test_set_mission_simulated(self):

        AccessStatus.set_mission_simulated(True)
        self.assertTrue(AccessStatus.is_mission_simulated)

        AccessStatus.set_mission_simulated(False)
        self.assertFalse(AccessStatus.is_mission_simulated)

    def test_get_mission_simulated(self):
        AccessStatus.is_mission_simulated = False
        self.assertFalse(AccessStatus.get_mission_simulated())

        AccessStatus.is_mission_simulated = True
        self.assertTrue(AccessStatus.get_mission_simulated())

    @mock.patch('services.status.access_status.update_specific_client')
    @mock.patch('services.status.access_status.update_all_clients')
    @mock.patch('services.status.access_status.SocketIO', mock.MagicMock)
    def test_set_mission_type(self, client_mock: mock.MagicMock,
                              specific_mock: mock.MagicMock):

        simulated = True
        AccessStatus.set_mission_type(mock.MagicMock(), simulated)
        client_mock.assert_called_once()

        self.assertTrue(AccessStatus.is_mission_simulated)

    @mock.patch('services.status.access_status.update_specific_client')
    @mock.patch('services.status.access_status.update_all_clients')
    @mock.patch('services.status.access_status.SocketIO', mock.MagicMock)
    def test_client_disconnected(self, client_mock: mock.MagicMock,
                                 specific_mock: mock.MagicMock):
        request = mock.Mock()
        request.sid = 'test'
        AccessStatus.controlling_client = 'test'

        self.assertTrue(
            AccessStatus.client_disconnected(mock.MagicMock(), request))
        self.assertEqual(None, AccessStatus.controlling_client)
        self.assertFalse(
            AccessStatus.client_disconnected(mock.MagicMock(), request))
        client_mock.assert_called_once()

    @mock.patch('services.status.access_status.update_specific_client')
    @mock.patch('services.status.access_status.update_all_clients')
    @mock.patch('services.status.access_status.SocketIO', mock.MagicMock)
    def test_take_control_successful(self, client_mock: mock.MagicMock,
                                     specific_mock: mock.MagicMock):
        request = mock.Mock()
        request.sid = 'test'
        AccessStatus.controlling_client = None

        self.assertTrue(AccessStatus.take_control(mock.MagicMock(), request))

        client_mock.assert_called_once()
        self.assertEqual(AccessStatus.controlling_client, request.sid)

    @mock.patch('services.status.access_status.update_specific_client')
    @mock.patch('services.status.access_status.update_all_clients')
    @mock.patch('services.status.access_status.SocketIO', mock.MagicMock)
    def test_take_control_unsuccessful(self, client_mock: mock.MagicMock,
                                       specific_mock: mock.MagicMock):
        request = mock.Mock()
        request.sid = 'test'
        AccessStatus.controlling_client = 'None'

        self.assertFalse(AccessStatus.take_control(mock.MagicMock(), request))
        client_mock.assert_not_called()
        specific_mock.assert_called_once()
        self.assertNotEqual(AccessStatus.controlling_client, request.sid)

    @mock.patch('services.status.access_status.update_specific_client')
    @mock.patch('services.status.access_status.update_all_clients')
    @mock.patch('services.status.access_status.SocketIO', mock.MagicMock)
    def test_revoke_controlling_client_unsuccessful(
            self, client_mock: mock.MagicMock, specific_mock: mock.MagicMock):

        request = mock.Mock()
        request.sid = 'test'

        AccessStatus.controlling_client = 'None'

        self.assertFalse(
            AccessStatus.revoke_controlling_client(mock.MagicMock(), request))
        specific_mock.assert_called_once()
        client_mock.assert_not_called()

    @mock.patch('services.status.access_status.update_specific_client')
    @mock.patch('services.status.access_status.update_all_clients')
    @mock.patch('services.status.access_status.SocketIO', mock.MagicMock)
    def test_revoke_controlling_client_successful(
            self, client_mock: mock.MagicMock, specific_mock: mock.MagicMock):
        request = mock.Mock()
        request.sid = 'test'

        AccessStatus.controlling_client = 'test'
        self.assertTrue(
            AccessStatus.revoke_controlling_client(mock.MagicMock(), request))
        specific_mock.assert_not_called()
        client_mock.assert_called_once()
        self.assertEqual(None, AccessStatus.controlling_client)

    @mock.patch('services.status.access_status.update_specific_client')
    @mock.patch('services.status.access_status.SocketIO')
    def test_update_all_clients(self, socket: mock.Mock,
                                specific_mock: mock.MagicMock):

        socket.emit = mock.MagicMock(return_value=None)
        AccessStatus.controlling_client = 'test'

        AccessStatus.update_all_clients(socket)

        socket.emit.assert_called_once()
        specific_mock.assert_called_once()

    @mock.patch('services.status.access_status.is_user_controlling')
    @mock.patch('services.status.access_status.get_mission_status')
    @mock.patch('services.status.access_status.SocketIO')
    def test_update_specific_client(self, socket: mock.Mock,
                                    mission_status_mock: mock.MagicMock,
                                    is_user_controlling_mock: mock.MagicMock):
        request = mock.Mock()
        request.sid = 'test'
        socket.emit = mock.MagicMock()
        AccessStatus.update_specific_client(socket, request.sid)
        self.assertEqual(mission_status_mock.call_count, 1)
        self.assertEqual(is_user_controlling_mock.call_count, 1)
        self.assertEqual(socket.emit.call_count, 1)

    def test_is_request_valid(self):
        request = mock.Mock()
        request.sid = 'test'
        AccessStatus.controlling_client = ''
        self.assertFalse(AccessStatus.is_request_valid(request))
        self.assertFalse(AccessStatus.is_request_valid(None))
        AccessStatus.controlling_client = request.sid
        self.assertTrue(AccessStatus.is_request_valid(request))

    def test_is_user_controlling(self):
        request = mock.Mock()
        request.sid = 'test'
        AccessStatus.controlling_client = ''
        self.assertFalse(AccessStatus.is_user_controlling(request.sid))
        AccessStatus.controlling_client = 'test'
        self.assertTrue(AccessStatus.is_user_controlling(request.sid))

    def test_is_someone_controlling(self):
        AccessStatus.controlling_client = None
        self.assertFalse(AccessStatus.is_someone_controlling())
        AccessStatus.controlling_client = 'test'
        self.assertTrue(AccessStatus.is_someone_controlling())
