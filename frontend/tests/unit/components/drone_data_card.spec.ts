import {shallowMount, Wrapper} from '@vue/test-utils';
import {DefaultProps} from 'vue/types/options';
import DroneDataCard from '@/components/drone_data_card.vue';
import {DEFAULT_DRONE_DATA, DroneData} from '@/communication/drone';

describe('drone_data_card.vue', () => {
  let wrapper: Wrapper<DefaultProps & DroneDataCard, Element>;

  beforeEach(() => {
    wrapper = shallowMount<DroneDataCard>(DroneDataCard, {
      propsData: {
        droneData: {...DEFAULT_DRONE_DATA},
        selected: true,
        userControlling: true,
      },
    });
  });

  it('should create', () => {
    expect(wrapper).toBeTruthy();
  });

  it('should compute the battery', async () => {
    expect(wrapper.vm.computeBatteryIconOutput()).toEqual('mdi-battery');
    let droneData: DroneData = {
      ...DEFAULT_DRONE_DATA,
      batteryLevel: 0,
    } as DroneData;
    await wrapper.setProps({
      droneData: droneData,
      selected: true,
      userControlling: true,
    });
    expect(wrapper.vm.computeBatteryIconOutput()).toEqual(
      'mdi-battery-outline'
    );
    droneData = {...DEFAULT_DRONE_DATA, batteryLevel: 50} as DroneData;

    await wrapper.setProps({
      droneData: droneData,
      selected: true,
      userControlling: true,
    });
    expect(wrapper.vm.computeBatteryIconOutput()).toEqual('mdi-battery-50');
  });
});
