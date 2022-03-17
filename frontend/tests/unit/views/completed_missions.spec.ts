import CompletedMissions from '@/views/completed_missions.vue';
import {shallowMount, Wrapper} from '@vue/test-utils';
import {DefaultProps} from 'vue/types/options';
import * as SERVER_CONSTANTS from '@/communication/server_constants';
import {ServerCommunication} from '@/communication/server_communication';
import {Mission, Point2d} from '@/utils/mission';
import Vuetify from 'vuetify';

describe('completed_missions.vue', () => {
  let wrapper: Wrapper<DefaultProps & CompletedMissions, Element>;
  let getCompletedMissionsSpy: jasmine.Spy;
  const DEFAULT_FAKE_MISSION_LIST = [
    {
      time_stamp: '1',
      flight_duration: 2,
      number_of_drones: 0,
      is_simulated: true,
      total_distance: 0,
      maps: [[{x: 1, y: 1} as Point2d]],
    } as Mission,
  ];
  const DIFFERENT_FAKE_MISSION_LIST = [
    {
      time_stamp: '2',
      flight_duration: 3,
      number_of_drones: 1,
      is_simulated: false,
      total_distance: 1,
      maps: [[{x: 2, y: 2} as Point2d]],
    } as Mission,
  ];
  const VUETIFY = new Vuetify();

  beforeEach(() => {
    fetchMock.resetMocks();
    fetchMock.mockResponse(JSON.stringify(DEFAULT_FAKE_MISSION_LIST), {
      status: SERVER_CONSTANTS.HTTP_OK,
    });
    getCompletedMissionsSpy = spyOn(
      ServerCommunication,
      'getCompletedMissions'
    ).and.callThrough();
    wrapper = shallowMount<CompletedMissions>(CompletedMissions, {
      vuetify: VUETIFY,
    });
  });

  it('should create', () => {
    expect(wrapper).toBeTruthy();
    expect(wrapper.vm['missions']).toEqual(DEFAULT_FAKE_MISSION_LIST);
    expect(wrapper.vm['showedMissions']).toEqual(DEFAULT_FAKE_MISSION_LIST);
  });

  it('should change filter mode', done => {
    const UPDATE_FILTER_SPY = spyOn(wrapper.vm, 'updateFilteredMissions');
    spyOn(wrapper.vm, 'sortMissions').and.callFake(() => {
      expect(UPDATE_FILTER_SPY).toHaveBeenCalled();
      expect(wrapper.vm['mode']).toEqual(wrapper.vm['filter'].physical);
      done();
    });
    wrapper.vm['changeFilterMode'](wrapper.vm['filter'].physical);
  });

  it('should update filtered missions', () => {
    wrapper.vm['mode'] = wrapper.vm['filter'].physical;
    wrapper.vm['updateFilteredMissions']();
    expect(wrapper.vm['showedMissions']).toHaveLength(0);

    wrapper.vm['mode'] = wrapper.vm['filter'].simulated;
    wrapper.vm['updateFilteredMissions']();
    expect(wrapper.vm['showedMissions']).toHaveLength(1);

    wrapper.vm['date'] = 'impossibleDate';
    wrapper.vm['mode'] = wrapper.vm['filter'].date;
    wrapper.vm['updateFilteredMissions']();
    expect(wrapper.vm['showedMissions']).toHaveLength(0);

    wrapper.vm['date'] = 'impossibleDate';
    wrapper.vm['mode'] = wrapper.vm['filter'].all;
    wrapper.vm['updateFilteredMissions']();
    expect(wrapper.vm['showedMissions']).toEqual(wrapper.vm['missions']);
  });

  it('should get completed missions', async () => {
    fetchMock.resetMocks();
    fetchMock.mockResponse(JSON.stringify(DIFFERENT_FAKE_MISSION_LIST), {
      status: SERVER_CONSTANTS.HTTP_OK,
    });
    await wrapper.vm['getCompletedMissions']();
    expect(getCompletedMissionsSpy).toHaveBeenCalled();
    expect(wrapper.vm['missions']).toEqual(DIFFERENT_FAKE_MISSION_LIST);
  });

  it('should filter by date', () => {
    const CHANGE_FILTER_MODE_SPY = spyOn(wrapper.vm, 'changeFilterMode');
    wrapper.vm['filterByDate']();
    expect(CHANGE_FILTER_MODE_SPY).toHaveBeenCalled();
  });

  it('should update sort missions', () => {
    const UPDATE_FILTER_SPY = spyOn(wrapper.vm, 'updateFilteredMissions');
    wrapper.vm['showedMissions'] = [{} as Mission, {} as Mission];
    const SORT_SPY = spyOn(
      wrapper.vm['showedMissions'],
      'sort'
    ).and.callThrough();

    wrapper.vm['currentFilter'] = wrapper.vm['sorts'][1];
    wrapper.vm['sortMissions']();
    expect(SORT_SPY).toHaveBeenCalledTimes(1);

    wrapper.vm['currentFilter'] = wrapper.vm['sorts'][2];
    wrapper.vm['sortMissions']();
    expect(SORT_SPY).toHaveBeenCalledTimes(2);

    wrapper.vm['currentFilter'] = wrapper.vm['sorts'][3];
    wrapper.vm['sortMissions']();
    expect(SORT_SPY).toHaveBeenCalledTimes(3);

    wrapper.vm['currentFilter'] = wrapper.vm['sorts'][4];
    wrapper.vm['sortMissions']();
    expect(SORT_SPY).toHaveBeenCalledTimes(4);

    wrapper.vm['currentFilter'] = wrapper.vm['sorts'][0];
    wrapper.vm['sortMissions']();
    expect(UPDATE_FILTER_SPY).toHaveBeenCalledTimes(1);
  });

  it('should sort missions by parameters', () => {
    wrapper.vm['isAscending'] = true;
    expect(wrapper.vm['sortMissionNumberParameters'](1, 2)).toEqual(-1);
    wrapper.vm['isAscending'] = false;
    expect(wrapper.vm['sortMissionNumberParameters'](1, 2)).toEqual(1);
  });

  it('should swap order', () => {
    wrapper.vm['isAscending'] = true;
    wrapper.vm['swapOrder']();
    expect(wrapper.vm['isAscending']).toBe(false);
  });

  it('should set logs menu', () => {
    const FAKE_MISSION_RESULT = {_id: 'res'} as Mission;
    fetchMock.resetMocks();
    fetchMock.mockResponse(JSON.stringify(FAKE_MISSION_RESULT), {
      status: SERVER_CONSTANTS.HTTP_OK,
    });

    const SPY = spyOn(wrapper.vm, 'getMissionInfo').and.callThrough();
    const DEFAULT_ID = 'id';
    const DEFAULT_MISSION = {_id: 'id', logs: [['a', 'b']]} as Mission;
    wrapper.vm['openLogId'] = DEFAULT_ID;
    wrapper.vm['setLogsMenu'](false, DEFAULT_MISSION);
    expect(wrapper.vm['openLogId']).not.toEqual(DEFAULT_ID);
    wrapper.vm['setLogsMenu'](true, DEFAULT_MISSION);
    expect(SPY).toHaveBeenCalled();
  });

  it('should get mission info', async () => {
    const FAKE_MISSION_ID = {_id: 'id'} as Mission;
    const FAKE_MISSION_RESULT = {_id: 'res'} as Mission;
    fetchMock.resetMocks();
    fetchMock.mockResponse(JSON.stringify(FAKE_MISSION_RESULT), {
      status: SERVER_CONSTANTS.HTTP_OK,
    });

    const RES = await wrapper.vm['getMissionInfo'](FAKE_MISSION_ID);
    expect(RES).toEqual(FAKE_MISSION_RESULT);
  });
});
