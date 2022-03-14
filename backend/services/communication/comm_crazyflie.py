from flask_socketio import SocketIO
from typing import List
import cflib.crtp
from cflib.crazyflie import Crazyflie
from cflib.crazyflie.syncCrazyflie import SyncCrazyflie
from cflib.crazyflie.log import LogConfig

from constants import COMMANDS
from services.communication.abstract_comm import AbstractComm
from services.communication.database.mongo_interface import Mission
from time import perf_counter
from datetime import datetime


class CommCrazyflie(AbstractComm):
    """This class is used to communicate with the crazyflie
    drones"""

    def __init__(self, socket_io: SocketIO, links: list, drone_list=None):

        super().__init__(socket_io)
        print('Creating Embedded Crazyflie communication')
        self.crazyflies: list[Crazyflie] = list(
            map(lambda link: Crazyflie(rw_cache='./cache'), links))
        self.links = links
        self.crazyflies_by_id = {}
        self.drone_list = drone_list
        for link, crazyflie in zip(links, self.crazyflies):
            self.crazyflies_by_id[link] = crazyflie
        self.initialized_drivers = False
        self.sync_crazyflies: list[SyncCrazyflie] = []
        self.__init_drivers()
        self.setup_log()
        self.start_time = perf_counter()
        self.end_time: int = 0
        self.current_mission = Mission(0, len(self.crazyflies), False, 0)

        self.current_mission.is_simulated = False

    def __del__(self):
        for sync in self.sync_crazyflies:
            sync.close_link()

    def shutdown(self):
        self.mission_end_time = perf_counter()
        self.current_mission.flight_duration = self.start_time - self.end_time
        self.current_mission.logs = self.logs
        return super().shutdown()

    def __init_drivers(self):
        cflib.crtp.init_drivers()

    def setup_log(self):
        self.log_configs: list[LogConfig] = []
        for crazyflie in self.crazyflies:
            log_config = LogConfig(name='DroneData',
                                   period_in_ms=AbstractComm.DELAY_RECEIVER_MS)
            log_config.add_variable('range.front', 'uint16_t')
            log_config.add_variable('range.left', 'uint16_t')
            log_config.add_variable('range.right', 'uint16_t')
            log_config.add_variable('range.back', 'uint16_t')
            log_config.add_variable('kalman.stateX', 'float')
            log_config.add_variable('kalman.stateY', 'float')
            log_config.add_variable('kalman.stateZ', 'float')
            log_config.add_variable('pm.batteryLevel', 'uint8_t')
            log_config.add_variable('custom.state', 'uint8_t')
            log_config.cf = crazyflie
            self.log_configs.append(log_config)

        self.sync_crazyflies: list[SyncCrazyflie] = []

        for link, crazyflie in zip(self.links, self.crazyflies):
            self.sync_crazyflies.append(SyncCrazyflie(link, cf=crazyflie))

        for sync, config in zip(self.sync_crazyflies, self.log_configs):
            sync.open_link()
            sync.cf.log.add_config(config)
            config.data_received_cb.add_callback(self.__retrieve_log)
            config.start()

    def send_command(self, command: COMMANDS, links=[]) -> None:

        sending_links = self.links if len(links) == 0 else links

        for link in sending_links:
            packet = bytearray(command)  # Command must be an array of numbers
            print('Sending packet : ', packet)
            self.crazyflies_by_id[link].appchannel.send_packet(packet)

    def __retrieve_log(self, timestamp, data, logconf: LogConfig):
        print('[%d][%s]: %s' % (timestamp, logconf.id, data))
        self.send_log([(datetime.now().isoformat(), f'{logconf.id}{data} ')])

    def send_command_to_all_drones(self, command):
        self.send_log([(datetime.now().isoformat(), command)])

        for drone in self.drone_list:
            self.send_command(command, drone['name'])
