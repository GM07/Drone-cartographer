import {MissionStatus} from '@/communication/mission_status';
import {STORE, ACCESSOR} from '@/store/index';

describe('store index.vue', () => {
  beforeEach(() => {
    STORE.commit('setConnectionState', false);
    STORE.commit('setMissionStatus', {
      isMissionStarted: false,
      isSomeoneControlling: false,
    } as MissionStatus);
  });

  it('should create', () => {
    expect(STORE).toBeTruthy();
    expect(ACCESSOR).toBeTruthy();
  });

  it('should set connection state', () => {
    ACCESSOR.setConnectionState(true);
    expect(ACCESSOR.isConnected).toBe(true);
  });

  it('should set mission status', () => {
    const EXPECTED_STATUS = {
      isMissionStarted: true,
      isSomeoneControlling: true,
    } as MissionStatus;
    ACCESSOR.setMissionStatus(EXPECTED_STATUS);
    expect(ACCESSOR.missionStatus).toEqual(EXPECTED_STATUS);
  });

  it('should update status on event', async () => {
    const EXPECTED_STATUS = {
      isMissionStarted: true,
      isSomeoneControlling: true,
    } as MissionStatus;
    await STORE.dispatch('socket_update_status', EXPECTED_STATUS);
    expect(ACCESSOR.missionStatus).toEqual(EXPECTED_STATUS);
  });

  it('should connect on connection event', async () => {
    await STORE.dispatch('socket_connect');
    expect(ACCESSOR.isConnected).toBe(true);
  });

  it('should disconnect on socket disconnect event', async () => {
    STORE.commit('setConnectionState', true);
    await STORE.dispatch('socket_disconnect');
    expect(ACCESSOR.isConnected).toBe(false);
  });

  it('should disconnect on socket error event', async () => {
    STORE.commit('setConnectionState', true);
    await STORE.dispatch('socket_error');
    expect(ACCESSOR.isConnected).toBe(false);
  });

  it('should disconnect on socket connect error event', async () => {
    STORE.commit('setConnectionState', true);
    await STORE.dispatch('socket_connect_error');
    expect(ACCESSOR.isConnected).toBe(false);
  });

  it('should disconnect on socket timeout event', async () => {
    STORE.commit('setConnectionState', true);
    await STORE.dispatch('socket_connect_timeout');
    expect(ACCESSOR.isConnected).toBe(false);
  });

  it('should disconnect on reconnect error event', async () => {
    STORE.commit('setConnectionState', true);
    await STORE.dispatch('socket_reconnect_error');
    expect(ACCESSOR.isConnected).toBe(false);
  });

  it('should disconnect on socket reconnect failed event', async () => {
    STORE.commit('setConnectionState', true);
    await STORE.dispatch('socket_reconnect_failed');
    expect(ACCESSOR.isConnected).toBe(false);
  });
});
