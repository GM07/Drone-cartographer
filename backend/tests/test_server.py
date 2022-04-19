"""Test module for the server.py file """
from math import fabs
from typing import Dict
from typing_extensions import Literal
import unittest
from unittest import mock

from flask import Flask, session, request, json as flask_json
from flask_socketio import SocketIO, send, emit, join_room, leave_room, \
    Namespace, disconnect
from constants import COMMANDS

import server
from services.data.starting_drone_data import StartingDroneData


class StartingDroneDataStub:

    def __init__(self, name):
        self.name = name
        return None


class TestApplication(unittest.TestCase):

    class CrazyflieStub:

        def __init__(self, socket_io, drone_list):
            return None

    @mock.patch('server.AccessStatus.update_specific_client', mock.MagicMock)
    @mock.patch('server.AccessStatus.is_request_valid')
    def test_identity_drone(
        self,
        is_request_mock: mock.MagicMock,
    ):

        is_request_mock.return_value = True
        client = server.SOCKETIO.test_client(server.APP,
                                             namespace='/limitedAccess')

        server.COMM.send_command = mock.MagicMock()
        self.assertTrue(client.is_connected('/limitedAccess'))

        client.emit('identify_drone', {'a': 'b'}, namespace='/limitedAccess')

        self.assertTrue(client.is_connected('/limitedAccess'))

        server.COMM.send_command.assert_called_once()
        is_request_mock.assert_called_once()

    @mock.patch('server.AccessStatus.update_specific_client', mock.MagicMock)
    @mock.patch('server.AccessStatus.is_request_valid')
    def test_identity_drone_unsuccessful(
        self,
        is_request_mock: mock.MagicMock,
    ):

        is_request_mock.return_value = False

        client = server.SOCKETIO.test_client(server.APP,
                                             namespace='/limitedAccess')

        server.COMM.send_command = mock.MagicMock()
        self.assertTrue(client.is_connected('/limitedAccess'))

        client.emit('identify_drone', {'a': 'b'}, namespace='/limitedAccess')

        self.assertTrue(client.is_connected('/limitedAccess'))

        server.COMM.send_command.assert_not_called()
        is_request_mock.assert_called_once()

    @mock.patch('server.FileHelper.update_files')
    def test_set_files(self, update_file_mock: mock.MagicMock):
        response = server.APP.test_client().post('/setFiles',
                                                 json={
                                                     'keys': 'test',
                                                     'values': 'otherTest'
                                                 })
        self.assertEqual(response.json, {'output': 'File updated'})
        update_file_mock.assert_called_once_with('test', 'otherTest')

    @mock.patch('server.FileHelper.get_files')
    @mock.patch('server.FileHelper.get_files_content')
    def test_get_files(
        self,
        get_content_mock: mock.MagicMock,
        get_files_mock: mock.MagicMock,
    ):

        get_content_mock.return_value = {
            '/workspaces/INF3995-106/tests': 'value'
        }

        response = server.APP.test_client().get('/getFiles')
        self.assertEqual(response.json, {
            'keys': ['tests'],
            'values': ['value']
        })
        get_content_mock.assert_called_once()
        get_files_mock.assert_called_once()

    @mock.patch('server.AbstractComm.get_full_drone_data',
                mock.MagicMock(return_value='test'))
    @mock.patch('server.AccessStatus.update_specific_client', mock.MagicMock)
    @mock.patch('server.RECOMPILE_EMBEDDED')
    @mock.patch('server.RECOMPILE_SIMULATION')
    def test_recompile(self, recompile_sim: mock.MagicMock,
                       recompile_embedded: mock.MagicMock):
        client = server.SOCKETIO.test_client(server.APP,
                                             namespace='/limitedAccess')
        self.assertTrue(client.is_connected('/limitedAccess'))
        client.emit('recompile', namespace='/limitedAccess')
        recompile_sim.start.assert_called_once()
        recompile_embedded.start.assert_called_once()

    @mock.patch('server.AbstractComm.get_full_drone_data',
                mock.MagicMock(return_value='test'))
    @mock.patch('server.AccessStatus.update_specific_client', mock.MagicMock)
    @mock.patch('server.AccessStatus.is_request_valid')
    @mock.patch('server.AccessStatus.get_mission_simulated')
    @mock.patch('server.COMM.get_drones')
    @mock.patch('server.COMM.start_logs')
    @mock.patch('server.COMM.stop_logs')
    @mock.patch('server.FLASH_ALL_DRONES.change_command')
    @mock.patch('server.FLASH_ALL_DRONES.start')
    def test_flash_successful(
        self,
        flash_start_mock: mock.MagicMock,
        change_command_mock: mock.MagicMock,
        stop_logs_mock: mock.MagicMock,
        start_logs_mock: mock.MagicMock,
        get_drones_mock: mock.MagicMock,
        get_mission_simulated_mock: mock.MagicMock,
        is_request_valid_mock: mock.MagicMock,
    ):
        is_request_valid_mock.return_value = True
        get_mission_simulated_mock.return_value = False
        get_drones_mock.return_value = [StartingDroneDataStub('test')]
        client = server.SOCKETIO.test_client(server.APP,
                                             namespace='/limitedAccess')
        self.assertTrue(client.is_connected('/limitedAccess'))
        client.emit('flash', namespace='/limitedAccess')
        is_request_valid_mock.assert_called_once()
        get_mission_simulated_mock.assert_called_once()
        change_command_mock.assert_called_once_with(
            "docker exec embedded sh -c 'cd /embedded-firmware || cd workspaces/INF3995-106/embedded/embedded-firmware && make cload radio=test && sleep 1'"
        )
        flash_start_mock.assert_called_once()
        stop_logs_mock.assert_called_once()

    @mock.patch('server.AbstractComm.get_full_drone_data',
                mock.MagicMock(return_value='test'))
    @mock.patch('server.AccessStatus.update_specific_client', mock.MagicMock)
    @mock.patch('server.AccessStatus.is_request_valid')
    @mock.patch('server.AccessStatus.get_mission_simulated')
    @mock.patch('server.COMM.get_drones')
    @mock.patch('server.COMM.start_logs')
    @mock.patch('server.COMM.stop_logs')
    @mock.patch('server.FLASH_ALL_DRONES.change_command')
    @mock.patch('server.FLASH_ALL_DRONES.start')
    def test_flash_request_not_valid(
        self,
        flash_start_mock: mock.MagicMock,
        change_command_mock: mock.MagicMock,
        stop_logs_mock: mock.MagicMock,
        start_logs_mock: mock.MagicMock,
        get_drones_mock: mock.MagicMock,
        get_mission_simulated_mock: mock.MagicMock,
        is_request_valid_mock: mock.MagicMock,
    ):
        is_request_valid_mock.return_value = False
        get_mission_simulated_mock.return_value = False
        get_drones_mock.return_value = [StartingDroneDataStub('test')]
        client = server.SOCKETIO.test_client(server.APP,
                                             namespace='/limitedAccess')
        self.assertTrue(client.is_connected('/limitedAccess'))
        client.emit('flash', namespace='/limitedAccess')
        is_request_valid_mock.assert_called_once()
        get_mission_simulated_mock.assert_not_called()
        change_command_mock.assert_not_called()
        flash_start_mock.assert_not_called()
        stop_logs_mock.assert_not_called()

    @mock.patch('server.AbstractComm.get_full_drone_data',
                mock.MagicMock(return_value='test'))
    @mock.patch('server.AccessStatus.update_specific_client', mock.MagicMock)
    @mock.patch('server.AccessStatus.is_request_valid')
    @mock.patch('server.AccessStatus.get_mission_simulated')
    @mock.patch('server.COMM.get_drones')
    @mock.patch('server.COMM.start_logs')
    @mock.patch('server.COMM.stop_logs')
    @mock.patch('server.FLASH_ALL_DRONES.change_command')
    @mock.patch('server.FLASH_ALL_DRONES.start')
    def test_flash_mission_simulated(
        self,
        flash_start_mock: mock.MagicMock,
        change_command_mock: mock.MagicMock,
        stop_logs_mock: mock.MagicMock,
        start_logs_mock: mock.MagicMock,
        get_drones_mock: mock.MagicMock,
        get_mission_simulated_mock: mock.MagicMock,
        is_request_valid_mock: mock.MagicMock,
    ):
        is_request_valid_mock.return_value = True
        get_mission_simulated_mock.return_value = True
        get_drones_mock.return_value = [StartingDroneDataStub('test')]
        client = server.SOCKETIO.test_client(server.APP,
                                             namespace='/limitedAccess')
        self.assertTrue(client.is_connected('/limitedAccess'))
        client.emit('flash', namespace='/limitedAccess')
        is_request_valid_mock.assert_not_called
        get_mission_simulated_mock.assert_called_once()
        change_command_mock.assert_not_called()
        flash_start_mock.assert_not_called()
        stop_logs_mock.assert_not_called()

    @mock.patch('server.AccessStatus.update_specific_client', mock.MagicMock)
    @mock.patch('server.SimulationConfiguration.launch')
    @mock.patch('server.SimulationConfiguration.add_obstacles')
    @mock.patch('server.SimulationConfiguration.add_drone')
    @mock.patch('server.CommCrazyflie.shutdown')
    @mock.patch('server.AbstractComm.get_full_drone_data')
    @mock.patch('server.MissionStatus.get_mission_started')
    @mock.patch('server.AccessStatus.is_request_valid')
    def test_launch_unsuccessful(self, is_request_mock: mock.MagicMock,
                                 get_mission_started: mock.MagicMock,
                                 get_drones_mock: mock.MagicMock,
                                 shutdown_mock: mock.MagicMock,
                                 add_drone_mock: mock.MagicMock,
                                 add_obstacles: mock.MagicMock,
                                 launch_mock: mock.MagicMock):

        get_drones_mock.return_value = True
        is_request_mock.return_value = False
        get_mission_started.return_value = False
        client = server.SOCKETIO.test_client(server.APP,
                                             namespace='/limitedAccess')

        server.COMM.send_command = mock.MagicMock()
        self.assertTrue(client.is_connected('/limitedAccess'))

        client.emit('launch', {'a': 'b'}, namespace='/limitedAccess')

        get_drones_mock.assert_called_once()
        shutdown_mock.assert_not_called()
        add_drone_mock.assert_not_called()
        add_obstacles.assert_not_called()
        launch_mock.assert_not_called()

    @mock.patch('server.CommSimulation.__init__',
                mock.MagicMock(return_value=None))
    @mock.patch('server.CommSimulation.send_command',
                mock.MagicMock(return_value=None))
    @mock.patch('server.CommSimulation.__init__',
                mock.MagicMock(return_value=None))
    @mock.patch('server.AccessStatus.update_specific_client', mock.MagicMock)
    @mock.patch('server.SimulationConfiguration.__init__',
                mock.MagicMock(return_value=None))
    @mock.patch('server.SimulationConfiguration.launch')
    @mock.patch('server.SimulationConfiguration.add_obstacles')
    @mock.patch('server.SimulationConfiguration.add_drone')
    @mock.patch('server.CommCrazyflie.shutdown')
    @mock.patch('server.AbstractComm.get_drones')
    @mock.patch('server.MissionStatus.get_mission_started')
    @mock.patch('server.AccessStatus.is_request_valid')
    def test_launch_successful(self, is_request_mock: mock.MagicMock,
                               get_mission_started: mock.MagicMock,
                               get_drones_mock: mock.MagicMock,
                               shutdown_mock: mock.MagicMock,
                               add_drone_mock: mock.MagicMock,
                               add_obstacles: mock.MagicMock,
                               launch_mock: mock.MagicMock):
        get_drones_mock.return_value = [
            StartingDroneData({
                'name': 'test',
                'startingOrientation': 0,
                'startingXPos': 0,
                'startingYPos': 0
            })
        ]
        is_request_mock.return_value = True
        get_mission_started.return_value = False
        client = server.SOCKETIO.test_client(server.APP,
                                             namespace='/limitedAccess')
        self.assertTrue(client.is_connected('/limitedAccess'))
        client.emit('launch', True, namespace='/limitedAccess')
        self.assertEqual(get_drones_mock.call_count, 1)
        self.assertEqual(shutdown_mock.call_count, 2)
        add_drone_mock.assert_called_once()
        add_obstacles.assert_called_once()
        launch_mock.assert_called_once()

    @mock.patch('server.COMM.get_full_drone_data',
                mock.MagicMock(return_value=None))
    @mock.patch('server.SimulationConfiguration.__init__',
                mock.MagicMock(return_value=None))
    @mock.patch('server.CommSimulation.send_command',
                mock.MagicMock(return_value=None))
    @mock.patch('server.CommSimulation.__init__',
                mock.MagicMock(return_value=None))
    @mock.patch('server.CommCrazyflie.send_command',
                mock.MagicMock(return_value=None))
    @mock.patch('server.CommCrazyflie.__init__',
                mock.MagicMock(return_value=None))
    @mock.patch('server.AccessStatus.update_specific_client', mock.MagicMock)
    @mock.patch('server.MissionStatus.launch_mission')
    @mock.patch('server.SimulationConfiguration.launch')
    @mock.patch('server.SimulationConfiguration.add_obstacles')
    @mock.patch('server.SimulationConfiguration.add_drone')
    @mock.patch('server.CommSimulation.shutdown')
    @mock.patch('server.CommSimulation.get_drones')
    @mock.patch('server.MissionStatus.get_mission_started')
    @mock.patch('server.AccessStatus.is_request_valid')
    def test_launch_successful_not_simulated(
            self, is_request_mock: mock.MagicMock,
            get_mission_started: mock.MagicMock,
            get_drones_mock: mock.MagicMock, shutdown_mock: mock.MagicMock,
            add_drone_mock: mock.MagicMock, add_obstacles: mock.MagicMock,
            launch_mock: mock.MagicMock, launch_mission_mock: mock.MagicMock):

        get_drones_mock.return_value = [
            StartingDroneData({
                'name': 'test',
                'startingOrientation': 0,
                'startingXPos': 0,
                'startingYPos': 0
            })
        ]
        is_request_mock.return_value = True
        get_mission_started.return_value = False
        client = server.SOCKETIO.test_client(server.APP,
                                             namespace='/limitedAccess')

        server.COMM.send_command = mock.MagicMock()
        self.assertTrue(client.is_connected('/limitedAccess'))

        client.emit('launch', False, namespace='/limitedAccess')

        self.assertEqual(get_drones_mock.call_count, 1)
        self.assertEqual(shutdown_mock.call_count, 1)
        add_drone_mock.assert_not_called()
        add_obstacles.assert_not_called()
        launch_mock.assert_not_called()
        launch_mission_mock.assert_called_once()

    @mock.patch('server.CommCrazyflie', CrazyflieStub)
    @mock.patch('server.StartingDroneData', StartingDroneDataStub)
    @mock.patch('server.COMM.get_full_drone_data',
                mock.MagicMock(return_value='test'))
    @mock.patch('server.CommCrazyflie.shutdown',
                mock.MagicMock(return_value=None))
    @mock.patch('server.AccessStatus.update_specific_client', mock.MagicMock)
    @mock.patch('server.COMM')
    @mock.patch('server.CommCrazyflie.set_drone')
    def test_set_drone_physical(self, set_drone_mock: mock.MagicMock,
                                comm_mock: mock.MagicMock):
        comm_mock.set_drone = mock.MagicMock()
        comm_mock.get_drones = mock.MagicMock(return_value='test')
        comm_mock.shutdown = mock.MagicMock()

        client = server.SOCKETIO.test_client(server.APP,
                                             namespace='/limitedAccess')
        self.assertTrue(client.is_connected('/limitedAccess'))
        client.get_received('/limitedAccess')

        client.emit('set_drone', [{
            'name': 'test'
        }],
                    False,
                    namespace='/limitedAccess')

        self.assertEqual(client.get_received('/limitedAccess'), [{
            'name': 'droneList',
            'args': ['test'],
            'namespace': '/limitedAccess'
        }])
        comm_mock.shutdown.assert_called_once()

    @mock.patch('server.StartingDroneData', StartingDroneDataStub)
    @mock.patch('server.COMM.get_full_drone_data',
                mock.MagicMock(return_value='test'))
    @mock.patch('server.CommCrazyflie.__del__',
                mock.MagicMock(return_value=None))
    @mock.patch('server.CommCrazyflie.__init__',
                mock.MagicMock(return_value=None))
    @mock.patch('server.AccessStatus.update_specific_client', mock.MagicMock)
    @mock.patch('server.COMM')
    @mock.patch('server.CommCrazyflie.set_drone')
    def test_set_drone_simulated(self, set_drone_mock: mock.MagicMock,
                                 comm_mock: mock.MagicMock):
        comm_mock.set_drone = mock.MagicMock()
        comm_mock.get_drones = mock.MagicMock(return_value='test')
        comm_mock.shutdown = mock.MagicMock()

        client = server.SOCKETIO.test_client(server.APP,
                                             namespace='/limitedAccess')
        self.assertTrue(client.is_connected('/limitedAccess'))
        client.get_received('/limitedAccess')

        client.emit('set_drone', [{
            'name': 'test'
        }],
                    True,
                    namespace='/limitedAccess')

        self.assertEqual(client.get_received('/limitedAccess'), [{
            'name': 'droneList',
            'args': ['test'],
            'namespace': '/limitedAccess'
        }])
        comm_mock.shutdown.assert_not_called()

    @mock.patch('server.COMM.get_full_drone_data',
                mock.MagicMock(return_value=None))
    @mock.patch('server.CommSimulation.__init__')
    @mock.patch('server.AccessStatus.update_specific_client', mock.MagicMock)
    @mock.patch('server.CommCrazyflie.__init__')
    @mock.patch('server.AccessStatus.set_mission_type')
    @mock.patch('server.COMM')
    def test_set_mission_type_simulated(self, comm_mock: mock.MagicMock,
                                        set_mission_type: mock.MagicMock,
                                        crazyflie_mock: mock.MagicMock,
                                        simulated_mock: mock.MagicMock):

        simulated_mock.return_value = None
        crazyflie_mock.return_value = None
        comm_mock.get_drones = mock.MagicMock(return_value='test')
        comm_mock.shutdown = mock.MagicMock()
        client = server.SOCKETIO.test_client(server.APP,
                                             namespace='/limitedAccess')
        client.emit('set_mission_type', True, namespace='/limitedAccess')
        set_mission_type.assert_called()
        crazyflie_mock.assert_not_called()
        simulated_mock.assert_called()

    @mock.patch('server.CommCrazyflie', CrazyflieStub)
    @mock.patch('server.COMM.get_full_drone_data',
                mock.MagicMock(return_value=None))
    @mock.patch('server.CommSimulation.__init__')
    @mock.patch('server.AccessStatus.update_specific_client', mock.MagicMock)
    @mock.patch('server.AccessStatus.set_mission_type')
    @mock.patch('server.COMM')
    def test_set_mission_type_physical(self, comm_mock: mock.MagicMock,
                                       set_mission_type: mock.MagicMock,
                                       simulated_mock: mock.MagicMock):

        simulated_mock.return_value = None

        comm_mock.get_drones = mock.MagicMock(return_value=[{'test'}])
        comm_mock.shutdown = mock.MagicMock()
        client = server.SOCKETIO.test_client(server.APP,
                                             namespace='/limitedAccess')
        client.emit('set_mission_type', False, namespace='/limitedAccess')
        set_mission_type.assert_called()
        simulated_mock.assert_not_called()
        client.disconnect('/limitedAccess')

    @mock.patch('server.COMM.get_full_drone_data',
                mock.MagicMock(return_value='test'))
    @mock.patch('server.COMM.mission_manager')
    @mock.patch('server.AccessStatus.update_specific_client', mock.MagicMock)
    @mock.patch('server.MissionStatus.terminate_mission')
    @mock.patch('server.MissionStatus.get_mission_started')
    @mock.patch('server.AccessStatus.is_request_valid')
    def test_on_terminate_limited_access_successful(
            self, is_request_valid_mock: mock.MagicMock,
            get_mission_started_mock: mock.MagicMock,
            terminate_mission_mock: mock.MagicMock,
            mission_manager_mock: mock.Mock):

        mission_manager_mock.current_mission.map = mock.Mock()
        mission_manager_mock.end_current_mission = mock.MagicMock()

        server.COMM.send_command = mock.MagicMock()
        is_request_valid_mock.return_value = True
        get_mission_started_mock.return_value = True
        client = server.SOCKETIO.test_client(server.APP,
                                             namespace='/limitedAccess')

        self.assertTrue(client.is_connected('/limitedAccess'))

        client.emit('terminate', '', namespace='/limitedAccess')
        terminate_mission_mock.assert_called_once()
        server.COMM.send_command.assert_called_once()
        self.assertEqual(mission_manager_mock.current_mission.map, '')
        mission_manager_mock.end_current_mission.assert_called_once()

    @mock.patch('server.COMM.get_full_drone_data',
                mock.MagicMock(return_value='test'))
    @mock.patch('server.AccessStatus.update_specific_client', mock.MagicMock)
    @mock.patch('server.MissionStatus.terminate_mission')
    @mock.patch('server.MissionStatus.get_mission_started')
    @mock.patch('server.AccessStatus.is_request_valid')
    def test_on_terminate_limited_access_unsuccessful(
            self, is_request_valid_mock: mock.MagicMock,
            get_mission_started_mock: mock.MagicMock,
            terminate_mission_mock: mock.MagicMock):

        server.COMM.send_command = mock.MagicMock()
        is_request_valid_mock.return_value = False
        get_mission_started_mock.return_value = True
        client = server.SOCKETIO.test_client(server.APP,
                                             namespace='/limitedAccess')

        self.assertTrue(client.is_connected('/limitedAccess'))

        client.emit('terminate', '', namespace='/limitedAccess')
        terminate_mission_mock.assert_not_called()
        server.COMM.send_command.assert_not_called()

    @mock.patch('server.MissionStatus.get_mission_started')
    @mock.patch('server.AccessStatus.is_request_valid')
    @mock.patch('server.COMM.get_full_drone_data',
                mock.MagicMock(return_value='test'))
    def test_return_to_base_successful(
            self, is_request_valid_mock: mock.MagicMock,
            get_mission_started_mock: mock.MagicMock):
        server.COMM.send_command = mock.MagicMock()
        is_request_valid_mock.return_value = True
        get_mission_started_mock.return_value = True
        client = server.SOCKETIO.test_client(server.APP,
                                             namespace='/limitedAccess')

        self.assertTrue(client.is_connected('/limitedAccess'))

        client.emit('return_to_base', namespace='/limitedAccess')
        server.COMM.send_command.assert_called_once_with(
            COMMANDS.RETURN_TO_BASE.value)
        get_mission_started_mock.assert_called_once()
        is_request_valid_mock.assert_called_once()

    @mock.patch('server.MissionStatus.get_mission_started')
    @mock.patch('server.AccessStatus.is_request_valid')
    @mock.patch('server.COMM.get_full_drone_data',
                mock.MagicMock(return_value='test'))
    def test_return_to_base_unsuccessful(
            self, is_request_valid_mock: mock.MagicMock,
            get_mission_started_mock: mock.MagicMock):
        server.COMM.send_command = mock.MagicMock()
        is_request_valid_mock.return_value = False
        get_mission_started_mock.return_value = True
        client = server.SOCKETIO.test_client(server.APP,
                                             namespace='/limitedAccess')

        self.assertTrue(client.is_connected('/limitedAccess'))

        client.emit('return_to_base', namespace='/limitedAccess')
        server.COMM.send_command.assert_not_called()
        get_mission_started_mock.assert_called_once()
        is_request_valid_mock.assert_called_once()

    @mock.patch('server.COMM.get_full_drone_data',
                mock.MagicMock(return_value='test'))
    @mock.patch('server.AccessStatus.update_specific_client', mock.MagicMock)
    @mock.patch('server.AccessStatus.take_control')
    @mock.patch('server.MissionStatus.update_all_clients')
    def test_take_control_successful(self, update_mock: mock.MagicMock,
                                     take_control_mock: mock.MagicMock):
        client = server.SOCKETIO.test_client(server.APP,
                                             namespace='/limitedAccess')
        self.assertTrue(client.is_connected('/limitedAccess'))

        take_control_mock.return_value = True
        client.emit('take_control', namespace='/limitedAccess')
        take_control_mock.assert_called_once()
        update_mock.assert_called_once_with(server.SOCKETIO)

    @mock.patch('server.COMM.get_full_drone_data',
                mock.MagicMock(return_value='test'))
    @mock.patch('server.AccessStatus.update_specific_client', mock.MagicMock)
    @mock.patch('server.AccessStatus.take_control')
    @mock.patch('server.MissionStatus.update_all_clients')
    def test_take_control_unsuccessful(self, update_mock: mock.MagicMock,
                                       take_control_mock: mock.MagicMock):
        client = server.SOCKETIO.test_client(server.APP,
                                             namespace='/limitedAccess')
        self.assertTrue(client.is_connected('/limitedAccess'))

        take_control_mock.return_value = False
        client.emit('take_control', namespace='/limitedAccess')
        take_control_mock.assert_called_once()
        update_mock.assert_not_called()

    @mock.patch('server.Database.__init__', mock.MagicMock(return_value=None))
    @mock.patch('server.jsonify')
    @mock.patch('server.Database.get_all_missions_time_stamps')
    def test_retrieve_missions(self, get_timestamp_mock: mock.MagicMock,
                               jsonify_mock: mock.MagicMock):
        get_timestamp_mock.return_value = []
        jsonify_mock.return_value = 'test'
        response = server.APP.test_client().get('/getCompletedMissions')
        self.assertEqual(response.get_data(as_text=True), 'test')
        jsonify_mock.assert_called_once_with([])
        get_timestamp_mock.assert_called_once_with()

    @mock.patch('server.Database.__init__', mock.MagicMock(return_value=None))
    @mock.patch('server.jsonify')
    @mock.patch('server.Database.get_mission_logs_from_id')
    def test_retrieve_specific_mission_logs(
            self, get_mission_from_id_mock: mock.MagicMock,
            jsonify_mock: mock.MagicMock):
        get_mission_from_id_mock.return_value = []
        jsonify_mock.return_value = 'test'
        response = server.APP.test_client().get('/getSpecificMissionLogs/1')
        self.assertEqual(response.get_data(as_text=True), 'test')
        get_mission_from_id_mock.assert_called_once_with('1')
        jsonify_mock.assert_called_once_with([])

    @mock.patch('server.Database.__init__', mock.MagicMock(return_value=None))
    @mock.patch('server.jsonify')
    @mock.patch('server.Database.get_mission_map_from_id')
    def test_retrieve_specific_mission_map(
            self, get_mission_from_id_mock: mock.MagicMock,
            jsonify_mock: mock.MagicMock):
        get_mission_from_id_mock.return_value = []
        jsonify_mock.return_value = 'test'
        response = server.APP.test_client().get('/getSpecificMissionMaps/1')
        self.assertEqual(response.get_data(as_text=True), 'test')
        get_mission_from_id_mock.assert_called_once_with('1')
        jsonify_mock.assert_called_once_with([])

    @mock.patch('server.COMM.get_full_drone_data',
                mock.MagicMock(return_value='test'))
    @mock.patch('server.AccessStatus.update_specific_client', mock.MagicMock)
    @mock.patch('server.AccessStatus.revoke_controlling_client')
    @mock.patch('server.MissionStatus.update_all_clients')
    def test_revoke_control_successful(self, update_mock: mock.MagicMock,
                                       revoke_mock: mock.MagicMock):
        client = server.SOCKETIO.test_client(server.APP,
                                             namespace='/limitedAccess')
        self.assertTrue(client.is_connected('/limitedAccess'))

        revoke_mock.return_value = True
        client.emit('revoke_control', namespace='/limitedAccess')

        update_mock.assert_called_once_with(server.SOCKETIO)
        revoke_mock.assert_called_once_with(server.SOCKETIO, request)

    @mock.patch('server.COMM.get_full_drone_data',
                mock.MagicMock(return_value='test'))
    @mock.patch('server.AccessStatus.update_specific_client', mock.MagicMock)
    @mock.patch('server.AccessStatus.revoke_controlling_client')
    @mock.patch('server.MissionStatus.update_all_clients')
    def test_revoke_control_unsuccessful(self, update_mock: mock.MagicMock,
                                         revoke_mock: mock.MagicMock):
        client = server.SOCKETIO.test_client(server.APP,
                                             namespace='/limitedAccess')
        self.assertTrue(client.is_connected('/limitedAccess'))

        revoke_mock.return_value = False
        client.emit('revoke_control', namespace='/limitedAccess')

        update_mock.assert_not_called()
        revoke_mock.assert_called_once_with(server.SOCKETIO, request)

    @mock.patch('server.COMM.get_drones', mock.MagicMock(return_value='test'))
    @mock.patch('server.AccessStatus.update_specific_client', mock.MagicMock)
    @mock.patch('server.AccessStatus.client_disconnected')
    @mock.patch('server.MissionStatus.update_all_clients')
    def test_disconnect_sucessful(self, update_mock: mock.MagicMock,
                                  client_disconnected_mock: mock.MagicMock):

        client_disconnected_mock.return_value = True

        client = server.SOCKETIO.test_client(server.APP,
                                             namespace='/limitedAccess')
        self.assertTrue(client.is_connected('/limitedAccess'))

        client.disconnect('/limitedAccess')
        update_mock.assert_called_once_with(server.SOCKETIO)
        client_disconnected_mock.assert_called_once_with(
            server.SOCKETIO, request)

    @mock.patch('server.COMM.get_drones', mock.MagicMock(return_value='test'))
    @mock.patch('server.AccessStatus.update_specific_client', mock.MagicMock)
    @mock.patch('server.AccessStatus.client_disconnected')
    @mock.patch('server.MissionStatus.update_all_clients')
    def test_disconnect_unsucessful(self, update_mock: mock.MagicMock,
                                    client_disconnected_mock: mock.MagicMock):

        client_disconnected_mock.return_value = False

        client = server.SOCKETIO.test_client(server.APP,
                                             namespace='/limitedAccess')
        self.assertTrue(client.is_connected('/limitedAccess'))

        client.disconnect('/limitedAccess')
        update_mock.assert_not_called()
        client_disconnected_mock.assert_called_once_with(
            server.SOCKETIO, request)

    @mock.patch('server.MissionStatus.client_connected')
    def test_mission_connect(self, client_connected_mock: mock.MagicMock):

        client = server.SOCKETIO.test_client(server.APP,
                                             namespace='/getMissionStatus')
        self.assertTrue(client.is_connected('/getMissionStatus'))
        client_connected_mock.assert_called_once_with(server.SOCKETIO,
                                                      server.request)

    @mock.patch('server.AccessStatus.update_specific_client')
    @mock.patch('server.COMM.get_full_drone_data')
    def test_connect(self, get_full_drone_data_mock: mock.MagicMock,
                     update_mock: mock.MagicMock):
        get_full_drone_data_mock.return_value = ['test']

        client = server.SOCKETIO.test_client(server.APP,
                                             namespace='/limitedAccess')
        self.assertTrue(client.is_connected('/limitedAccess'))
        update_mock.assert_called_once()
        get_full_drone_data_mock.assert_called_once_with()
        self.assertEqual(
            client.get_received('/limitedAccess')[1], {
                'name': 'droneList',
                'args': [['test']],
                'namespace': '/limitedAccess'
            })

    @mock.patch('server.start_drone_status_task')
    def test_send_drone_status(self, start_drone_mock: mock.MagicMock):
        client = server.SOCKETIO.test_client(server.APP,
                                             namespace='/getDroneStatus')
        self.assertTrue(client.is_connected('/getDroneStatus'))
        start_drone_mock.assert_called_once_with(server.SOCKETIO)

    @mock.patch('server.Map.get_all_filtered_data')
    def test_send_all_map_data(self, filtered_data_mock: mock.MagicMock):
        filtered_data_mock.return_value = 'test'
        client = server.SOCKETIO.test_client(server.APP,
                                             namespace='/getAllMapData')
        self.assertTrue(client.is_connected('/getAllMapData'))
        self.assertEqual(
            client.get_received('/getAllMapData')[0]['args'], ['test'])

    def test_send_logs(self):
        server.COMM.logs = 'test'

        client = server.SOCKETIO.test_client(server.APP, namespace='/getLogs')
        self.assertEqual(client.get_received('/getLogs'), [{
            'name': 'get_logs',
            'args': ['test'],
            'namespace': '/getLogs'
        }])

    @mock.patch('server.COMM.get_full_drone_data',
                mock.MagicMock(return_value='test'))
    @mock.patch('server.MissionStatus.update_p2p_gradient_value')
    @mock.patch('server.MissionStatus.get_mission_started')
    @mock.patch('server.AccessStatus.is_request_valid')
    def test_set_p2p_true_successful(self,
                                     get_mission_started_mock: mock.MagicMock,
                                     is_request_mock: mock.MagicMock,
                                     update_p2p_mock: mock.MagicMock):
        get_mission_started_mock.return_value = True
        is_request_mock.return_value = True

        server.COMM.send_command = mock.MagicMock()

        client = server.SOCKETIO.test_client(server.APP,
                                             namespace='/limitedAccess')

        client.emit('setP2PGradient', True, namespace='/limitedAccess')
        server.COMM.send_command.assert_called_once_with(
            COMMANDS.START_P2P.value)
        update_p2p_mock.assert_called()

    @mock.patch('server.COMM.get_full_drone_data',
                mock.MagicMock(return_value='test'))
    @mock.patch('server.MissionStatus.update_p2p_gradient_value')
    @mock.patch('server.MissionStatus.get_mission_started')
    @mock.patch('server.AccessStatus.is_request_valid')
    def test_set_p2p_false_successful(self,
                                      get_mission_started_mock: mock.MagicMock,
                                      is_request_mock: mock.MagicMock,
                                      update_p2p_mock: mock.MagicMock):
        get_mission_started_mock.return_value = True
        is_request_mock.return_value = True

        server.COMM.send_command = mock.MagicMock()

        client = server.SOCKETIO.test_client(server.APP,
                                             namespace='/limitedAccess')

        client.emit('setP2PGradient', False, namespace='/limitedAccess')
        server.COMM.send_command.assert_called_once_with(COMMANDS.END_P2P.value)
        update_p2p_mock.assert_called()

    @mock.patch('server.COMM.get_full_drone_data',
                mock.MagicMock(return_value='test'))
    @mock.patch('server.MissionStatus.update_p2p_gradient_value')
    @mock.patch('server.MissionStatus.get_mission_started')
    @mock.patch('server.AccessStatus.is_request_valid')
    def test_set_p2p_unsuccessful(self,
                                  get_mission_started_mock: mock.MagicMock,
                                  is_request_mock: mock.MagicMock,
                                  update_p2p_mock: mock.MagicMock):
        get_mission_started_mock.return_value = False
        is_request_mock.return_value = True

        server.COMM.send_command = mock.MagicMock()

        client = server.SOCKETIO.test_client(server.APP,
                                             namespace='/limitedAccess')

        client.emit('setP2PGradient', True, namespace='/limitedAccess')
        server.COMM.send_command.assert_not_called()
        update_p2p_mock.assert_called_once()