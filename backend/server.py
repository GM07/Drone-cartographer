from pickle import NONE
from flask import jsonify, Flask, request
from flask_socketio import SocketIO
from flask_cors import CORS
from services.communication.abstract_comm import AbstractComm
from services.communication.comm_crazyflie import CommCrazyflie
from services.communication.comm_simulation import CommSimulation
import services.status.access_status as AccessStatus
import services.status.mission_status as MissionStatus
from services.communication.simulation_configuration import SimulationConfiguration;
from constants import MAX_TIMEOUT, COMMANDS, URI

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

COMM : AbstractComm = CommCrazyflie([])

@APP.route('/getDrones')
def get_drones():
    return jsonify(URI)


# Identifying drones
@SOCKETIO.on('identify_drone', namespace="/limitedAccess")
def identify_drone(drone_addr):
    if not AccessStatus.is_request_valid(request):
        return ''

    COMM.send_command(COMMANDS.IDENTIFY.value, [drone_addr])
    return 'Identified drone'


# Launch mission
@SOCKETIO.on('launch', namespace="/limitedAccess")
def launch(is_simulated: bool, drone_list):
    if(MissionStatus.get_mission_started() or not AccessStatus.is_request_valid(request)):
        return ''

    global COMM
    if is_simulated:
        configuration = SimulationConfiguration()
        
        for drone in drone_list:
            configuration.add_drone(drone)
        configuration.add_obstacles()
        configuration.launch()

        COMM = CommSimulation(drone_list)
    else:
        COMM = CommCrazyflie(drone_list)

    COMM.send_command(COMMANDS.LAUNCH.value)
    AccessStatus.set_mission_type(SOCKETIO, is_simulated)
    MissionStatus.launch_mission(SOCKETIO)
    return 'Launched'


@SOCKETIO.on('set_mission_type', namespace="/limitedAccess")
def set_mission_type(is_simulated: bool):
    AccessStatus.set_mission_type(SOCKETIO, is_simulated, request.sid)
    global COMM
    COMM.shutdown()
    if is_simulated:
        COMM = CommSimulation()
    else:
        COMM = CommCrazyflie(URI)
    return ''


# Terminate mission
@SOCKETIO.on('terminate', namespace="/limitedAccess")
def terminate():
    if(not MissionStatus.get_mission_started() or not AccessStatus.is_request_valid(request)):
        return ''

    COMM.send_command(COMMANDS.LAND.value)


    MissionStatus.terminate_mission(SOCKETIO)
    return 'Terminated'


@SOCKETIO.on('take_control', namespace="/limitedAccess")
def request_control():
    change = AccessStatus.take_control(SOCKETIO, request)
    if change:
        MissionStatus.update_all_clients(SOCKETIO)
    return ''


@SOCKETIO.on('revoke_control', namespace="/limitedAccess")
def revoke_control():
    change = AccessStatus.revoke_controlling_client(SOCKETIO, request)
    if change:
        MissionStatus.update_all_clients(SOCKETIO)
    return ''


@SOCKETIO.on('disconnect', namespace="/limitedAccess")
def disconnect():
    change = AccessStatus.client_disconnected(SOCKETIO, request)
    if change:
        MissionStatus.update_all_clients(SOCKETIO)
    return ''


@SOCKETIO.on('connect', namespace="/getMissionStatus")
def MissionConnect():
    MissionStatus.client_connected(SOCKETIO, request)
    return ''


if __name__ == '__main__':
    print('The backend is running on port 5000')

    SOCKETIO.run(APP, debug=False, host='0.0.0.0', port=5000)
