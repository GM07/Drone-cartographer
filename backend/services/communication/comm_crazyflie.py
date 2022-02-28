from time import sleep
import threading
from typing import Dict, List
import cflib.crtp
from cflib.crazyflie import Crazyflie
from cflib.crazyflie.syncCrazyflie import SyncCrazyflie
from cflib.crazyflie.log import LogConfig
from cflib.crazyflie.syncLogger import SyncLogger

from constants import COMMANDS
from services.communication.abstract_comm import AbstractComm
from services.data.drone_data import DroneData

class CommCrazyflie(AbstractComm): 

    def __init__(self, links: List):

        print('Creating Embedded Crazyflie communication')
        self.crazyflies: List[Crazyflie] = list(map(lambda link: Crazyflie(rw_cache='./cache'), links))
        self.links = links
        self.crazyflies_by_id: Dict[str, Crazyflie] = dict()
        for link, crazyflie in zip(links, self.crazyflies):
            self.crazyflies_by_id[link] = crazyflie 
        self.initialized_drivers = False
        self.sync_crazyflies: List[SyncCrazyflie] = []
        self.__init_drivers()
        self.setup_log()

    def __del__(self):
        for sync in self.sync_crazyflies:
            sync.close_link()

    def __init_drivers(self):
        cflib.crtp.init_drivers()

    def setup_log(self):
        self.log_configs: List[LogConfig] = []
        for crazyflie in self.crazyflies:
            log_config = LogConfig(name='DroneData', period_in_ms=AbstractComm.DELAY_RECEIVER_MS)
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
            sync.open_link()
            sync.cf.log.add_config(config)
            config.data_received_cb.add_callback(self.__retrieve_log)
            config.start()

    def send_command(self, command: COMMANDS, links = []) -> None:

        sending_links = self.links if len(links) == 0 else links

        for link in sending_links:
            packet = bytearray(command) # Command must be an array of numbers
            self.crazyflies_by_id[link].open_link(link)
            print('Sending packet : ', packet)
            self.crazyflies_by_id[link].appchannel.send_packet(packet)
            self.crazyflies_by_id[link].close_link()


    def __retrieve_log(self, timestamp, data, logconf: LogConfig):
        print('[%d][%s]: %s' % (timestamp, logconf.id, data))