import {mount, shallowMount, Wrapper} from '@vue/test-utils';
import {DefaultProps} from 'vue/types/options';
import SocketIO from 'socket.io-client';
import {Server} from 'socket.io';
import {createServer} from 'http';
import Map from '@/components/map.vue';
import {DEFAULT_DRONE_DATA, DroneData} from '@/communication/drone';
import * as SERVER_CONSTANTS from '@/communication/server_constants';
import {MapData, Sensors} from '@/utils/map_constants';
import {Vec2d} from '@/utils/vec2d';

describe('map.vue', () => {
  let canvasTest: HTMLCanvasElement;
  let wrapper: Wrapper<DefaultProps & Map, Element>;
  let defaultMapData: MapData;

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
    canvasTest = document.createElement('canvas') as HTMLCanvasElement;
    canvasTest.width = 10;
    canvasTest.height = 10;

    defaultMapData = {
      id: '',
      position: [0, 0],
      sensors: {
        front: 0,
        right: 0,
        back: 0,
        left: 0,
      } as Sensors,
    } as MapData;

    wrapper = mount<Map>(Map, {
      propsData: {
        maps: [canvasTest],
        indexDrone: 0,
        droneList: [{...DEFAULT_DRONE_DATA} as DroneData],
      },
    });

    const CANVAS = (wrapper.vm.$refs.canvas as HTMLCanvasElement).getContext(
      '2d'
    ) as CanvasRenderingContext2D;
    spyOn(CANVAS, 'drawImage');
  });

  it('should create', () => {
    expect(wrapper).toBeTruthy();
  });

  it('should react on index change', async () => {
    wrapper.vm['hasBeenMounted'] = true;
    await wrapper.setProps({indexDrone: 1});
    const CREATE_SPY = spyOn(wrapper.vm, 'createMaps').and.callFake(
      async () => {
        await wrapper.setProps({maps: [canvasTest]});
      }
    );
    wrapper.vm.onIndexChange();
    expect(CREATE_SPY).not.toHaveBeenCalled();

    await wrapper.setProps({maps: [], indexDrone: 0});
    wrapper.vm.onIndexChange();
    expect(CREATE_SPY).toHaveBeenCalled();
  });

  it('should change map', async () => {
    await wrapper.setProps({maps: [], droneList: []});
    const CREATE_SPY = spyOn(wrapper.vm, 'createMaps');
    wrapper.vm.onMapChange({} as MapData, 0);
    expect(CREATE_SPY).toHaveBeenCalled();

    await wrapper.setProps({
      maps: [canvasTest],
      droneList: [{...DEFAULT_DRONE_DATA} as DroneData],
    });
    spyOn(wrapper.vm, 'findDroneIdx').and.returnValue(1);
    const CTX_SPY = spyOn(
      wrapper.vm['maps'][0],
      'getContext'
    ).and.callThrough();
    wrapper.vm.onMapChange(defaultMapData, 0);
    expect(CTX_SPY).toHaveBeenCalledTimes(2);

    CTX_SPY.calls.reset();
    defaultMapData.sensors.back = 1;
    defaultMapData.sensors.front = 1;
    defaultMapData.sensors.right = 1;
    defaultMapData.sensors.left = 1;
    wrapper.vm.onMapChange(defaultMapData, 0);
    expect(CTX_SPY).toHaveBeenCalledTimes(10);
  });

  it('should find drone index', () => {
    expect(wrapper.vm.findDroneIdx({id: ''} as MapData)).toEqual(1);
  });

  it('should create maps', async () => {
    await wrapper.setProps({maps: []});
    const CREATE_SPY = spyOn(document, 'createElement').and.returnValue(
      canvasTest
    );
    wrapper.vm.createMaps();
    expect(CREATE_SPY).toHaveBeenCalled();
    expect(wrapper.vm['maps'].length).toEqual(2);
  });

  it('should draw on canvas', () => {
    const DRAW_SPY = spyOn(wrapper.vm, 'onMapChange');
    wrapper.vm.drawOnCanvas({} as MapData);
    expect(DRAW_SPY).toHaveBeenCalledTimes(2);
  });

  it('should convert to canvas pos', () => {
    wrapper.vm['canvasSize'] = new Vec2d(1, 1);
    wrapper.vm['mapSize'] = new Vec2d(1, 1);
    expect(wrapper.vm.toCanvasPos(new Vec2d(1, 1))).toEqual(new Vec2d(2, 0));
  });

  it('should get map data', done => {
    Object.defineProperty(SERVER_CONSTANTS, 'SOCKETIO_MAP_DATA', {
      value: clientSocket,
    });

    const TEST_WRAPPER = shallowMount<Map>(Map, {
      propsData: {
        maps: [canvasTest],
        indexDrone: -1,
        droneList: [{...DEFAULT_DRONE_DATA} as DroneData],
      },
    });

    const DRAW_SPY = spyOn(TEST_WRAPPER.vm, 'drawOnCanvas');
    serverSocket.emit('getMapData', [{} as MapData]);
    setTimeout(() => {
      expect(DRAW_SPY).toHaveBeenCalled();
      done();
    }, 200);
  });

  it('should clear all maps', done => {
    Object.defineProperty(SERVER_CONSTANTS, 'SOCKETIO_MAP_DATA', {
      value: clientSocket,
    });

    const TEST_WRAPPER = shallowMount<Map>(Map, {
      propsData: {
        maps: [canvasTest],
        indexDrone: -1,
        droneList: [{...DEFAULT_DRONE_DATA} as DroneData],
      },
    });
    const GET_CONTEXT_SPY = spyOn(
      TEST_WRAPPER.vm['maps'][0],
      'getContext'
    ).and.callThrough();

    serverSocket.emit('clear_all_maps', false);
    setTimeout(() => {
      expect(GET_CONTEXT_SPY).not.toHaveBeenCalled();
      serverSocket.emit('clear_all_maps', true);
      setTimeout(() => {
        expect(GET_CONTEXT_SPY);
        done();
      }, 200);
    }, 200);
  });

  it('should destroy', () => {
    Object.defineProperty(SERVER_CONSTANTS, 'SOCKETIO_MAP_DATA', {
      value: clientSocket,
    });

    const TEST_WRAPPER = shallowMount<Map>(Map, {
      propsData: {
        maps: [canvasTest],
        indexDrone: -1,
        droneList: [{...DEFAULT_DRONE_DATA} as DroneData],
      },
    });
    TEST_WRAPPER.destroy();
    expect(clientSocket.connected).toEqual(false);
  });
});
