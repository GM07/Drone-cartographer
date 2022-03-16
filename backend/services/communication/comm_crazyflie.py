"""This module has the CommCrazyflie class that is used to
communicate with the physical drones """

from typing import Dict, List
import cflib.crtp
from cflib.crazyflie import Crazyflie
from cflib.crazyflie.syncCrazyflie import SyncCrazyflie
from cflib.crazyflie.log import LogConfig

from constants import COMMANDS
from services.communication.database.mongo_interface import Mission, Database
from time import perf_counter
from datetime import datetime
from services.communication.abstract_comm import AbstractComm


class CommCrazyflie(AbstractComm):
    """This class is used to communicate with the crazyflie
    drones and inherits from the AbstractComm class
    An example use is comm=CommCrazyflie([])
    comm.__init_drivers()"""

    def __init__(self, drone_list: list):
        if drone_list is None:
            self.sync_crazyflies = []
            self.drone_list = []
            return

        print('Creating Embedded Crazyflie communication')
        self.links = list(map(lambda drone: drone['name'], drone_list))
        self.crazyflies: list[Crazyflie] = list(
            map(lambda link: Crazyflie(rw_cache='./cache'), self.links))
        self.crazyflies_by_id = {}
        self.drone_list = drone_list
        for link, crazyflie in zip(self.links, self.crazyflies):
            self.crazyflies_by_id[link] = crazyflie
        self.initialized_drivers = False
        self.sync_crazyflies: List[SyncCrazyflie] = []
        self.__init_drivers()
        try:
            self.setup_log()
        except:
            pass

    def __del__(self):
        for sync in self.sync_crazyflies:
            sync.close_link()

    def shutdown(self):

        return super().shutdown()

    def __init_drivers(self):
        cflib.crtp.init_drivers()

    def setup_log(self):
        self.log_configs: List[LogConfig] = []
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

        self.sync_crazyflies = []
        for link, crazyflie in zip(self.links, self.crazyflies):
            self.sync_crazyflies.append(SyncCrazyflie(link, cf=crazyflie))

        for sync, config in zip(self.sync_crazyflies, self.log_configs):
            try:
                sync.open_link()
                sync.cf.log.add_config(config)
                config.data_received_cb.add_callback(self.__retrieve_log)
                config.start()
            except Exception as e:
                print('Exception: {}'.format(e))

    def send_command(self, command: COMMANDS, links=[]) -> None:
        sending_links = self.links if len(links) == 0 else links

        if command == COMMANDS.LAUNCH.value:
            self.current_mission = Mission(0, len(self.drone_list), False, 0,
                                           [[]])
            self.mission_start_time = perf_counter()
        for link in sending_links:
            packet = bytearray(command)  # Command must be an array of numbers
            print('Sending packet : ', packet)
            self.crazyflies_by_id[link].appchannel.send_packet(packet)

        if command == COMMANDS.LAND.value:
            self.current_mission.flight_duration = self.mission_start_time - perf_counter(
            )
            self.current_mission.logs = self.logs
            self.logs = []
            database = Database()
            database.upload_mission_info(self.current_mission)

    def __retrieve_log(self, timestamp, data, logconf: LogConfig):
        print(f'{timestamp}{logconf.id}:{data}')
        self.send_log([(datetime.now().isoformat(), f'{logconf.id}{data} ')])
