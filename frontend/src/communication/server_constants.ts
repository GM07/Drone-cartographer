export const SERVER_ADDRESS =
  window.location.protocol + '//' + window.location.hostname + ':5000';
export const GET_DRONES_ADDRESS = SERVER_ADDRESS + '/getDrones';
export const IDENTIFY_DRONE_ADDRESS = SERVER_ADDRESS + '/identifyDrone';
export const LAUNCH_MISSION_ADDRESS = SERVER_ADDRESS + '/launch';
export const TERMINATE_MISSION_ADDRESS = SERVER_ADDRESS + '/terminate';
export const GET_MISSION_STATUS_PATH = '/getMissionStatus';
export const HTTP_OK = 200;
