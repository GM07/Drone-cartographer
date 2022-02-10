import cflib.crtp
from cflib.crazyflie import Crazyflie
from services.communication.abstract_comm import AbstractComm

class CommCrazyflie(AbstractComm): 

    def __init__(self, links: list, init_drivers: bool = True):
        if init_drivers:
            cflib.crtp.init_drivers()
        
        self.crazyflie = Crazyflie()
        self.links = links

    def with_addr(self, links: list) -> AbstractComm:
        self.links = links
        return self

    def send_command(self, command) -> None:

        for link in self.links:
            self.crazyflie.open_link(link)
            packet = bytearray(command) # Command must be an array of numbers
            print(packet)
            self.crazyflie.appchannel.send_packet(packet)
            self.crazyflie.close_link()

        self.links = self.links_backup