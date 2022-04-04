import {shallowMount, Wrapper} from '@vue/test-utils';
import {DefaultProps} from 'vue/types/options';
import DroneCommands from '@/components/drone_commands.vue';
import {ServerCommunication} from '@/communication/server_communication';

describe('drone_menu.vue', () => {
  let wrapper: Wrapper<DefaultProps & DroneCommands, Element>;
  let identifyStub: jasmine.Spy;

  beforeEach(() => {
    identifyStub = spyOn(ServerCommunication, 'identifyDrone');
    wrapper = shallowMount<DroneCommands>(DroneCommands, {
      propsData: {
        droneid: 'a',
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

  it('should identify', () => {
    wrapper.vm.identify();
    expect(identifyStub).toHaveBeenCalled();
  });
});
