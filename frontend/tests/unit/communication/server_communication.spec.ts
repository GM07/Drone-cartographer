import {ServerCommunication} from '@/communication/server_communication';
import * as SERVER_CONSTANTS from '@/communication/server_constants';
import fetchMock from 'jest-fetch-mock';

describe('Communication.ts', () => {
  let limitedAccessEmitSpy: jasmine.Spy;

  beforeEach(() => {
    limitedAccessEmitSpy = spyOn(
      SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS,
      'emit'
    );
    SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.connected = false;
    fetchMock.resetMocks();
  });

  it('Should get the current drones', async () => {
    const MESSAGE = ['droneid1', 'droneid2', 'droneid2'];
    const EXPECTED_CALL = [
      SERVER_CONSTANTS.GET_DRONES_ADDRESS,
      {
        method: 'GET',
      },
    ];

    fetchMock.mockResponse(JSON.stringify(MESSAGE), {
      status: SERVER_CONSTANTS.HTTP_OK,
    });
    const RESPONSE = await ServerCommunication.getConnectedDrones();
    const OBTAINED_VALUES = await RESPONSE.json();
    expect(fetchMock).toHaveBeenCalledWith(...EXPECTED_CALL);
    expect(OBTAINED_VALUES).toEqual(MESSAGE);
    expect(RESPONSE.status).toEqual(SERVER_CONSTANTS.HTTP_OK);
  });

  it('should identify', () => {
    let response = ServerCommunication.identifyDrone('');
    expect(response).toBe(false);
    expect(limitedAccessEmitSpy).not.toHaveBeenCalled();
    SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.connected = true;
    response = ServerCommunication.identifyDrone('');
    expect(response).toBe(true);
    expect(limitedAccessEmitSpy).toHaveBeenCalled();
  });

  it('should take mission control', () => {
    let response = ServerCommunication.takeMissionControl();
    expect(response).toBe(false);
    expect(limitedAccessEmitSpy).not.toHaveBeenCalled();
    SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.connected = true;
    response = ServerCommunication.takeMissionControl();
    expect(response).toBe(true);
    expect(limitedAccessEmitSpy).toHaveBeenCalled();
  });

  it('should take mission control with a timeout', () => {
    const TIMEOUT_SPY = spyOn(
      SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS,
      'timeout'
    ).and.callThrough();
    ServerCommunication.takeMissionControlTimeout(1, () => {});
    expect(limitedAccessEmitSpy).toHaveBeenCalled();
    expect(TIMEOUT_SPY).toHaveBeenCalled();
  });

  it('should revoke mission control', () => {
    let response = ServerCommunication.revokeMissionControl();
    expect(response).toBe(false);
    expect(limitedAccessEmitSpy).not.toHaveBeenCalled();
    SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.connected = true;
    response = ServerCommunication.revokeMissionControl();
    expect(response).toBe(true);
    expect(limitedAccessEmitSpy).toHaveBeenCalled();
  });

  it('should set mission type', () => {
    let response = ServerCommunication.setMissionType(false);
    expect(response).toBe(false);
    expect(limitedAccessEmitSpy).not.toHaveBeenCalled();
    SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.connected = true;
    response = ServerCommunication.setMissionType(false);
    expect(response).toBe(true);
    expect(limitedAccessEmitSpy).toHaveBeenCalled();
  });

  it('should launch a mission', () => {
    let response = ServerCommunication.launchMission(false, () => {});
    expect(response).toBe(false);
    expect(limitedAccessEmitSpy).not.toHaveBeenCalled();
    SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.connected = true;
    response = ServerCommunication.launchMission(false, () => {});
    expect(response).toBe(true);
    expect(limitedAccessEmitSpy).toHaveBeenCalled();
  });

  it('should return to base', () => {
    let response = ServerCommunication.returnToBase(() => {});
    expect(response).toBe(false);
    expect(limitedAccessEmitSpy).not.toHaveBeenCalled();
    SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.connected = true;
    response = ServerCommunication.returnToBase(() => {});
    expect(response).toBe(true);
    expect(limitedAccessEmitSpy).toHaveBeenCalled();
  });

  it('should terminates a mission', () => {
    let response = ServerCommunication.terminateMission(() => {});
    expect(response).toBe(false);
    expect(limitedAccessEmitSpy).not.toHaveBeenCalled();
    SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.connected = true;
    response = ServerCommunication.terminateMission(() => {});
    expect(response).toBe(true);
    expect(limitedAccessEmitSpy).toHaveBeenCalled();
  });

  it('should set drones', () => {
    ServerCommunication.setDrone([], true);
    expect(limitedAccessEmitSpy).not.toHaveBeenCalled();
    SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.connected = true;
    ServerCommunication.setDrone([], true);
    expect(limitedAccessEmitSpy).toHaveBeenCalled();
  });

  it('should get completed missions', async () => {
    fetchMock.mockResponse(JSON.stringify(''), {
      status: SERVER_CONSTANTS.HTTP_OK,
    });
    await ServerCommunication.getCompletedMissions();
    expect(fetchMock).toHaveBeenCalledWith(
      SERVER_CONSTANTS.GET_COMPLETED_MISSIONS
    );
  });

  it('should get specific mission', async () => {
    fetchMock.mockResponse(JSON.stringify(''), {
      status: SERVER_CONSTANTS.HTTP_OK,
    });
    await ServerCommunication.getSpecificMission('id');
    expect(fetchMock).toHaveBeenCalledWith(
      SERVER_CONSTANTS.GET_SPECIFIC_MISSION + '/' + 'id'
    );
  });
});
