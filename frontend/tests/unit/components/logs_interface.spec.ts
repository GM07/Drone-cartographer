import LogsInterface from '@/components/logs_interface.vue';
import {shallowMount, Wrapper} from '@vue/test-utils';
import {DefaultProps} from 'vue/types/options';
import * as SERVER_CONSTANTS from '@/communication/server_constants';
import SocketIO from 'socket.io-client';
import {Server} from 'socket.io';
import {createServer} from 'http';

describe('logs_interface.vue', () => {
  let socketSpy: jasmine.Spy;
  let nextTickSpy: jasmine.Spy;
  let wrapper: Wrapper<DefaultProps & LogsInterface, Element>;

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
    socketSpy = spyOn(SERVER_CONSTANTS.SOCKETIO_GET_LOGS, 'emit');
    SERVER_CONSTANTS.SOCKETIO_GET_LOGS.connected = false;
    wrapper = shallowMount<LogsInterface>(LogsInterface);
    nextTickSpy = spyOn(wrapper.vm, '$nextTick').and.callFake(callback =>
      callback()
    );
  });

  it('should create', () => {
    expect(wrapper).toBeTruthy();
  });

  it('should get logs', done => {
    SERVER_CONSTANTS.SOCKETIO_GET_LOGS.removeAllListeners().close();

    Object.defineProperty(SERVER_CONSTANTS, 'SOCKETIO_GET_LOGS', {
      value: clientSocket,
    });

    const TEST_WRAPPER = shallowMount<LogsInterface>(LogsInterface);
    const PUSH_SPY = spyOn(TEST_WRAPPER.vm['logs'], 'push').and.callThrough();
    const SCROLL_SPY = spyOn(document, 'getElementById').and.returnValue(null);

    serverSocket.emit('get_logs', [['a', 'b']]);
    setTimeout(() => {
      expect(PUSH_SPY).toHaveBeenCalled();
      expect(nextTickSpy).not.toHaveBeenCalled();
      SCROLL_SPY.and.returnValue({
        scrollTop: 1,
        scrollHeight: 1,
      });
      serverSocket.emit('get_logs', [['a', 'b']]);
      setTimeout(() => {
        SCROLL_SPY.and.returnValue({
          scrollTop: 1,
          scrollHeight: 10000,
        });
        serverSocket.emit('get_logs', [['a', 'b']]);
        setTimeout(() => {
          done();
        }, 100);
      }, 100);
    }, 100);
  });

  it('should destroy', () => {
    const SPY = spyOn(SERVER_CONSTANTS.SOCKETIO_GET_LOGS, 'close');

    Object.defineProperty(SERVER_CONSTANTS, 'SOCKETIO_GET_LOGS', {
      value: clientSocket,
    });

    const TEST_WRAPPER = shallowMount<LogsInterface>(LogsInterface);

    serverSocket.disconnect();
    clientSocket.close();
    TEST_WRAPPER.destroy();
    expect(SPY).toHaveBeenCalled();
  });
});
