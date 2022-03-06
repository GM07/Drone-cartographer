"""This module is used to dedclare The TestCommCrazyflie
class that is used to test the CommCrazyflie class"""
import cflib
import unittest

from unittest.mock import MagicMock, patch
from services.communication.crazyflie.comm_crazyflie import CommCrazyflie
from constants import COMMANDS, URI


class TestCommCrazyflie(unittest.TestCase):
    """This class is used to test the CommeCrazyflie class"""
    @patch('cflib.crtp.init_drivers', MagicMock)
    @patch('cflib.crazyflie.Crazyflie', MagicMock)
    def test_constructor(self):
        comm_cf = CommCrazyflie()
        self.assertIsInstance(comm_cf, CommCrazyflie)

    @patch('cflib.crazyflie.Crazyflie.open_link', MagicMock())
    @patch('cflib.crazyflie.appchannel.Appchannel.send_packet', MagicMock())
    @patch('cflib.crazyflie.Crazyflie.close_link', MagicMock())
    def test_send_command(self):
        comm_cf = CommCrazyflie()
        comm_cf.send_command(COMMANDS.IDENTIFY.value, URI[0])
        cflib.crazyflie.Crazyflie.open_link.assert_called()
        cflib.crazyflie.appchannel.Appchannel.send_packet.assert_called()
        cflib.crazyflie.Crazyflie.close_link.assert_called()
