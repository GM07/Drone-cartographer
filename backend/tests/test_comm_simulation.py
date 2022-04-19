"""This module is used to test the CommSimulation class"""
from dataclasses import dataclass
import queue
import threading
import time
from typing import Any
from errno import EINVAL
import unittest
import socket
from unittest import mock

from unittest.mock import MagicMock, PropertyMock, patch
from xml.dom import InvalidAccessErr
from services.communication.comm_simulation import CommSimulation
from constants import COMMANDS
from services.data.starting_drone_data import StartingDroneData


class fakeSocket:

    def shutdown(self, *args):
        return

    def setblocking(self, *args):
        return

    def settimeout(self, *args):
        return

    def accept(self, *args):
        return


class DroneSocketStub:
    server: fakeSocket
    conn: Any

    def __init__(self, server, conn):
        self.server = server
        self.conn = conn


class TestCommSimulation(unittest.TestCase):
    """This class defines the tests of the CommSImulation class"""

    @patch('os.path.exists', MagicMock())
    @patch('os.remove', MagicMock())
    @patch('socket.socket', MagicMock(bind=''))
    @patch('threading.Thread', MagicMock())
    def test_constructor(self):
        comm_sim = CommSimulation(MagicMock(), [
            StartingDroneData({
                'name': 'test',
                'startingXPos': 0,
                'startingYPos': 0,
                'startingOrientation': 0
            })
        ])
        self.assertIsInstance(comm_sim, CommSimulation)

    @patch('socket.socket', MagicMock(bind=''))
    @patch('threading.Thread', MagicMock())
    @patch('services.data.simulation_wrappers.DroneSimulationSocket',
           MagicMock())
    def test_shutdown(self):
        test = fakeSocket()
        comm_sim = CommSimulation(MagicMock(), [])
        comm_sim.data_servers[test] = fakeSocket()
        comm_sim.command_servers = dict(
            test=DroneSocketStub(fakeSocket(), fakeSocket()))
        comm_sim._CommSimulation__COMMANDS_QUEUE.put(tuple(['test', 'command']))
        comm_sim.shutdown()
        assert comm_sim.thread_active is False

    @patch('socket.socket', MagicMock(bind=''))
    @patch('threading.Thread', MagicMock())
    @patch('services.data.simulation_wrappers.DroneSimulationSocket',
           MagicMock())
    def test_shutdown_unsuccessful(self):
        comm_sim = CommSimulation(MagicMock(), [])
        comm_sim.shutdown()
        assert comm_sim.thread_active is False

    @patch('socket.socket', MagicMock(bind=''))
    @patch('threading.Thread', MagicMock())
    @patch('services.communication.comm_simulation.CommSimulation', MagicMock())
    @patch('queue.Queue.put_nowait', MagicMock())
    def test_send_command(self):
        comm_sim = CommSimulation(MagicMock(), [])

        comm_sim.send_command(COMMANDS.IDENTIFY, ['test'])
        comm_sim._CommSimulation__COMMANDS_QUEUE.put_nowait.assert_called()

    @patch('socket.socket', MagicMock(bind=''))
    @patch('threading.Thread', MagicMock())
    @patch('services.communication.comm_simulation.CommSimulation', MagicMock())
    @patch('queue.Queue.put_nowait', MagicMock())
    def test_send_command_unsuccessful(self):
        comm_sim = CommSimulation(MagicMock(), [])
        comm_sim._CommSimulation__COMMANDS_QUEUE.put_nowait = mock.Mock(
            side_effect=queue.Full)
        comm_sim.send_command(COMMANDS.IDENTIFY, ['test'])
        comm_sim._CommSimulation__COMMANDS_QUEUE.put_nowait.assert_called()

    @patch('os.path.exists', return_value=True)
    @patch('os.remove')
    @patch('socket.socket', MagicMock(bind=''))
    def test_init_server_bind(self, remove: MagicMock, exists: MagicMock):
        comm_sim = CommSimulation(MagicMock(), [])
        comm_sim._CommSimulation__init_server_bind('test')
        exists.assert_called()
        remove.assert_called()

    @patch('services.communication.comm_simulation.CommSimulation', MagicMock())
    def test_thread_attempt_socket_connections(self):
        drone_socket_mock = MagicMock()
        methods = {
            'values.return_value': [drone_socket_mock] * 2,
        }
        sockets = MagicMock()
        server = MagicMock()
        drone_socket_mock.server = server
        drone_socket_mock.conn = 'test'
        accept_mock = MagicMock()
        server.accept = accept_mock()
        sockets.configure_mock(**methods)
        CommSimulation._CommSimulation__thread_attempt_socket_connection(
            sockets)
        accept_mock.assert_called()

    @patch('services.communication.comm_simulation.CommSimulation', MagicMock())
    def test_thread_attempt_data_socket_connection(self):
        drone_socket_mock = MagicMock()
        methods = {
            'items.return_value': [drone_socket_mock] * 2,
        }
        sockets = MagicMock()
        server = MagicMock()
        drone_socket_mock.server = server
        drone_socket_mock.conn = 'test'

        accept_mock = MagicMock()
        server.accept = accept_mock()
        sockets.configure_mock(**methods)
        CommSimulation._CommSimulation__thread_attempt_socket_connection(
            sockets)
        accept_mock.assert_called()

    @mock.patch(
        'services.communication.comm_simulation.CommSimulation._CommSimulation__thread_attempt_data_socket_connection'
    )
    def test_receive_data_tasks_wrapper(self, connect_mock: MagicMock):
        comm = CommSimulation({})
        comm.nb_connections = 1
        comm.thread_active = mock.Mock(side_effect=[True, True, False])
        th = threading.Thread(
            target=comm._CommSimulation__receive_data_tasks_wrapper, args=())
        th.start()
        time.sleep(0.5)
        comm.thread_active = False

        th.join()

        connect_mock.assert_called()

    @mock.patch(
        'services.communication.comm_simulation.CommSimulation._CommSimulation__thread_attempt_data_socket_connection'
    )
    def test_receive_data_tasks_wrapper_unsucessful(self,
                                                    connect_mock: MagicMock):
        comm = CommSimulation({})
        comm.nb_connections = 0
        comm.thread_active = mock.Mock(side_effect=[True, True, False])

        th = threading.Thread(
            target=comm._CommSimulation__receive_data_tasks_wrapper, args=())
        th.start()
        time.sleep(0.5)
        comm.thread_active = False

        th.join()

        connect_mock.assert_not_called()

    @mock.patch(
        'services.communication.comm_simulation.CommSimulation._CommSimulation__thread_send_command',
        mock.MagicMock)
    @mock.patch(
        'services.communication.comm_simulation.CommSimulation._CommSimulation__thread_attempt_socket_connection'
    )
    def test_send_command_tasks_wrapper(self, connect_mock: MagicMock):
        comm = CommSimulation({})
        comm.nb_connections = 1
        comm.thread_active = mock.Mock(side_effect=[True, True, False])

        th = threading.Thread(
            target=comm._CommSimulation__send_command_tasks_wrapper, args=())
        th.start()
        time.sleep(0.5)
        comm.thread_active = False

        th.join()

        connect_mock.assert_called()

    @mock.patch(
        'services.communication.comm_simulation.CommSimulation._CommSimulation__thread_send_command',
        mock.MagicMock)
    @mock.patch(
        'services.communication.comm_simulation.CommSimulation._CommSimulation__thread_attempt_socket_connection'
    )
    def test_send_command_tasks_wrapper_unsuccessful(self,
                                                     connect_mock: MagicMock):
        comm = CommSimulation({})
        comm.nb_connections = 0
        comm.thread_active = mock.Mock(side_effect=[True, True, False])

        th = threading.Thread(
            target=comm._CommSimulation__send_command_tasks_wrapper, args=())
        th.start()
        time.sleep(0.5)
        comm.thread_active = False

        th.join()

        connect_mock.assert_not_called()

    def test_thread_attempt_socket_connection(self):
        sockets = {
            'test': DroneSocketStub(fakeSocket(), None),
        }
        comm = CommSimulation({})

        fakeSocket.accept = mock.MagicMock(return_value=tuple([1, 2]))
        fakeSocket.setblocking = mock.MagicMock()
        fakeSocket.settimeout = mock.MagicMock()
        comm._CommSimulation__thread_attempt_socket_connection(sockets)
        fakeSocket.accept.assert_called_once()
        fakeSocket.setblocking.assert_called_once()
        fakeSocket.settimeout.assert_called_once()

    def test_thread_attempt_socket_connection_unsucessful_caught(self):
        sockets = {
            'test': DroneSocketStub(fakeSocket(), None),
        }
        comm = CommSimulation({})
        err = socket.error(socket.EBADF)
        err.errno = socket.EBADF

        fakeSocket.accept = mock.MagicMock(return_value=tuple([1, 2]))
        fakeSocket.setblocking = mock.Mock(side_effect=err)
        fakeSocket.settimeout = mock.MagicMock()
        try:
            comm._CommSimulation__thread_attempt_socket_connection(sockets)
        except socket.error as err:
            raise err

        fakeSocket.accept.assert_not_called()
        fakeSocket.setblocking.assert_called_once()
        fakeSocket.settimeout.assert_not_called()

    def test_thread_attempt_socket_connection_unsucessful(self):
        sockets = {
            'test': DroneSocketStub(fakeSocket(), None),
        }
        comm = CommSimulation({})
        err = socket.error(socket.EBADF)
        err.errno = 11

        fakeSocket.accept = mock.MagicMock(return_value=tuple([1, 2]))
        fakeSocket.setblocking = mock.Mock(side_effect=err)
        fakeSocket.settimeout = mock.MagicMock()
        try:
            comm._CommSimulation__thread_attempt_socket_connection(sockets)
        except socket.error as err:
            self.assertEqual(err.errno, 11)

        fakeSocket.accept.assert_not_called()
        fakeSocket.setblocking.assert_called_once()
        fakeSocket.settimeout.assert_not_called()

    def test_thread_attempt_data_socket_connection_unsucessful(self):

        index = fakeSocket()

        sockets = {index: None}

        comm = CommSimulation({})
        err = socket.error(socket.EBADF)
        err.errno = 0

        fakeSocket.accept = mock.MagicMock(return_value=tuple([1, 2]))
        fakeSocket.setblocking = mock.Mock(side_effect=err)
        fakeSocket.settimeout = mock.MagicMock()
        try:
            comm._CommSimulation__thread_attempt_data_socket_connection(sockets)
        except socket.error as err:
            self.assertEqual(err.errno, 0)

        fakeSocket.accept.assert_not_called()
        fakeSocket.setblocking.assert_called_once()
        fakeSocket.settimeout.assert_not_called()

    def test_thread_attempt_data_socket_connection_unsucessful_caught(self):

        index = fakeSocket()

        sockets = {index: None}

        comm = CommSimulation({})
        err = socket.error(socket.EBADF)
        err.errno = socket.EBADF

        fakeSocket.accept = mock.MagicMock(return_value=tuple([1, 2]))
        fakeSocket.setblocking = mock.Mock(side_effect=err)
        fakeSocket.settimeout = mock.MagicMock()
        try:
            comm._CommSimulation__thread_attempt_data_socket_connection(sockets)
        except socket.error as err:
            self.assertEqual(err.errno, 11)

        fakeSocket.accept.assert_not_called()
        fakeSocket.setblocking.assert_called_once()
        fakeSocket.settimeout.assert_not_called()

    def test_thread_attempt_data_socket_connection_succeseful(self):

        index = fakeSocket()

        sockets = {index: None}

        comm = CommSimulation({})

        fakeSocket.accept = mock.MagicMock(return_value=tuple([1, 2]))
        fakeSocket.setblocking = mock.Mock()
        fakeSocket.settimeout = mock.MagicMock()
        try:
            comm._CommSimulation__thread_attempt_data_socket_connection(sockets)
        except socket.error as err:
            self.assertEqual(err.errno, 11)

        fakeSocket.accept.assert_called_once()
        fakeSocket.setblocking.assert_called_once()
        fakeSocket.settimeout.assert_called_once()
