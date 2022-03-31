from enum import Enum
from services.communication.byte_decoder import ByteDecoder


class PhysDroneData:

    DATA_SIZE: int = 29

    def __init__(self, data: bytes):
        self.__from_bytes(data)

    def __from_bytes(self, data: bytes):
        decoder: ByteDecoder = ByteDecoder(
            data, ['f', 'f', 'f', 'f', 'f', 'f', 'f', 'i'])
        values = decoder.get_values()
        self.sensors = DroneSensors(values[0], values[1], values[2], values[3])
        self.position = Point2D(values[4], values[5])
        self.battery_level = values[6]
        self.state = DroneState(values[7])

    def __str__(self) -> str:
        return 'Drone Information :\n Sensor (f, l, b, r) : ' \
            + str(self.sensors.front) + ', '\
            + str(self.sensors.left) + ', ' \
            + str(self.sensors.back) + ', ' \
            + str(self.sensors.right) + ', ' \
            + ' \nPosition (x, y, z) : ' \
            + str(self.position.x) + ', ' \
            + str(self.position.y) + ', ' \
            + str(self.position.z) + ', ' \
            + ' \nBattery level ' \
            + str(self.battery_level) \
            + ' \nState ' \
            + str(self.state.name) \
            + '\n\n'
