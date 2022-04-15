import SocketIO from 'socket.io-client';

export const SERVER_ADDRESS =
  process.env.NODE_ENV === 'test'
    ? ''
    : window.location.protocol + '//' + window.location.hostname + ':5000';
export const GET_DRONES_ADDRESS = SERVER_ADDRESS + '/getDrones';
export const GET_COMPLETED_MISSIONS = SERVER_ADDRESS + '/getCompletedMissions';
export const GET_SPECIFIC_MISSION_LOGS =
  SERVER_ADDRESS + '/getSpecificMissionLogs';
export const GET_SPECIFIC_MISSION_MAPS =
  SERVER_ADDRESS + '/getSpecificMissionMaps';
export const GET_FILES_ADDRESS = SERVER_ADDRESS + '/getFiles';
export const SET_FILES_ADDRESS = SERVER_ADDRESS + '/setFiles';
export const GET_CURRENT_MISSION = SERVER_ADDRESS + '/getCurrentMission';
export const IDENTIFY_DRONE_ADDRESS = 'identify_drone';
export const RECOMPILE_ADDRESS = 'recompile';
export const FLASH_ADDRESS = 'flash';
export const TAKE_CONTROL_ADDRESS = 'take_control';
export const REVOKE_CONTROL_ADDRESS = 'revoke_control';
export const LAUNCH_MISSION_ADDRESS = 'launch';
export const SET_P2P_ADDRESS = 'setP2PGradient';
export const UPDATE_P2P_GRADIENT = 'updateP2PGradient';
export const RETURN_TO_BASE_ADDRESS = 'return_to_base';
export const TERMINATE_MISSION_ADDRESS = 'terminate';
export const SET_MISSION_TYPE_ADDRESS = 'set_mission_type';
export const SET_DRONE_ADDRESS = 'set_drone';
export const GET_MISSION_STATUS_NAMESPACE = '/getMissionStatus';
export const LIMITED_ACCESS_NAMESPACE = '/limitedAccess';
export const ALL_MAP_DATA_NAMESPACE = '/getAllMapData';

export const GET_LOGS_NAMESPACE = '/getLogs';
export const DRONE_STATUS_NAMESPACE = '/getDroneStatus';
export const HTTP_OK = 200;
export const TIME_MULTIPLIER = 60000;

export const SOCKETIO_SERVER_STATUS = SocketIO(
  SERVER_ADDRESS + GET_MISSION_STATUS_NAMESPACE
);

export const SOCKETIO_GET_LOGS = SocketIO(SERVER_ADDRESS + GET_LOGS_NAMESPACE, {
  transports: ['websocket'],
}).close();

export const SOCKETIO_DRONE_STATUS = SocketIO(
  SERVER_ADDRESS + DRONE_STATUS_NAMESPACE,
  {
    transports: ['websocket'],
  }
).close();

export const SOCKETIO_LIMITED_ACCESS = SocketIO(
  SERVER_ADDRESS + LIMITED_ACCESS_NAMESPACE
).close();

export const SOCKETIO_MAP_DATA = SocketIO(
  SERVER_ADDRESS + ALL_MAP_DATA_NAMESPACE,
  {
    transports: ['websocket'],
  }
).close();
