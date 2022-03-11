import shutil
import pathlib
import os
import subprocess
import xml.etree.ElementTree as ET

class SimulationConfiguration:

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
        body.set('position', str(drone['xPos'])
				+ ',' + str(drone['yPos']) + ',0')
        body.set('orientation', '0,0,0')

        tree.write(current_path + '/crazyflie_sensing.argos')


    def add_obstacles(self):
        current_path = str(pathlib.Path(__file__).parent.resolve())
        tree = ET.parse(current_path + '/crazyflie_sensing.argos')
        arena = tree.find('arena')

        distribute = ET.SubElement(arena, 'distribute')

        position = ET.SubElement(distribute, 'position')
        position.set('method', 'uniform')
        position.set('min', '-2.5,-2.5,0')
        position.set('max', '2.5,2.5,0')

        orientation = ET.SubElement(distribute, 'orientation')
        orientation.set('method', 'uniform')
        orientation.set('min', '0,0,0')
        orientation.set('max', '360,0,0')

        entity = ET.SubElement(distribute, 'entity')
        entity.set('quantity', '5')
        entity.set('max_trials', '100')

        box = ET.SubElement(entity, 'box')
        box.set('id', 'b')
        box.set('size', '0.05,2,1')
        box.set('movable', 'false')

        tree.write(current_path + '/crazyflie_sensing.argos')

    def launch(self):
        nb_tries = 0
        max_nb_tries = 25
        current_path = str(pathlib.Path(__file__).parent.resolve())
        os.system('docker cp ' + current_path
				+ '/crazyflie_sensing.argos embedded:/crazyflie_sensing.argos')

        while nb_tries < max_nb_tries:
            return_value = subprocess.call(current_path + '/docker_exec.sh',
						shell=True)

            if return_value == 0:
                break
            nb_tries = nb_tries + 1
        os.system('rm -f ' + current_path + '/crazyflie_sensing.argos')

