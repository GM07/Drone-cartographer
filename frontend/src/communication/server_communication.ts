import * as SERVER_CONSTANTS from '@/communication/server_constants';
import {Drone} from '@/communication/drone';

export class ServerCommunication {
  public static getConnectedDrones(): Promise<Response> {
    return fetch(SERVER_CONSTANTS.GET_DRONES_ADDRESS, {
      method: 'GET',
    });
  }

  public static identifyDrone(droneId: string): boolean {
    if (SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.connected) {
      SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.emit(
        SERVER_CONSTANTS.IDENTIFY_DRONE_ADDRESS,
        droneId
      );
    }

    return SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.connected;
  }

  public static takeMissionControl(): boolean {
    if (SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.connected) {
      SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.emit(
        SERVER_CONSTANTS.TAKE_CONTROL_ADDRESS
      );
    }

    return SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.connected;
  }

  public static takeMissionControlTimeout(
    timeout: number,
    callback: () => void
  ): void {
    SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.timeout(timeout).emit(
      SERVER_CONSTANTS.TAKE_CONTROL_ADDRESS,
      callback
    );
  }

  public static revokeMissionControl(): boolean {
    if (SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.connected) {
      SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.emit(
        SERVER_CONSTANTS.REVOKE_CONTROL_ADDRESS
      );
    }

    return SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.connected;
  }

  public static setMissionType(isMissionSimulated: boolean): boolean {
    if (SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.connected) {
      SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.emit(
        SERVER_CONSTANTS.SET_MISSION_TYPE_ADDRESS,
        isMissionSimulated
      );
    }

    return SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.connected;
  }

  public static launchMission(
    isMissionSimulated: boolean,
    droneList: Drone[],
    callback: () => void
  ): boolean {
    if (SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.connected) {
      SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.emit(
        SERVER_CONSTANTS.LAUNCH_MISSION_ADDRESS,
        isMissionSimulated,
        droneList,
        callback
      );
    }

    return SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.connected;
  }

  public static returnToBase(callback: () => void): boolean {
    if (SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.connected) {
      SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.emit(
        SERVER_CONSTANTS.RETURN_TO_BASE_ADDRESS,
        callback
      );
    }

    return SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.connected;
  }

  public static terminateMission(callback: () => void): boolean {
    if (SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.connected) {
      SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.emit(
        SERVER_CONSTANTS.TERMINATE_MISSION_ADDRESS,
        callback
      );
    }

    return SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.connected;
  }

  public static getCompletedMissions(): Promise<Response> {
    return fetch(SERVER_CONSTANTS.GET_COMPLETED_MISSIONS);
  }
}
