import Home from '@/views/home.vue';
import {ROUTER} from '@/router';
import {shallowMount, Wrapper} from '@vue/test-utils';
import {DefaultProps} from 'vue/types/options';
import {STORE_STUB} from '../stubs/store_stub';

describe('home.vue', () => {
  let wrapper: Wrapper<DefaultProps & Home, Element>;
  let routerSpy: jasmine.Spy;

  beforeEach(() => {
    routerSpy = spyOn(ROUTER, 'push');
    wrapper = shallowMount<Home>(Home, {store: STORE_STUB});
  });

  it('should create', () => {
    expect(wrapper).toBeTruthy();
  });

  it('should change path', () => {
    wrapper.vm.configureMission();
    expect(routerSpy).toHaveBeenCalledWith('/Mission');
  });
});
