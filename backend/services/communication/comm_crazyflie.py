"""This module has the CommCrazyflie class that is used to
communicate with the physical drones """

from typing import List
import cflib.crtp
from cflib.crazyflie import Crazyflie
from cflib.crazyflie.syncCrazyflie import SyncCrazyflie
from cflib.crazyflie.log import LogConfig
from flask_socketio import SocketIO

from constants import COMMANDS
from services.data.drone_data import DroneState, log_data_to_drone_data
from services.communication.abstract_comm import AbstractComm

from services.data.map import Map, MapData
from services.data.starting_drone_data import StartingDroneData


class CommCrazyflie(AbstractComm):
    """This class is used to communicate with the crazyflie
    drones and inherits from the AbstractComm class
    An example use is comm=CommCrazyflie([])
    comm.__init_drivers()"""

    def __init__(self, socket_io: SocketIO,
                 drone_list: List[StartingDroneData]):
        super().__init__(socket_io, drone_list)
        if drone_list is None:
            print('Error : drone list is empty')
            self.sync_crazyflies = []
            self.set_drone([])
            return

        print('Creating Embedded Crazyflie communication with drone list :',
              drone_list)
        Map().set_drone_len(len(drone_list))
        self.links = list(map(lambda drone: drone.name, drone_list))
        self.crazyflies: list[Crazyflie] = list(
            map(lambda link: Crazyflie(rw_cache='./cache'), self.links))
        self.crazyflies_by_id = {}
        for link, crazyflie in zip(self.links, self.crazyflies):
            self.crazyflies_by_id[link] = crazyflie
        self.initialized_drivers = False
        self.sync_crazyflies: List[SyncCrazyflie] = []
        self.__init_drivers()
        try:
            self.setup_log()
        except Exception as e:
            print(f'Exception: {e}')

    def __del__(self):
        print('destructor called')
        self.shutdown()

    def __init_drivers(self):
        cflib.crtp.init_drivers()

    def shutdown(self):
        print(f'shutdown called : {self.sync_crazyflies}')
        for sync in self.sync_crazyflies:
            print(f'closing link : {sync}')
            sync.close_link()
        return super().shutdown()

    def setup_log(self):
        self.log_configs: List[LogConfig] = []
        for index, crazyflie in enumerate(self.crazyflies):
            log_config = LogConfig(name=str(self.drone_list[index].name),
                                   period_in_ms=AbstractComm.DELAY_RECEIVER_MS)
            log_config.add_variable('range.front', 'uint16_t')
            log_config.add_variable('range.left', 'uint16_t')
            log_config.add_variable('range.right', 'uint16_t')
            log_config.add_variable('range.back', 'uint16_t')
            log_config.add_variable('kalman.stateX', 'float')
            log_config.add_variable('kalman.stateY', 'float')
            log_config.add_variable('kalman.stateZ', 'float')
            log_config.add_variable('pm.batteryLevel', 'uint8_t')
            log_config.add_variable('custom.droneCustomState', 'uint8_t')
            self.log_configs.append(log_config)

        self.sync_crazyflies = []
        for link, crazyflie in zip(self.links, self.crazyflies):
            self.sync_crazyflies.append(SyncCrazyflie(link, cf=crazyflie))

        for sync, config in zip(self.sync_crazyflies, self.log_configs):
            try:
                print(f'opening link ({self.drone_list}) : {sync}')
                if not sync.is_link_open():
                    sync.open_link()
                sync.cf.log.add_config(config)
                config.data_received_cb.add_callback(self.__retrieve_log)
                config.start()
            except Exception as e:
                print('Exception: {}'.format(e))

    def send_command(self,
                     command: COMMANDS,
                     links=[],
                     args: bytes = None) -> None:
        drone_list_name = [drone.name for drone in self.drone_list]
        sending_links = drone_list_name if len(links) == 0 else links

        if command == COMMANDS.LAUNCH.value:
            self.mission_manager.start_current_mission(len(self.drone_list),
                                                       False)
            self.logs = []

        for link in sending_links:
            packet = bytearray(command)  # Command must be an array of numbers
            if args is not None:
                for arg in args:
                    packet.append(arg)
            print('Sending packet : ', packet)
            try:
                self.crazyflies_by_id[link].appchannel.send_packet(packet)
            except Exception as e:
                print(f'Error : {e}')

        if command == COMMANDS.LAND.value:
            self.mission_manager.end_current_mission(self.logs)

    def __retrieve_log(self, timestamp, data, logconf: LogConfig):
        drone_data = log_data_to_drone_data(logconf.name, data)
        Map().add_data(MapData(logconf.name, drone_data), self.SOCKETIO)
        # print('[%d][%s]: %s' % (timestamp, logconf.id, data))
        # print(f'{timestamp}{logconf.id}:{data}')
        self.send_log(f'{logconf.id}{data} ')
        self.send_drone_status([drone_data.to_dict()])
        self.set_drone_data(drone_data)

    def validate_name(self, name: str) -> str:
        return name
