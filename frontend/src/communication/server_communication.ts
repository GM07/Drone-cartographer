import * as SERVER_CONSTANTS from '@/communication/server_constants';
import {NewDroneData} from '@/communication/drone';

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

  public static recompile(): boolean {
    if (SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.connected) {
      SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.emit(
        SERVER_CONSTANTS.RECOMPILE_ADDRESS
      );
    }

    return true;
  }

  public static async sendFiles(files: Map<string, string>): Promise<Response> {
    const KEYS: string[] = [];
    const VALUES: string[] = [];
    files.forEach((value: string, key: string) => {
      KEYS.push(key);
      VALUES.push(value);
    });

    const BODY = JSON.stringify({keys: KEYS, values: VALUES});
    return (
      await fetch(SERVER_CONSTANTS.SET_FILES_ADDRESS, {
        method: 'POST',
        body: BODY,
        headers: {'Content-type': 'application/json'},
      })
    ).json();
  }

  public static async getFiles(): Promise<Response> {
    return (
      await fetch(SERVER_CONSTANTS.GET_FILES_ADDRESS, {method: 'GET'})
    ).json();
  }

  public static flash(): boolean {
    if (SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.connected) {
      SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.emit(
        SERVER_CONSTANTS.FLASH_ADDRESS
      );
    }

    return true;
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
    callback: () => void
  ): boolean {
    if (SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.connected) {
      SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.emit(
        SERVER_CONSTANTS.LAUNCH_MISSION_ADDRESS,
        isMissionSimulated,
        callback
      );
    }

    return SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.connected;
  }

  public static setP2PGradient(newValue: boolean): boolean {
    if (SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.connected) {
      SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.emit(
        SERVER_CONSTANTS.SET_P2P_ADDRESS,
        newValue
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

  public static terminateMission(maps: string, callback: () => void): boolean {
    if (SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.connected) {
      SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.emit(
        SERVER_CONSTANTS.TERMINATE_MISSION_ADDRESS,
        maps,
        callback
      );
    }

    return SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.connected;
  }

  public static setDrone(
    drone: NewDroneData[],
    isMissionSimulated: boolean
  ): void {
    if (SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.connected) {
      SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.emit(
        SERVER_CONSTANTS.SET_DRONE_ADDRESS,
        drone,
        isMissionSimulated
      );
    }
  }

  public static getCompletedMissions(): Promise<Response> {
    return fetch(SERVER_CONSTANTS.GET_COMPLETED_MISSIONS);
  }

  public static getSpecificMissionLogs(id: string): Promise<Response> {
    return fetch(SERVER_CONSTANTS.GET_SPECIFIC_MISSION_LOGS + '/' + id);
  }

  public static getSpecificMissionMaps(id: string): Promise<Response> {
    return fetch(SERVER_CONSTANTS.GET_SPECIFIC_MISSION_MAPS + '/' + id);
  }
}
