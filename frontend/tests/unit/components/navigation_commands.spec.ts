import {shallowMount, Wrapper} from '@vue/test-utils';
import {DefaultProps} from 'vue/types/options';
import NavigationCommands from '@/components/navigation_commands.vue';
import {ServerCommunication} from '@/communication/server_communication';
import {ROUTER} from '@/router';
import {STORE} from '@/store';

describe('navigation_commands.vue', () => {
  let wrapper: Wrapper<DefaultProps & NavigationCommands, Element>;

  beforeEach(() => {
    spyOn(ServerCommunication, 'takeMissionControlTimeout').and.callFake(
      (timeout, callback) => {
        callback();
      }
    );
    wrapper = shallowMount<NavigationCommands>(NavigationCommands, {
      propsData: {
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

  it('should set limited connection', () => {
    wrapper.vm['attemptedLimitedConnection'] = false;
    wrapper.vm.setLimitedConnection(true);
    expect(wrapper.vm['attemptedLimitedConnection']).toBe(true);
  });

  it('should tell if the user can revoke the control', () => {
    expect(wrapper.vm.canRevokeControl()).toBe(false);
  });

  it('should return to main menu', () => {
    const ROUTER_SPY = spyOn(ROUTER, 'push');
    wrapper.vm.returnToMainMenu();
    expect(ROUTER_SPY).toHaveBeenCalled();
  });

  it('should take mission control', () => {
    const COMMUNICATION_SPY = spyOn(ServerCommunication, 'takeMissionControl');
    STORE.commit('setMissionStatus', {
      isMissionStarted: true,
      isSomeoneControlling: true,
    });
    wrapper.vm.takeMissionControl();
    expect(COMMUNICATION_SPY).not.toHaveBeenCalled();

    STORE.commit('setMissionStatus', {
      isMissionStarted: false,
      isSomeoneControlling: false,
    });
    wrapper.vm.takeMissionControl();
    expect(COMMUNICATION_SPY).toHaveBeenCalled();
  });

  it('should revoke mission control', async () => {
    const COMMUNICATION_SPY = spyOn(
      ServerCommunication,
      'revokeMissionControl'
    );
    wrapper.vm.revokeMissionControl();
    expect(COMMUNICATION_SPY).not.toHaveBeenCalled();

    await wrapper.setProps({
      accessStatus: {
        isMissionSimulated: false,
        isUserControlling: true,
      },
    });
    wrapper.vm.revokeMissionControl();
    expect(COMMUNICATION_SPY).toHaveBeenCalled();
  });
});
