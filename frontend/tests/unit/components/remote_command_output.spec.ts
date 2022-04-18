import {shallowMount, Wrapper} from '@vue/test-utils';
import {DefaultProps} from 'vue/types/options';
import RemoteCommandOutput from '@/components/remote_command_output.vue';

describe('remote_command_output.vue', () => {
  let wrapper: Wrapper<DefaultProps & RemoteCommandOutput, Element>;

  beforeEach(() => {
    wrapper = shallowMount<RemoteCommandOutput>(RemoteCommandOutput, {
      propsData: {namespace: 'fake'},
    });

    wrapper.vm['socket'].close();
  });

  it('should create', () => {
    expect(wrapper).toBeTruthy();
  });

  it('should listen to stdout', () => {
    const LISTENER = wrapper.vm['socket'].listeners('stdout')[0];
    wrapper.vm['output'] = [['0', '1']];
    const UNSHIFT_SPY = spyOn(
      wrapper.vm['output'],
      'unshift'
    ).and.callThrough();
    LISTENER('a');
    expect(UNSHIFT_SPY).not.toHaveBeenCalled();
    LISTENER('\n');
    expect(UNSHIFT_SPY).toHaveBeenCalled();
  });

  it('should listen to stderr', () => {
    const LISTENER = wrapper.vm['socket'].listeners('stderr')[0];
    wrapper.vm['output'] = [['0', '1']];
    const UNSHIFT_SPY = spyOn(
      wrapper.vm['output'],
      'unshift'
    ).and.callThrough();
    LISTENER('a');
    expect(UNSHIFT_SPY).not.toHaveBeenCalled();
    LISTENER('\n');
    expect(UNSHIFT_SPY).toHaveBeenCalled();
  });

  it('should listen to stop', () => {
    const LISTENER = wrapper.vm['socket'].listeners('stop')[0];
    wrapper.vm.isFinished = false;
    LISTENER();
    expect(wrapper.vm.isFinished).toEqual(true);
  });

  it('should listen to start', () => {
    const LISTENER = wrapper.vm['socket'].listeners('start')[0];
    wrapper.vm.isFinished = true;
    LISTENER();
    expect(wrapper.vm.isFinished).toEqual(false);
  });
});
