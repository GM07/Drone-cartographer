import {ServerCommunication} from '@/communication/server_communication';
import * as SERVER_CONSTANTS from '@/communication/server_constants';
import fetchMock from 'jest-fetch-mock';

describe('Communication.ts', () => {
  beforeEach(() => {
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

  it('Should identify a drone', async () => {
    const INPUT = 'droneid1';

    fetchMock.mockResponse('', {status: SERVER_CONSTANTS.HTTP_OK});
    const RESPONSE = await ServerCommunication.identifyDrone(INPUT);
    expect(RESPONSE.status).toEqual(SERVER_CONSTANTS.HTTP_OK);
  });
});
