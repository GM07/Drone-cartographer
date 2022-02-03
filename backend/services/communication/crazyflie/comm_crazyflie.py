import cflib.crtp
from cflib.crazyflie import Crazyflie
from numpy import number

class CommCrazyflie: 

    def __init__(self):
        cflib.crtp.init_drivers()
        self.crazyflie = Crazyflie()
    
    def send_command(self, command, link_uri):
        self.crazyflie.open_link(link_uri)
        packet = bytearray(command) # Command must be an array of numbers
        self.crazyflie.appchannel.send_packet(packet)
        self.crazyflie.close_link()
