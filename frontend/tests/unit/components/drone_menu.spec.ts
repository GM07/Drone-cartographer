import {shallowMount, Wrapper} from '@vue/test-utils';
import {DefaultProps} from 'vue/types/options';
import DroneMenu from '@/components/drone_menu.vue';
import {NewDroneData} from '@/communication/drone';

describe('drone_menu.vue', () => {
  let wrapper: Wrapper<DefaultProps & DroneMenu, Element>;

  beforeEach(() => {
    wrapper = shallowMount<DroneMenu>(DroneMenu, {
      propsData: {droneList: [], isDroneMenuOpen: true},
    });
  });

  it('should create', () => {
    expect(wrapper).toBeTruthy();
  });

  it('should submit', () => {
    const NEW_DRONE = {
      name: 'a',
      startingXPos: 1,
      startingYPos: 1,
      startingOrientation: 0,
    } as NewDroneData;
    wrapper.vm['newDrone'] = NEW_DRONE;
    const RESET_SPY = spyOn(wrapper.vm, 'resetValidation');
    const CLOSE_MENU_SPY = spyOn(wrapper.vm, 'closeMenu');
    const EMIT_SPY = spyOn(wrapper.vm, '$emit');
    (wrapper.vm.$refs.form as Vue & {validate: () => boolean}).validate = () =>
      false;
    (wrapper.vm.$refs.pos as Vue & {validate: () => boolean}).validate = () =>
      false;
    wrapper.vm.submit();
    expect(RESET_SPY).not.toHaveBeenCalled();

    (wrapper.vm.$refs.form as Vue & {validate: () => boolean}).validate = () =>
      true;
    (wrapper.vm.$refs.pos as Vue & {validate: () => boolean}).validate = () =>
      true;
    wrapper.vm.submit();
    expect(RESET_SPY).toHaveBeenCalled();
    expect(CLOSE_MENU_SPY).toHaveBeenCalled();
    expect(EMIT_SPY).toHaveBeenCalledWith('addDrone', NEW_DRONE);
  });

  it('should close the menu', () => {
    const EMIT_SPY = spyOn(wrapper.vm, '$emit');
    wrapper.vm.closeMenu();
    expect(EMIT_SPY).toHaveBeenCalledWith('setDroneMenuOpen', false);
  });

  it('should reset validation', () => {
    (
      wrapper.vm.$refs.form as Vue & {resetValidation: () => boolean}
    ).resetValidation = () => true;
    const RESET_SPY = spyOn(
      wrapper.vm.$refs.form as Vue & {resetValidation: () => boolean},
      'resetValidation'
    );
    wrapper.vm.resetValidation();
    expect(RESET_SPY).toHaveBeenCalled();
  });

  it('should validate the drone position form', () => {
    (wrapper.vm.$refs.pos as Vue & {validate: () => boolean}).validate = () =>
      true;
    const VALIDATE_SPY = spyOn(
      wrapper.vm.$refs.pos as Vue & {validate: () => boolean},
      'validate'
    );
    wrapper.vm.validatePositionForm();
    expect(VALIDATE_SPY).toHaveBeenCalled();
  });

  it('should validate if the drone does not exist twice', async () => {
    const NEW_DRONE = {
      name: 'a',
      startingXPos: 1,
      startingYPos: 1,
      startingOrientation: 0,
    } as NewDroneData;
    wrapper.vm['newDrone'] = NEW_DRONE;
    expect(wrapper.vm.validateDroneExistTwice() as boolean).toBe(true);
    await wrapper.setProps({droneList: [NEW_DRONE]});
    expect(wrapper.vm.validateDroneExistTwice()).toHaveProperty('length');
  });

  it('should validate the drone name length', () => {
    expect(wrapper.vm.validateDroneNameLength('')).toHaveProperty('length');
    expect(wrapper.vm.validateDroneNameLength('a')).toBe(true);
    const LONG_NAME = 'AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA';
    expect(wrapper.vm.validateDroneNameLength(LONG_NAME)).toHaveProperty(
      'length'
    );
  });

  it('should validate the drone distance', async () => {
    const NEW_DRONE = {
      name: 'a',
      startingXPos: 1,
      startingYPos: 1,
      startingOrientation: 0,
    } as NewDroneData;
    wrapper.vm['newDrone'] = NEW_DRONE;
    expect(wrapper.vm.validateDistance()).toBe(true);
    await wrapper.setProps({droneList: [NEW_DRONE]});
    expect(wrapper.vm.validateDistance()).toHaveProperty('length');
  });

  it('should validate the drone position', () => {
    const INVALID_POSITION = 100;
    expect(wrapper.vm.validatePosition(0)).toBe(true);
    expect(wrapper.vm.validatePosition(INVALID_POSITION)).toHaveProperty(
      'length'
    );
  });

  it('should validate the characters in the drone name', () => {
    const INVALID_CHAR = '@';
    expect(wrapper.vm.validateDroneNameCharacters('a')).toBe(true);
    expect(wrapper.vm.validateDroneNameCharacters(INVALID_CHAR)).toHaveProperty(
      'length'
    );
  });

  it('should tell if an element is mandatory', () => {
    expect(wrapper.vm.mandatory(true)).toBe(true);
    expect(wrapper.vm.mandatory(false)).toHaveProperty('length');
  });
});
