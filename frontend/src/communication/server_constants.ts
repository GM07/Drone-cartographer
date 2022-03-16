import SocketIO from 'socket.io-client';

export const SERVER_ADDRESS =
  window.location.protocol + '//' + window.location.hostname + ':5000';
export const GET_DRONES_ADDRESS = SERVER_ADDRESS + '/getDrones';
export const GET_COMPLETED_MISSIONS = SERVER_ADDRESS + '/getCompletedMissions';
export const GET_SPECIFIC_MISSION = SERVER_ADDRESS + '/getSpecificMission';
export const IDENTIFY_DRONE_ADDRESS = 'identify_drone';
export const TAKE_CONTROL_ADDRESS = 'take_control';
export const REVOKE_CONTROL_ADDRESS = 'revoke_control';
export const LAUNCH_MISSION_ADDRESS = 'launch';
export const RETURN_TO_BASE_ADDRESS = 'return_to_base';
export const TERMINATE_MISSION_ADDRESS = 'terminate';
export const SET_MISSION_TYPE_ADDRESS = 'set_mission_type';
export const SET_DRONE_ADDRESS = 'set_drone';
export const GET_MISSION_STATUS_NAMESPACE = '/getMissionStatus';
export const LIMITED_ACCESS_NAMESPACE = '/limitedAccess';
export const GET_LOGS_NAMESPACE = '/getLogs';
export const HTTP_OK = 200;
export const TIME_MULTIPLIER = 60000;

export const SOCKETIO_SERVER_STATUS = SocketIO(
  SERVER_ADDRESS + GET_MISSION_STATUS_NAMESPACE
);
export const SOCKETIO_GET_LOGS = SocketIO(SERVER_ADDRESS + GET_LOGS_NAMESPACE, {
  transports: ['websocket'],
}).close();

export const SOCKETIO_LIMITED_ACCESS = SocketIO(
  SERVER_ADDRESS + LIMITED_ACCESS_NAMESPACE
).close();
