import Mission from '@/views/mission.vue';
import {ROUTER} from '@/router';
import {shallowMount, Wrapper} from '@vue/test-utils';
import {DefaultProps} from 'vue/types/options';
import {STORE_STUB} from '../stubs/store_stub';
import * as SERVER_CONSTANTS from '@/communication/server_constants';
import fetchMock from 'jest-fetch-mock';
import Vuetify from 'vuetify';
import SocketIO from 'socket.io-client';
import {Server} from 'socket.io';
import {createServer} from 'http';
import {
  DEFAULT_DRONE_DATA,
  DroneData,
  NewDroneData,
} from '@/communication/drone';

describe('mission.vue', () => {
  let limitedAccessEmitSpy: jasmine.Spy;
  let routerSpy: jasmine.Spy;
  let wrapper: Wrapper<DefaultProps & Mission, Element>;
  const STUBS = [
    'DroneCommands',
    'MissionCommands',
    'NavigationCommands',
    'DroneMenu',
    'Map',
    'LogsInterface',
    'DroneDataCard',
    'RemoteCommandOutput',
    'Editor',
  ];
  const VUETIFY = new Vuetify();
  const DEFAULT_DRONE_STATUS = {...DEFAULT_DRONE_DATA} as DroneData;
  DEFAULT_DRONE_STATUS.name = 'a';

  /* eslint-disable @typescript-eslint/no-explicit-any */
  /* eslint-disable @typescript-eslint/no-unused-vars */
  let io: any, serverSocket: any, clientSocket: any;
  beforeAll(done => {
    const HTTP = createServer();
    io = new Server(HTTP);
    HTTP.listen(() => {
      const PORT = (HTTP.address() as any).port;
      clientSocket = SocketIO(`http://localhost:${PORT}`);
      io.on('connection', (socket: any) => {
        serverSocket = socket;
      });
      clientSocket.on('connect', done);
    });
  });

  afterAll(() => {
    io.close();
    clientSocket.close();
  });

  beforeEach(() => {
    limitedAccessEmitSpy = spyOn(
      SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS,
      'emit'
    );
    routerSpy = spyOn(ROUTER, 'push');
    SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.connected = false;
    fetchMock.resetMocks();
    fetchMock.mockResponse(JSON.stringify(''), {
      status: SERVER_CONSTANTS.HTTP_OK,
    });

    wrapper = shallowMount<Mission>(Mission, {
      store: STORE_STUB,
      vuetify: VUETIFY,
      stubs: STUBS,
    });
  });
  /* eslint-enable @typescript-eslint/no-explicit-any */
  /* eslint-enable @typescript-eslint/no-unused-vars */

  it('should create', () => {
    expect(wrapper).toBeTruthy();
  });

  it('should set selected drone', () => {
    wrapper.vm.setSelected(-1);
    expect(wrapper.vm['chosenOption']).toEqual(-1);
  });

  it('should delete a drone', () => {
    wrapper.vm['chosenOption'] = 100;
    wrapper.vm.droneList = [DEFAULT_DRONE_STATUS, DEFAULT_DRONE_STATUS];
    expect(wrapper.vm.droneList.length).toEqual(2);
    wrapper.vm.deleteDrone(1);
    expect(wrapper.vm.droneList.length).toEqual(1);
  });

  it('should add a drone', () => {
    wrapper.vm.droneList = [DEFAULT_DRONE_STATUS];
    expect(wrapper.vm.droneList.length).toEqual(1);
    wrapper.vm.addDrone(DEFAULT_DRONE_STATUS as NewDroneData);
    expect(wrapper.vm.droneList.length).toEqual(2);
  });

  it('should tell if drone commands are enabled', () => {
    wrapper.vm['chosenOption'] = 1;
    wrapper.vm.accessStatus.isUserControlling = true;
    expect(wrapper.vm.isDroneCommandsEnabled()).toEqual(true);
  });

  it('should set the view of the drone menu', () => {
    wrapper.vm.isDroneMenuOpen = true;
    wrapper.vm.setDroneMenuOpen(false);
    expect(wrapper.vm.isDroneMenuOpen).toBe(false);
  });

  it('should tell if user is controlling', () => {
    wrapper.vm.accessStatus.isUserControlling = true;
    expect(wrapper.vm.isUserControlling()).toEqual(true);
    wrapper.vm.accessStatus.isUserControlling = false;
    expect(wrapper.vm.isUserControlling()).toEqual(false);
  });

  it('should get selected drone name', () => {
    wrapper.vm['chosenOption'] = -1;
    expect(wrapper.vm.getSelectedDrone()).toEqual('');
    const EXPECTED_DRONE = {
      ...DEFAULT_DRONE_DATA,
    } as DroneData;
    EXPECTED_DRONE.name = 'expected';
    wrapper.vm.droneList[0] = EXPECTED_DRONE;
    wrapper.vm['chosenOption'] = 0;
    expect(wrapper.vm.getSelectedDrone()).toEqual(EXPECTED_DRONE.name);
  });

  it('should tell if connected', () => {
    expect(wrapper.vm.isConnected()).toBe(false);
    SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.connected = true;
    expect(wrapper.vm.isConnected()).toBe(true);
  });

  it('should switch editor status', () => {
    wrapper.vm.editorMode = false;
    wrapper.vm.switchEditorStatus();
    expect(wrapper.vm.editorMode).toBe(true);
  });

  it('should update status', done => {
    SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.removeAllListeners().close();

    Object.defineProperty(SERVER_CONSTANTS, 'SOCKETIO_LIMITED_ACCESS', {
      value: clientSocket,
    });

    const TEST_WRAPPER = shallowMount<Mission>(Mission, {
      store: STORE_STUB,
      vuetify: VUETIFY,
      stubs: STUBS,
    });

    serverSocket.emit('droneList', [DEFAULT_DRONE_STATUS]);

    serverSocket.emit(
      'update_status',
      {
        isMissionSimulated: true,
        isUserControlling: true,
      },
      () => {
        expect(TEST_WRAPPER.vm.accessStatus.isMissionSimulated).toBe(true);
        done();
      }
    );
  });

  it('should update drone status', done => {
    const FAKE_DRONE_DATA = {
      name: 'fakeName',
      xPos: 0,
    } as DroneData;
    SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.removeAllListeners().close();

    Object.defineProperty(SERVER_CONSTANTS, 'SOCKETIO_DRONE_STATUS', {
      value: clientSocket,
    });

    const TEST_WRAPPER = shallowMount<Mission>(Mission, {
      store: STORE_STUB,
      vuetify: VUETIFY,
      stubs: STUBS,
    });

    TEST_WRAPPER.vm.droneList = [FAKE_DRONE_DATA];
    serverSocket.emit('update_drone_status', [
      {
        ...FAKE_DRONE_DATA,
        name: 'wrongName',
      },
    ]);
    setTimeout(() => {
      expect(TEST_WRAPPER.vm.droneList[0].name).toEqual(FAKE_DRONE_DATA.name);
      serverSocket.emit('update_drone_status', [{...FAKE_DRONE_DATA, xPos: 1}]);
      setTimeout(() => {
        expect(TEST_WRAPPER.vm.droneList[0].xPos).toEqual(1);
        done();
      }, 100);
    }, 100);
  });

  it('should disconnect', () => {
    const LISTENER =
      SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS.listeners('disconnect')[0];
    // eslint-disable-next-line @typescript-eslint/no-explicit-any
    LISTENER({} as any);
  });

  it('should destroy', () => {
    const SPY = spyOn(SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS, 'close');

    Object.defineProperty(SERVER_CONSTANTS, 'SOCKETIO_LIMITED_ACCESS', {
      value: clientSocket,
    });

    const TEST_WRAPPER = shallowMount<Mission>(Mission, {
      store: STORE_STUB,
      vuetify: VUETIFY,
      stubs: STUBS,
    });

    serverSocket.disconnect();
    clientSocket.close();
    TEST_WRAPPER.destroy();
    expect(SPY).toHaveBeenCalled();
  });
});
