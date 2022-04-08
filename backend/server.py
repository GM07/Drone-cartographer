import os
from gevent import monkey

from services.communication.bash_executor import BashExecutor

if __name__ == '__main__':
    monkey.patch_all()

from pickle import NONE
"""Root of the Flask Backend for the drone application
Defines all routes in this file"""

from services.communication.comm_tasks import start_drone_status_task, start_logs_task
from flask import jsonify, Flask, request
from flask_socketio import SocketIO
from flask_cors import CORS
from services.communication.abstract_comm import AbstractComm
import services.status.access_status as AccessStatus
import services.status.mission_status as MissionStatus
from services.communication.comm_crazyflie import CommCrazyflie
from services.communication.comm_simulation import CommSimulation
from services.communication.simulation_configuration import SimulationConfiguration
from constants import COMMANDS, RECOMPILE_EMBEDDED_COMMAND, RECOMPILE_SIMULATION_COMMAND, URI
from services.communication.database.mongo_interface import Database
from services.communication.file_helper import FileHelper

# Flask application
APP = Flask(__name__)
# In order to accept external requests
CORS(APP)
APP.config['SECRET_KEY'] = 'dev'

# Socketio instance to communicate with frontend

SOCKETIO = SocketIO(
    APP,
    async_mode='gevent',
    cors_allowed_origins='*',
    logger=False,
)

# Bash executor instance to execute commands and transmit stderr and stdout over websocket
RECOMPILE_SIMULATION = BashExecutor(RECOMPILE_SIMULATION_COMMAND, SOCKETIO,
                                    "/recompileSimulation")
RECOMPILE_EMBEDDED = BashExecutor(RECOMPILE_EMBEDDED_COMMAND, SOCKETIO,
                                  "/recompileEmbedded")
FLASH_ALL_DRONES = BashExecutor("", SOCKETIO, "/flashDrones")

# PyMongo instance to communicate with DB -> Add when DB created
# app.config['MONGO_URI'] = 'mongodb://localhost:27017/db'
# mongo = PyMongo(app)

COMM: AbstractComm = CommCrazyflie(SOCKETIO, [])


# Identifying drones
@SOCKETIO.on('identify_drone', namespace='/limitedAccess')
def identify_drone(drone_addr):
    if not AccessStatus.is_request_valid(request):
        return ''

    COMM.send_command(COMMANDS.IDENTIFY.value, [drone_addr])
    return 'Identified drone'


@APP.route('/setFiles', methods=['POST'])
def set_files():
    files = request.get_json()['keys']
    contents = request.get_json()['values']
    FileHelper.update_files(files, contents)
    return {'output': 'File updated'}


@APP.route('/getFiles')
def get_files():
    abs_path = os.path.abspath(__file__ + '/../../embedded')
    directories = [
        abs_path + '/simulation/src', abs_path + '/shared-firmware/src',
        abs_path + '/shared-firmware/include',
        abs_path + '/embedded-firmware/src'
    ]
    files = FileHelper.get_files(directories)

    files_content = FileHelper.get_files_content(files)

    keys = []
    values = []
    for (key, value) in files_content.items():
        keys.append(key.replace('/workspaces/INF3995-106/', ''))
        values.append(value)

    return jsonify({'keys': keys, 'values': values})


# Recompile firmware
@SOCKETIO.on('recompile', namespace='/limitedAccess')
def recompile():
    RECOMPILE_SIMULATION.start()
    RECOMPILE_EMBEDDED.start()
    return 'Recompiling'


# Reflash firmware
@SOCKETIO.on('flash', namespace='/limitedAccess')
def flash():
    if not AccessStatus.is_request_valid(request):
        return ''

    if AccessStatus.get_mission_simulated():
        return ''

    drone_list = COMM.get_drones()

    COMM.stop_logs()

    # Create the command to flash all drones
    flashDrone = []
    for drone in drone_list:
        flashDrone.append("make cload radio=" + drone['name'])

    bashCommand = f"docker exec embedded sh -c 'cd workspaces/INF3995-106/embedded/embedded-firmware" + " && " + " && ".join(
        flashDrone) + "'"
    FLASH_ALL_DRONES.changeCommand(bashCommand)
    FLASH_ALL_DRONES.start(COMM.start_logs)

    return 'Flashing'


# Launch mission
@SOCKETIO.on('launch', namespace='/limitedAccess')
def launch(is_simulated: bool):
    if (MissionStatus.get_mission_started() or
            not AccessStatus.is_request_valid(request)):
        return ''

    global COMM
    COMM.shutdown()
    drone_list = COMM.get_drones()

    if is_simulated:
        configuration = SimulationConfiguration()

        for drone in drone_list:
            configuration.add_drone(drone)
        configuration.add_obstacles(drone_list)
        configuration.launch()
        COMM = CommSimulation(SOCKETIO, drone_list)
    else:
        COMM = CommCrazyflie(SOCKETIO, drone_list)

    COMM.send_command(COMMANDS.LAUNCH.value)
    AccessStatus.set_mission_type(SOCKETIO, is_simulated)
    MissionStatus.launch_mission(SOCKETIO)
    return 'Launched'


@SOCKETIO.on('set_drone', namespace='/limitedAccess')
def set_drone(drone_list, is_simulated):
    global COMM
    COMM.set_drone(drone_list)

    SOCKETIO.emit('droneList',
                  COMM.get_drones(),
                  namespace='/limitedAccess',
                  broadcast=True,
                  include_self=False,
                  skip_sid=True)

    if not is_simulated:
        COMM.shutdown()
        COMM = CommCrazyflie(
            SOCKETIO, drone_list)  # Recreate object to reconnect to drones
    return ''


@SOCKETIO.on('set_mission_type', namespace='/limitedAccess')
def set_mission_type(is_simulated: bool):
    AccessStatus.set_mission_type(SOCKETIO, is_simulated, request.sid)
    global COMM
    drone_list = COMM.get_drones()
    COMM.shutdown()
    if is_simulated:
        COMM = CommSimulation(SOCKETIO, drone_list)
    else:
        COMM = CommCrazyflie(SOCKETIO, drone_list)
    return ''


# Terminate mission
@SOCKETIO.on('terminate', namespace='/limitedAccess')
def terminate():
    if (not MissionStatus.get_mission_started() or
            not AccessStatus.is_request_valid(request)):
        return ''

    global COMM
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


@APP.route('/getSpecificMission/<id>')
def retrieve_specific_mission(id: str):
    database_connection = Database()
    return jsonify(database_connection.get_mission_from_id(id))


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


@SOCKETIO.on('connect', namespace='/limitedAccess')
def connect():
    AccessStatus.update_specific_client(SOCKETIO, request.sid)
    SOCKETIO.emit('droneList',
                  COMM.get_drones(),
                  namespace='/limitedAccess',
                  room=request.sid)
    return ''


@SOCKETIO.on('connect', namespace='/getDroneStatus')
def send_drone_status():
    start_drone_status_task(SOCKETIO)
    return ''


@SOCKETIO.on('connect', namespace='/getLogs')
def send_logs():

    SOCKETIO.emit('get_logs',
                  COMM.logs,
                  namespace='/getLogs',
                  broadcast=True,
                  include_self=False,
                  skip_sid=True)

    start_logs_task(SOCKETIO)
    return ''


if __name__ == '__main__':
    print('The backend is running on port 5000')
    SOCKETIO.run(APP, debug=False, host='0.0.0.0', port=5000)
