"""This module is used to dedclare The TestCommCrazyflie
class that is used to test the CommCrazyflie class"""
import cflib
import unittest

from unittest.mock import MagicMock, patch
from services.communication.comm_crazyflie import CommCrazyflie
from services.communication.abstract_comm import AbstractComm
from constants import COMMANDS, URI
from flask_socketio import SocketIO
from cflib.crazyflie import Crazyflie
from cflib.crazyflie.syncCrazyflie import SyncCrazyflie


class TestCommCrazyflie(unittest.TestCase):
    """This class is used to test the CommeCrazyflie class"""

    @patch('cflib.crtp.init_drivers', MagicMock)
    @patch('cflib.crazyflie.Crazyflie', MagicMock)
    @patch('services.communication.comm_crazyflie.CommCrazyflie.__del__',
           MagicMock())
    def test_constructor(self):
        comm_cf = CommCrazyflie(MagicMock(), [])
        self.assertIsInstance(comm_cf, CommCrazyflie)

    @patch('cflib.crazyflie.syncCrazyflie.SyncCrazyflie.is_link_open',
           MagicMock({'cflib.crtp.init_drivers': True}))
    @patch('cflib.crazyflie.syncCrazyflie.SyncCrazyflie.close_link',
           MagicMock())
    def test_del(self):
        comm_cf = CommCrazyflie(MagicMock(), [])
        self.assertIsInstance(comm_cf, CommCrazyflie)
        comm_cf.sync_crazyflies = [SyncCrazyflie(URI[0])]
        comm_cf.__del__()
        cflib.crazyflie.syncCrazyflie.SyncCrazyflie.close_link.assert_called()

    @patch('services.communication.comm_crazyflie.CommCrazyflie.shutdown')
    def test_shutdown(self, shutdown_mock: MagicMock):
        comm_cf = CommCrazyflie(MagicMock(), [])
        comm_cf.shutdown()
        shutdown_mock.assert_called()

    @patch('cflib.crtp.init_drivers', MagicMock())
    def test_init_drivers(self):
        comm_cf = CommCrazyflie(MagicMock(), [])
        comm_cf._CommCrazyflie__init_drivers()
        cflib.crtp.init_drivers.assert_called()

    # TODO
    def test_setup_log(self):
        comm_cf = CommCrazyflie(MagicMock(), [])
        #comm_cf.crazyflies.append(Crazyflie(rw_cache='./cache'))
        comm_cf.setup_log()

    @patch('cflib.crazyflie.Crazyflie.open_link', MagicMock())
    @patch('cflib.crazyflie.appchannel.Appchannel.send_packet', MagicMock())
    @patch('cflib.crazyflie.Crazyflie.close_link', MagicMock())
    def test_send_command(self):
        comm_cf = CommCrazyflie(MagicMock(), [])
        comm_cf.crazyflies_by_id[URI[0]] = Crazyflie(rw_cache='./cache')
        comm_cf.links = ["radio://0/80/2M/E7E7E7E761"]
        print("Crazyflies by id: ", comm_cf.crazyflies_by_id)
        comm_cf.send_command(COMMANDS.IDENTIFY.value, [])
        cflib.crazyflie.appchannel.Appchannel.send_packet.assert_called()
