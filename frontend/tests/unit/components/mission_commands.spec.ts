import {shallowMount, Wrapper} from '@vue/test-utils';
import {DefaultProps} from 'vue/types/options';
import MissionCommands from '@/components/mission_commands.vue';
import {ServerCommunication} from '@/communication/server_communication';
import {STORE} from '@/store';
import {STORE_STUB} from '../stubs/store_stub';
import * as SERVER_CONSTANTS from '@/communication/server_constants';
import SocketIO from 'socket.io-client';
import {Server} from 'socket.io';
import {createServer} from 'http';

describe('mission_commands.vue', () => {
  let wrapper: Wrapper<DefaultProps & MissionCommands, Element>;

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
  /* eslint-enable @typescript-eslint/no-explicit-any */
  /* eslint-enable @typescript-eslint/no-unused-vars */

  beforeEach(() => {
    wrapper = shallowMount<MissionCommands>(MissionCommands, {
      store: STORE_STUB,
      propsData: {
        accessStatus: {
          isMissionSimulated: false,
          isUserControlling: false,
        },
        maps: [{} as HTMLCanvasElement],
      },
    });
  });

  it('should create', () => {
    expect(wrapper).toBeTruthy();
  });

  it('change the mission mode', () => {
    const COMMUNICATION_SPY = spyOn(ServerCommunication, 'setMissionType');

    STORE.commit('setMissionStatus', {
      isMissionStarted: true,
      isSomeoneControlling: true,
    });
    wrapper.vm.simulatedMission = true;
    expect(COMMUNICATION_SPY).not.toHaveBeenCalled();

    STORE.commit('setMissionStatus', {
      isMissionStarted: false,
      isSomeoneControlling: false,
    });
    wrapper.vm.simulatedMission = true;
    expect(COMMUNICATION_SPY).toHaveBeenCalled();
  });

  it('should launch mission', () => {
    const COMMUNICATION_SPY = spyOn(ServerCommunication, 'launchMission');
    STORE.commit('setMissionStatus', {
      isMissionStarted: true,
      isSomeoneControlling: true,
    });
    wrapper.vm.launchMission();
    expect(COMMUNICATION_SPY).not.toHaveBeenCalled();

    STORE.commit('setMissionStatus', {
      isMissionStarted: false,
      isSomeoneControlling: false,
    });
    COMMUNICATION_SPY.and.returnValue(false);
    wrapper.vm.launchMission();
    expect(wrapper.vm.isLaunchMissionSelected).toBe(false);
    expect(COMMUNICATION_SPY).toHaveBeenCalled();

    COMMUNICATION_SPY.and.callFake((arg1, callback) => {
      callback();
      return true;
    });
    wrapper.vm.launchMission();
    expect(wrapper.vm.isLaunchMissionSelected).toBe(false);
  });

  it('should return to base', () => {
    const COMMUNICATION_SPY = spyOn(ServerCommunication, 'returnToBase');
    STORE.commit('setMissionStatus', {
      isMissionStarted: false,
      isSomeoneControlling: false,
    });
    wrapper.vm.returnToBase();
    expect(COMMUNICATION_SPY).not.toHaveBeenCalled();

    STORE.commit('setMissionStatus', {
      isMissionStarted: true,
      isSomeoneControlling: true,
    });
    COMMUNICATION_SPY.and.returnValue(false);
    wrapper.vm.returnToBase();
    expect(wrapper.vm.isReturnToBaseSelected).toBe(false);
    expect(COMMUNICATION_SPY).toHaveBeenCalled();

    COMMUNICATION_SPY.and.callFake(callback => {
      callback();
      return true;
    });
    wrapper.vm.returnToBase();
    expect(wrapper.vm.isReturnToBaseSelected).toBe(false);
  });

  it('should send p2p gradient to base', () => {
    const P2P_SPY = spyOn(ServerCommunication, 'setP2PGradient');
    STORE.commit('setMissionStatus', {
      isMissionStarted: false,
      isSomeoneControlling: false,
    });
    wrapper.vm.setP2PGradient(true);
    expect(P2P_SPY).not.toHaveBeenCalled();

    STORE.commit('setMissionStatus', {
      isMissionStarted: true,
      isSomeoneControlling: true,
    });
    wrapper.vm.setP2PGradient(true);
    expect(P2P_SPY).toHaveBeenCalled();
  });

  it('should terminate the mission', () => {
    const COMMUNICATION_SPY = spyOn(ServerCommunication, 'terminateMission');
    wrapper.vm['maps'][0] = document.createElement(
      'canvas'
    ) as HTMLCanvasElement;
    const CANVAS_SPY = spyOn(wrapper.vm['maps'][0], 'toDataURL');
    STORE.commit('setMissionStatus', {
      isMissionStarted: false,
      isSomeoneControlling: false,
    });
    wrapper.vm.terminateMission();
    expect(COMMUNICATION_SPY).not.toHaveBeenCalled();

    STORE.commit('setMissionStatus', {
      isMissionStarted: true,
      isSomeoneControlling: true,
    });
    COMMUNICATION_SPY.and.returnValue(false);
    wrapper.vm.terminateMission();
    expect(wrapper.vm.isTerminateMissionSelected).toBe(false);
    expect(COMMUNICATION_SPY).toHaveBeenCalled();
    expect(CANVAS_SPY).toHaveBeenCalled();

    COMMUNICATION_SPY.and.callFake((a, callback) => {
      callback();
      return true;
    });
    wrapper.vm.terminateMission();
    expect(wrapper.vm.isTerminateMissionSelected).toBe(false);
  });

  it('should destroy', () => {
    const SPY = spyOn(
      SERVER_CONSTANTS.SOCKETIO_LIMITED_ACCESS,
      'removeListener'
    );

    wrapper.destroy();
    expect(SPY).toHaveBeenCalled();
  });

  it('should update p2p gradient', done => {
    Object.defineProperty(SERVER_CONSTANTS, 'SOCKETIO_LIMITED_ACCESS', {
      value: clientSocket,
    });

    const TEST_WRAPPER = shallowMount<MissionCommands>(MissionCommands, {
      store: STORE_STUB,
      propsData: {
        accessStatus: {
          isMissionSimulated: false,
          isUserControlling: false,
        },
      },
    });

    TEST_WRAPPER.vm.isP2PGradientRunning = false;
    serverSocket.emit(SERVER_CONSTANTS.UPDATE_P2P_GRADIENT, true);
    setTimeout(() => {
      expect(TEST_WRAPPER.vm.isP2PGradientRunning).toBe(true);
      done();
    }, 100);
  });
});
