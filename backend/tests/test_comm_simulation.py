import unittest
import socket

from unittest.mock import MagicMock, patch
from services.communication.simulation.comm_simulation import CommSimulation
from constants import COMMANDS

class TestCommSimulation(unittest.TestCase):

    @patch('os.path.exists', MagicMock)
    @patch('os.remove', MagicMock)
    @patch('socket.socket', MagicMock(bind=''))
    @patch('services.communication.simulation.comm_simulation.CommSimulation.attemptSocketConnection',
            MagicMock)
    def test_constructor(self):
        comm_sim = CommSimulation()
        self.assertIsInstance(comm_sim, CommSimulation)

    @patch('os.path.exists', MagicMock)
    @patch('os.remove', MagicMock)
    @patch('socket.socket', MagicMock(bind='', listen=''))
    @patch('socket.socket.listen', MagicMock)
    @patch('socket.socket.settimeout', MagicMock)
    def test_attempt_socket_connection(self):
        RETURN_VALUE = 1, 1
        socket.socket().accept.return_value = RETURN_VALUE
        comm_sim = CommSimulation()
        comm_sim.attemptSocketConnection(0)
        self.assertTrue(comm_sim.isConnected)

    @patch('os.path.exists', MagicMock)
    @patch('os.remove', MagicMock)
    @patch('socket.socket', MagicMock(bind='', listen=''))
    @patch('services.communication.simulation.comm_simulation.CommSimulation.attemptSocketConnection', MagicMock())
    def test_send_command(self):
        comm_sim = CommSimulation()
        comm_sim.send_command(COMMANDS.IDENTIFY.value)
        CommSimulation.attemptSocketConnection.assert_called()
