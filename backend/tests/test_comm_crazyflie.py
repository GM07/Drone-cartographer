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
from cflib.crazyflie.log import LogConfig

from cflib.crazyflie.syncCrazyflie import SyncCrazyflie

from services.data.starting_drone_data import StartingDroneData


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

    @patch('services.communication.comm_crazyflie.CommCrazyflie.init_comm',
           MagicMock())
    @patch('services.communication.comm_crazyflie.CommCrazyflie.__del__',
           MagicMock())
    def test_start_logs(self):
        comm_cf = CommCrazyflie(MagicMock(), [])
        comm_cf.start_logs()
        CommCrazyflie.init_comm.assert_called()

    @patch('services.communication.comm_crazyflie.CommCrazyflie.shutdown',
           MagicMock())
    @patch('services.communication.comm_crazyflie.CommCrazyflie.__del__',
           MagicMock())
    def test_stop_logs(self):
        comm_cf = CommCrazyflie(MagicMock(), [])
        comm_cf.stop_logs()
        CommCrazyflie.shutdown.assert_called()

    @patch('cflib.crtp.init_drivers', MagicMock)
    @patch('cflib.crazyflie.Crazyflie', MagicMock)
    @patch('services.communication.comm_crazyflie.CommCrazyflie.__del__',
           MagicMock())
    @patch('cflib.crazyflie.syncCrazyflie.SyncCrazyflie.open_link', MagicMock())
    @patch('cflib.crazyflie.syncCrazyflie.SyncCrazyflie.close_link',
           MagicMock())
    @patch('cflib.crazyflie.log.LogConfig.start', MagicMock())
    def test_setup_log(self):
        comm_cf = CommCrazyflie(MagicMock(), [
            StartingDroneData({
                'name': 'test',
                'startingXPos': 1,
                'startingYPos': 1,
                'startingOrientation': 0
            })
        ])
        #comm_cf.crazyflies.append(Crazyflie(rw_cache='./cache'))
        comm_cf.setup_log()
        assert len(comm_cf.log_configs) == 1
        cflib.crazyflie.log.LogConfig.start.assert_called()

    @patch('cflib.crazyflie.Crazyflie.open_link', MagicMock())
    @patch('cflib.crazyflie.appchannel.Appchannel.send_packet', MagicMock())
    @patch('cflib.crazyflie.Crazyflie.close_link', MagicMock())
    def test_send_command(self):
        comm_cf = CommCrazyflie(MagicMock(), [])
        comm_cf.crazyflies_by_id[URI[0]] = Crazyflie(rw_cache='./cache')
        comm_cf.links = ["radio://0/80/2M/E7E7E7E761"]
        print("Crazyflies by id: ", comm_cf.crazyflies_by_id)
        comm_cf.send_command(COMMANDS.IDENTIFY.value)
        cflib.crazyflie.appchannel.Appchannel.send_packet.assert_not_called()
