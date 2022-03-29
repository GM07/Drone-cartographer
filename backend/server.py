from gevent import monkey

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
from constants import MAX_TIMEOUT, COMMANDS, URI
from services.communication.database.mongo_interface import Database

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
)

# PyMongo instance to communicate with DB -> Add when DB created
# app.config['MONGO_URI'] = 'mongodb://localhost:27017/db'
# mongo = PyMongo(app)

COMM: AbstractComm = CommCrazyflie(SOCKETIO, [])


# Identifying drones
@SOCKETIO.on('identify_drone', namespace='/limitedAccess')
def identify_drone(drone_addr):
    if not AccessStatus.is_request_valid(request):
        return ''

    global COMM
    COMM.send_command(COMMANDS.IDENTIFY.value, [drone_addr])
    return 'Identified drone'


@APP.route('/getFiles')
def get_files():
    return {
        'test.cpp':
            """#include "components/drone.h"
#include "utils/math.h"

constexpr float kMinDistanceObstacle = 200.0F;  // Millimeters

void Drone::step() {
  updateCrashStatus();
  updateSensorsData();

  droneState = static_cast<uint8_t>(m_controller->state);

  m_controller->sendP2PMessage(static_cast<void *>(&m_data), sizeof(m_data));
  m_controller->receiveP2PMessage(&m_peerData);

  switch (m_controller->state) {
    case State::kIdle:
      break;
    case State::kTakingOff:
      if (m_controller->isTrajectoryFinished()) {
        m_controller->state = State::kExploring;
        m_controller->setVelocity(m_data.direction, kDroneSpeed);
      }
      break;
    case State::kLanding:
      if (m_controller->isTrajectoryFinished()) {
        m_controller->state = State::kIdle;
      }
      break;
    case State::kExploring:
      m_normal = Vector3D();
      wallAvoidance();
      collisionAvoidance();
      changeDirection();
      m_controller->setVelocity(m_data.direction, kDroneSpeed);
    default:
      break;
  }
}

void Drone::wallAvoidance() {
  if (m_controller->data.front > 0 &&
      m_controller->data.front <= kMinDistanceObstacle) {
    m_normal += Vector3D::x(-1.0F);
  }

  if (m_controller->data.back > 0 &&
      m_controller->data.back <= kMinDistanceObstacle) {
    m_normal += Vector3D::x(1.0F);
  }

  // This extra condition makes sure that if we are trapped between walls we
  // move away from the closest one
  if (m_controller->data.back > 0 && m_controller->data.front > 0 &&
      m_controller->data.back <= kMinDistanceObstacle &&
      m_controller->data.front <= kMinDistanceObstacle) {
    m_normal += m_controller->data.back < m_controller->data.front
                    ? Vector3D::x(1.0F)
                    : Vector3D::x(-1.0F);
  }

  if (m_controller->data.left > 0 &&
      m_controller->data.left <= kMinDistanceObstacle) {
    m_normal += Vector3D::y(-1.0F);
  }

  if (m_controller->data.right > 0 &&
      m_controller->data.right <= kMinDistanceObstacle) {
    m_normal += Vector3D::y(1.0F);
  }

  // This extra condition makes sure that if we are trapped between walls we
  // move away from the closest one
  if (m_controller->data.left > 0 && m_controller->data.right > 0 &&
      m_controller->data.left <= kMinDistanceObstacle &&
      m_controller->data.right <= kMinDistanceObstacle) {
    m_normal += m_controller->data.left < m_controller->data.right
                    ? Vector3D::y(-1.0F)
                    : Vector3D::y(1.0F);
  }

  if (areAlmostEqual<Vector3D>(m_normal, m_data.direction) ||
      Vector3D::areSameDirection(m_data.direction, m_normal)) {
    m_normal = Vector3D();
  }
}

void Drone::collisionAvoidance() {
  for (const std::pair<const uint64_t, DroneData> &data : m_peerData) {
    DroneData peerData = data.second;

    if (peerData.range <= m_controller->getMinCollisionAvoidanceDistance()) {
      if (m_usedPeerData.find(peerData.id) == m_usedPeerData.end()) {
        m_usedPeerData.insert_or_assign(peerData.id, peerData);
        m_normal += peerData.direction - m_data.direction;
        m_controller->blinkLED(LED::kLedGreenLeft);
      }
    } else {
      m_usedPeerData.erase(peerData.id);
    }
  }
}

void Drone::changeDirection() {
  if (!areAlmostEqual<Vector3D>(m_normal, Vector3D())) {
    Vector3D newDirection = m_data.direction.reflect(m_normal);

    if (!areAlmostEqual<Vector3D>(m_data.direction, newDirection)) {
      m_data.direction = newDirection;
    }
  }
}

void Drone::initDrone() { m_data.id = m_controller->getId(); }

        
        """
    }


# Recompile firmware
@SOCKETIO.on('recompile', namespace='/limitedAccess')
def recompile():
    if not AccessStatus.is_request_valid(request):
        return ''

    global COMM
    COMM.recompile()
    return 'Recompiled'


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
