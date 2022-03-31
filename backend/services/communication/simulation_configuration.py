"""This module includes the necessary class
to be able to configure the simulation"""
import shutil
import pathlib
import os
import subprocess
import math
from dataclasses import dataclass
import random
import xml.etree.ElementTree as ET

WALL_SPACING: float = 0.05


@dataclass
class Box:
    x_pos: float
    y_pos: float
    x_pos_argos: float
    y_pos_argos: float
    width: float
    height: float
    orientation: float

    def __init__(self, x_pos: float, y_pos: float, length: float, width: float,
                 orientation: float):
        self.width = width * math.cos(orientation) + length * math.sin(
            orientation) + WALL_SPACING
        self.height = length * math.cos(orientation) + width * math.sin(
            orientation) + WALL_SPACING
        self.x_pos_argos = x_pos
        self.y_pos_argos = y_pos
        self.x_pos = x_pos - self.width / 2
        self.y_pos = y_pos - self.height / 2
        self.orientation = orientation


class SimulationConfiguration:
    """This class configures the argos3 simulation
    an example would be
    sim = SimulationConfiguration()
    sim.add_drone(Drone())"""

    def __init__(self):
        current_path = str(pathlib.Path(__file__).parent.resolve())
        shutil.copyfile(current_path + '/template.argos',
                        current_path + '/crazyflie_sensing.argos')

    def add_drone(self, drone):
        current_path = str(pathlib.Path(__file__).parent.resolve())
        tree = ET.parse(current_path + '/crazyflie_sensing.argos')
        arena = tree.find('arena')

        crazyflie = ET.SubElement(arena, 'crazyflie')
        crazyflie.set('id', drone['name'])

        controller = ET.SubElement(crazyflie, 'controller')
        controller.set('config', 'ssc')

        battery = ET.SubElement(crazyflie, 'battery')
        battery.set('model', 'time_motion')
        battery.set('delta', '1e-3')
        battery.set('pos_delta', '1e-3')
        battery.set('orient_delta', '1e-3')

        body = ET.SubElement(crazyflie, 'body')
        body.set('position',
                 str(drone['xPos']) + ',' + str(drone['yPos']) + ',0')
        body.set('orientation', '0,0,0')

        tree.write(current_path + '/crazyflie_sensing.argos')

    def add_obstacles(self, drone_list):
        box_list = list()
        WALL_LENGTH = 2
        NB_WALLS = 5
        DRONE_SIZE = 0.2
        ROOM_SIZE = 2.5
        for drone in drone_list:
            box_list.append(
                Box(
                    float(drone['xPos']) - DRONE_SIZE / 2,
                    float(drone['yPos']) - DRONE_SIZE / 2, DRONE_SIZE,
                    DRONE_SIZE, drone['orientation']))

        while (len(box_list) < len(drone_list) + NB_WALLS):
            # Generate random point
            pos_x = random.uniform(-(ROOM_SIZE - WALL_LENGTH / 2),
                                   (ROOM_SIZE - WALL_LENGTH / 2))
            pos_y = random.uniform(-(ROOM_SIZE - WALL_LENGTH / 2),
                                   (ROOM_SIZE - WALL_LENGTH / 2))
            direction = random.uniform(0, math.pi / 2)
            box: Box = Box(pos_x, pos_y, WALL_LENGTH, 0.05, direction)

            is_invalid = False
            for box_test in box_list:
                if self.__is_colliding(box_test, box):
                    is_invalid = True
                    break
            if not is_invalid:
                box_list.append(box)

        current_path = str(pathlib.Path(__file__).parent.resolve())
        tree = ET.parse(current_path + '/crazyflie_sensing.argos')
        arena = tree.find('arena')

        index = len(drone_list)
        while index < len(box_list):
            box = ET.SubElement(arena, 'box')
            box.set('id', 'b' + str(index))
            box.set('size', '0.05,2,1')
            box.set('movable', 'false')

            body = ET.SubElement(box, 'body')
            body.set(
                'position',
                str(box_list[index].x_pos_argos) + ',' +
                str(box_list[index].y_pos_argos) + ',0')
            body.set('orientation',
                     str(math.degrees(box_list[index].orientation)) + ',0,0')

            index += 1

        tree.write(current_path + '/crazyflie_sensing.argos')

    def __is_colliding(self, box1: Box, box2: Box):
        return \
            (box1.x_pos < box2.x_pos + box2.width) \
        and (box1.x_pos + box1.width > box2.x_pos) \
        and (box1.y_pos < box2.y_pos + box2.height) \
        and (box1.y_pos + box1.height > box2.y_pos)

    def launch(self):
        current_path = str(pathlib.Path(__file__).parent.resolve())
        os.system('docker cp ' + current_path +
                  '/crazyflie_sensing.argos embedded:/crazyflie_sensing.argos')

        subprocess.call(current_path + '/docker_exec.sh', shell=True)

        os.system('rm -f ' + current_path + '/crazyflie_sensing.argos')
