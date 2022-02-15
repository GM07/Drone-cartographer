from pickle import NONE
from flask import jsonify, Flask, request
from flask_socketio import SocketIO
from flask_pymongo import PyMongo
from flask_cors import CORS
from services.communication.abstract_comm import AbstractComm
from services.communication.comm_crazyflie import CommCrazyflie
from services.communication.comm_simulation import CommSimulation
from services.status.mission_status import *
from constants import MAX_TIMEOUT, COMMANDS, URI

# Flask application
APP = Flask(__name__)
# In order to accept external requests
CORS(APP)
APP.config['SECRET_KEY'] = 'dev'

# Socketio instance to communicate with frontend
ASYNC_MODE = None
SOCKETIO = SocketIO(APP, async_mode=ASYNC_MODE, path="/getMissionStatus", cors_allowed_origins='*')

# PyMongo instance to communicate with DB -> Add when DB created
# app.config['MONGO_URI'] = 'mongodb://localhost:27017/db'
# mongo = PyMongo(app)
COMM_SIMULATION = CommSimulation()
COMM_EMBEDDED = CommCrazyflie(URI)
COMM: AbstractComm = COMM_EMBEDDED

# Get drone addresses
@APP.route('/getDrones')
def get_drones():
    return jsonify(URI)

# Identifying drones
@APP.route('/identifyDrone', methods=['POST'])
def identify_drone():
    drone_addr = request.get_json()
    comm = CommCrazyflie(drone_addr)
    comm.send_command(COMMANDS.IDENTIFY.value)
    return 'Identified drone'

# Launch mission
@APP.route('/launch', methods=['POST'])
def launch():
    # if(get_mission_started()):
    #     return ''

    is_simulated = request.get_json()

    if is_simulated:
        COMM = COMM_SIMULATION
    else:
        COMM = COMM_EMBEDDED

    COMM.send_command(COMMANDS.LAUNCH.value)

    set_mission_simulated(is_simulated)
    set_mission_started(True)
    update_status()
    return 'Launched'

# Terminate mission
@APP.route('/terminate')
def terminate():
    if(not get_mission_started()):
        return ''

    COMM.send_command(COMMANDS.LAND.value)

    set_mission_started(False)
    update_status()
    return ''


# Communication with frontend using socketio
@SOCKETIO.on('connect')
def connection():
    SOCKETIO.emit('update_status', get_mission_status(), room=request.sid)
    return ''

@SOCKETIO.on('update_status')
def update_status():
    SOCKETIO.emit('update_status', get_mission_status(), broadcast=True, include_self=False, skip_sid=True)
    return ''

if __name__ == '__main__':
    print('The backend is running on port 5000')

    SOCKETIO.run(APP, debug=False, host='0.0.0.0', port=5000)
    
  