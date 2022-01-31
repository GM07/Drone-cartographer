import {MissionStatus} from '@/communication/mission_status';
import Vue from 'vue';
import Vuex from 'vuex';

import {useAccessor, getterTree, mutationTree, actionTree} from 'typed-vuex';

Vue.use(Vuex);

const STATE = () => ({
  isConnected: false,
  missionStatus: {
    isMissionStarted: false,
    isMissionSimulated: false,
  } as MissionStatus,
});

const GETTERS = getterTree(STATE, {});

const MUTATIONS = mutationTree(STATE, {
  setConnectionState(state, isConnected: boolean) {
    state.isConnected = isConnected;
  },

  setMissionStatus(state, missionStatus: MissionStatus) {
    state.missionStatus = missionStatus;
  },
});

const ACTIONS = actionTree(
  {state: STATE, getters: GETTERS, mutations: MUTATIONS},
  {
    // socket-specific methods
    /* eslint-disable @typescript-eslint/naming-convention */
    socket_mission_status(state, missionStatus) {
      state.commit('setMissionStatus', missionStatus as MissionStatus);
    },

    socket_connect(state) {
      state.commit('setConnectionState', true);
    },

    socket_disconnect(state) {
      state.commit('setConnectionState', false);
    },

    socket_error(state) {
      state.commit('setConnectionState', false);
    },

    socket_connect_error(state) {
      state.commit('setConnectionState', false);
    },

    socket_connect_timeout(state) {
      state.commit('setConnectionState', false);
    },

    socket_reconnect_error(state) {
      state.commit('setConnectionState', false);
    },

    socket_reconnect_failed(state) {
      state.commit('setConnectionState', false);
    },
    /* eslint-enable @typescript-eslint/naming-convention */
  }
);

const STORE_PATTERN = {
  state: STATE,
  getters: GETTERS,
  mutations: MUTATIONS,
  actions: ACTIONS,
};

export const STORE = new Vuex.Store(STORE_PATTERN);
export const ACCESSOR = useAccessor(STORE, STORE_PATTERN);

Vue.prototype.$accessor = ACCESSOR;
