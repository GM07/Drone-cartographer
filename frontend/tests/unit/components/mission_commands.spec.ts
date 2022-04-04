import {shallowMount, Wrapper} from '@vue/test-utils';
import {DefaultProps} from 'vue/types/options';
import MissionCommands from '@/components/mission_commands.vue';
import {ServerCommunication} from '@/communication/server_communication';
import {STORE} from '@/store';
import {STORE_STUB} from '../stubs/store_stub';

describe('mission_commands.vue', () => {
  let wrapper: Wrapper<DefaultProps & MissionCommands, Element>;

  beforeEach(() => {
    wrapper = shallowMount<MissionCommands>(MissionCommands, {
      store: STORE_STUB,
      propsData: {
        droneList: ['a'],
        accessStatus: {
          isMissionSimulated: false,
          isUserControlling: false,
        },
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

  it('should terminate the mission', () => {
    const COMMUNICATION_SPY = spyOn(ServerCommunication, 'terminateMission');
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

    COMMUNICATION_SPY.and.callFake(callback => {
      callback();
      return true;
    });
    wrapper.vm.terminateMission();
    expect(wrapper.vm.isTerminateMissionSelected).toBe(false);
  });
});
