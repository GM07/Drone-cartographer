from time import sleep
import threading
import cflib.crtp
from cflib.crazyflie import Crazyflie
from cflib.crazyflie.syncCrazyflie import SyncCrazyflie
from cflib.crazyflie.log import LogConfig
from cflib.crazyflie.syncLogger import SyncLogger

from constants import COMMANDS
from services.communication.abstract_comm import AbstractComm
from services.data.drone_data import DroneData

class CommCrazyflie(AbstractComm): 

    def __init__(self, links: list, init_drivers: bool = True):

        print('Creating Embedded Crazyflie communication')
        if init_drivers:
            cflib.crtp.init_drivers()
        self.crazyflie = Crazyflie(rw_cache='./cache')
        self.links = links
        self.backup_links = links
        self.setup_log()

        self.receive_thread = threading.Thread(target=self.__receive, name='[Embedded] Receiving thread', args=[self.links])
        self.receive_thread.start()
        self.sending_commands_links = set()

    def setup_log(self):
        self.log_config = LogConfig(name='Stabilizer', period_in_ms=10)
        self.log_config.add_variable('range.front', 'uint16_t')
        self.log_config.add_variable('range.left', 'uint16_t')
        self.log_config.add_variable('range.right', 'uint16_t')
        self.log_config.add_variable('range.back', 'uint16_t')
        self.log_config.add_variable('ctrltarget.x', 'float')
        self.log_config.add_variable('ctrltarget.y', 'float')
        self.log_config.add_variable('ctrltarget.z', 'float')
        self.log_config.add_variable('pm.batteryLevel', 'uint8_t')
        self.log_config.add_variable('custom.state', 'uint8_t')

    def with_addr(self, links: list) -> AbstractComm:
        self.links = links
        return self

    def send_command(self, command: COMMANDS) -> None:

        for link in self.links:
            self.crazyflie.open_link(link)
            packet = bytearray(command) # Command must be an array of numbers
            print('Sending packet : ', packet)
            self.crazyflie.appchannel.send_packet(packet)
            
            self.crazyflie.close_link()

        self.links = self.backup_links

    def __receive(self, links: list):
        print('Receiving thread started')

        while True:
            # Iterate through every radio address
            for link in links:
                try:
                    with SyncCrazyflie(link, cf=self.crazyflie) as sync_crazyflie:
                        with SyncLogger(sync_crazyflie, self.log_config) as logger:

                            for log_entry in logger:

                                timestamp = log_entry[0]
                                data = log_entry[1]
                                logconf_name = log_entry[2]

                                print('[%d][%s]: %s' % (timestamp, logconf_name, data))

                                break
                        logger.disconnect()
                    sync_crazyflie.close_link()
                except Exception as e:
                    print('Error when logging : ', e)

            print(f'Waiting {AbstractComm.DELAY_RECEIVER_MS} ms')
            sleep(AbstractComm.DELAY_RECEIVER_MS / 1000)


            