import * as SERVER_CONSTANTS from '@/communication/server_constants';

export class ServerCommunication {
  public static getConnectedDrones(): Promise<Response> {
    return fetch(SERVER_CONSTANTS.GET_DRONES_ADDRESS);
  }

  public static identifyDrone(droneId: string): Promise<Response> {
    return fetch(SERVER_CONSTANTS.IDENTIFY_DRONE_ADDRESS, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify(droneId),
    });
  }

  public static launchMission(): Promise<Response> {
    return fetch(SERVER_CONSTANTS.LAUNCH_MISSION_ADDRESS);
  }

  public static terminateMission(): Promise<Response> {
    return fetch(SERVER_CONSTANTS.TERMINATE_MISSION_ADDRESS);
  }

  public static specifyMissionType(isMissionSimulated: boolean): Promise<Response> {
    return fetch(SERVER_CONSTANTS.SPECIFY_MISSION_TYPE_ADDRESS, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify(isMissionSimulated),
    });
  }
}
