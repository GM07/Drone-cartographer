from pickle import NONE
from flask import jsonify, Flask, request
from flask_socketio import SocketIO
from flask_pymongo import PyMongo
from flask_cors import CORS
from services.communication.crazyflie.comm_crazyflie import CommCrazyflie
from services.communication.simulation.comm_simulation import CommSimulation
from services.status.mission_status import *
from constants import MAX_TIMEOUT, COMMANDS, URI
import os

# Flask application
APP = Flask(__name__)
# In order to accept external requests
CORS(APP)
APP.config['SECRET_KEY'] = 'dev'

# Socketio instance to communicate with frontend
ASYNC_MODE = None
SOCKETIO = SocketIO(APP, async_mode=ASYNC_MODE, cors_allowed_origins='*')

# PyMongo instance to communicate with DB -> Add when DB created
# app.config['MONGO_URI'] = 'mongodb://localhost:27017/db'
# mongo = PyMongo(app)
COMM_SIMULATION = CommSimulation()
COMM_CRAZYFLIE = CommCrazyflie()

# Get drone addresses
@APP.route('/getDrones')
def get_drones():
    return jsonify(URI)

# Identifying drones
@APP.route('/identifyDrone', methods=['POST'])
def identify_drone():
    drone_addr = request.get_json()
    COMM_CRAZYFLIE.send_command(COMMANDS.IDENTIFY.value, drone_addr)
    return 'Identified drone'

# Launch mission
@APP.route('/launch', methods=['POST'])
def launch():
    if(get_mission_started()):
        return ''

    is_simulated = request.get_json()
    print("launch")
    if is_simulated:
        os.system("docker exec -d embedded /bin/bash -c \"echo 'IyEvYmluL2Jhc2gKQVJHT1NfTE9DPSQoZmluZCAvIC1pbmFtZSAiY3JhenlmbGllX3NlbnNpbmcuYXJnb3MiIDI+IC9kZXYvbnVsbCkKYXJnb3MzIC1jICRBUkdPU19MT0MK' | base64 -d | /bin/bash\"")
        COMM_SIMULATION.send_command(COMMANDS.LAUNCH.value)
    else:
        COMM_CRAZYFLIE.send_command(COMMANDS.LAUNCH.value, URI[0])
        COMM_CRAZYFLIE.send_command(COMMANDS.LAUNCH.value, URI[1])
    
    set_mission_simulated(is_simulated)
    set_mission_started(True)
    update_status()
    return 'Launched'

# Terminate mission
@APP.route('/terminate')
def terminate():
    if(not get_mission_started()):
        return ''

    if get_mission_simulated():
        COMM_SIMULATION.send_command(COMMANDS.LAND.value)
    else:
        COMM_CRAZYFLIE.send_command(COMMANDS.LAND.value, URI[0])
        COMM_CRAZYFLIE.send_command(COMMANDS.LAND.value, URI[1])

    set_mission_started(False)
    update_status()
    return ''


# Communication with frontend using socketio
@SOCKETIO.on('connect', namespace="/getMissionStatus")
def connection():
    SOCKETIO.emit('update_status', get_mission_status(),namespace="/getMissionStatus", room=request.sid)
    return ''

@SOCKETIO.on('update_status', namespace="/getMissionStatus")
def update_status():
    SOCKETIO.emit('update_status', get_mission_status(), namespace="/getMissionStatus", broadcast=True, include_self=False, skip_sid=True)
    return ''


if __name__ == '__main__':
    print('The backend is running on port 5000')
    SOCKETIO.run(APP, debug=False, host='0.0.0.0', port=5000)
    
  