"""Root of the Flask Backend for the drone application
Defines all routes in this file"""
import services.status.access_status as AccessStatus
import services.status.mission_status as MissionStatus

from flask import jsonify, Flask, request
from gevent import monkey

monkey.patch_all()
import threading
from flask_socketio import SocketIO
from flask_cors import CORS
from services.communication.abstract_comm import AbstractComm
from services.communication.comm_crazyflie import CommCrazyflie
from services.communication.comm_simulation import CommSimulation
import services.status.access_status as AccessStatus
import services.status.mission_status as MissionStatus
from services.communication.simulation_configuration import SimulationConfiguration
from constants import MAX_TIMEOUT, COMMANDS, URI
from services.communication.database.mongo_interface import Database

# Flask application
APP = Flask(__name__)
# In order to accept external requests
CORS(APP)
APP.config['SECRET_KEY'] = 'dev'

# Socketio instance to communicate with frontend

SOCKETIO = SocketIO(APP,
                    async_mode='gevent',
                    cors_allowed_origins='*',
                    logger=True)

# PyMongo instance to communicate with DB -> Add when DB created
# app.config['MONGO_URI'] = 'mongodb://localhost:27017/db'
# mongo = PyMongo(app)

COMM: AbstractComm = CommCrazyflie(SOCKETIO, [])


# Get drone addresses
@APP.route('/getDrones')
def get_drones():
    return jsonify(URI)


# Identifying drones
@SOCKETIO.on('identify_drone', namespace='/limitedAccess')
def identify_drone(drone_addr):
    if not AccessStatus.is_request_valid(request):
        return ''

    COMM.send_command(COMMANDS.IDENTIFY.value, [drone_addr])
    return 'Identified drone'


@SOCKETIO.on('launch', namespace='/limitedAccess')
def launch(is_simulated: bool, drone_list):
    if (MissionStatus.get_mission_started() or
            not AccessStatus.is_request_valid(request)):
        return ''

    global COMM
    COMM.shutdown()

    if is_simulated:
        configuration = SimulationConfiguration()

        for drone in drone_list:
            configuration.add_drone(drone)
        configuration.add_obstacles()
        configuration.launch()

        COMM = CommSimulation(SOCKETIO, drone_list)
    else:
        COMM = CommCrazyflie(SOCKETIO, drone_list)

    COMM.send_command(COMMANDS.LAUNCH.value)
    AccessStatus.set_mission_type(SOCKETIO, is_simulated)
    MissionStatus.launch_mission(SOCKETIO)
    return 'Launched'


@SOCKETIO.on('set_mission_type', namespace='/limitedAccess')
def set_mission_type(is_simulated: bool):
    AccessStatus.set_mission_type(SOCKETIO, is_simulated, request.sid)
    global COMM
    COMM.shutdown()
    if is_simulated:
        COMM = CommSimulation(SOCKETIO)
    else:
        COMM = CommCrazyflie(SOCKETIO, URI)
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


def test(arg):
    print('hi')


@SOCKETIO.on('get_logs', namespace='/getLogs')
def send_logs():
    print('here')
    SOCKETIO.emit('get_logs',
                  COMM.logs,
                  namespace='/getLogs',
                  broadcast=True,
                  include_self=False,
                  skip_sid=True,
                  callback=test)

    threading.Timer(2, send_logs).start()


@APP.route('/getCurrentLogs')
def get_current_logs():

    return jsonify(COMM.logs)


if __name__ == '__main__':
    print('The backend is running on port 5000')
    print(SOCKETIO.async_mode)

    SOCKETIO.run(APP, debug=False, host='0.0.0.0', port=5000)
