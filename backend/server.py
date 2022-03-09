"""Root of the Flask Backend for the drone application
Defines all routes in this file"""
import os
import services.status.access_status as AccessStatus
import services.status.mission_status as MissionStatus

from flask import jsonify, Flask, request
from flask_socketio import SocketIO
from flask_cors import CORS
from services.communication.abstract_comm import AbstractComm
from services.communication.comm_crazyflie import CommCrazyflie
from services.communication.comm_simulation import CommSimulation
import services.status.access_status as AccessStatus
import services.status.mission_status as MissionStatus
from constants import MAX_TIMEOUT, COMMANDS, URI
from services.communication.database.mongo_interface import Database
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
#COMM: AbstractComm = CommCrazyflie(URI)


# Get drone addresses
@APP.route('/getDrones')
def get_drones():
    return jsonify(URI)


# Identifying drones
@SOCKETIO.on('identify_drone', namespace='/limitedAccess')
def identify_drone(drone_addr):
    if not AccessStatus.is_request_valid(request):
        return ''

    COMM.send_command(COMMANDS.IDENTIFY.value, links=[drone_addr])
    return 'Identified drone'


# Launch mission
@SOCKETIO.on('launch', namespace='/limitedAccess')
def launch(is_simulated: bool):
    #Add here logs
    if (MissionStatus.get_mission_started() or
            not AccessStatus.is_request_valid(request)):
        return ''

    print('launch')
    if is_simulated:
        # pylint: disable=line-too-long
        os.system(
            "docker exec -d embedded /bin/bash -c \"echo 'IyEvYmluL2Jhc2gKQVJHT1NfTE9DPSQoZmluZCAvIC1pbmFtZSAiY3JhenlmbGllX3NlbnNpbmcuYXJnb3MiIDI+IC9kZXYvbnVsbCkKYXJnb3MzIC1jICRBUkdPU19MT0MK' | base64 -d | /bin/bash\""
        )


# pylint: enable=line-too-long
    COMM.send_command(COMMANDS.LAUNCH.value)

    MissionStatus.launch_mission(SOCKETIO)
    return 'Launched'


@SOCKETIO.on('set_mission_type', namespace='/limitedAccess')
def set_mission_type(is_simulated: bool):
    AccessStatus.set_mission_type(SOCKETIO, is_simulated, request.sid)
    global COMM
    if is_simulated:
        COMM = CommSimulation()
    else:
        COMM = CommCrazyflie(URI)
    return ''


# Terminate mission


@SOCKETIO.on('terminate', namespace='/limitedAccess')
def terminate():
    if (not MissionStatus.get_mission_started() or
            not AccessStatus.is_request_valid(request)):
        return ''

    COMM.send_command(COMMANDS.LAND.value)

    MissionStatus.terminate_mission(SOCKETIO)
    return 'Terminated'


@SOCKETIO.on('take_control', namespace='/limitedAccess')
def request_control():
    change = AccessStatus.take_control(SOCKETIO, request)
    if change:
        MissionStatus.update_all_clients(SOCKETIO)
    return ''


# Get Completed mission logs
@APP.route('/getCompletedMissions')
def retrieve_missions():
    database_connection = Database()
    return jsonify(database_connection.get_all_missions_time_stamps())


@SOCKETIO.on('revoke_control', namespace='/limitedAccess')
def revoke_control():
    change = AccessStatus.revoke_controlling_client(SOCKETIO, request)
    if change:
        MissionStatus.update_all_clients(SOCKETIO)
    return ''


@SOCKETIO.on('disconnect', namespace='/limitedAccess')
def disconnect():
    change = AccessStatus.client_disconnected(SOCKETIO, request)
    if change:
        MissionStatus.update_all_clients(SOCKETIO)
    return ''


@SOCKETIO.on('connect', namespace='/getMissionStatus')
def mission_connect():
    MissionStatus.client_connected(SOCKETIO, request)
    return ''


if __name__ == '__main__':
    print('The backend is running on port 5000')
    SOCKETIO.run(APP, debug=False, host='0.0.0.0', port=5000)
