import CompletedMissions from '@/views/completed_missions.vue';
import {shallowMount, Wrapper} from '@vue/test-utils';
import {DefaultProps} from 'vue/types/options';
import * as SERVER_CONSTANTS from '@/communication/server_constants';
import {ServerCommunication} from '@/communication/server_communication';
import {Mission} from '@/utils/mission';
import Vuetify from 'vuetify';

describe('completed_missions.vue', () => {
  let wrapper: Wrapper<DefaultProps & CompletedMissions, Element>;
  let getCompletedMissionsSpy: jasmine.Spy;
  let defaultFakeMissionList: Mission[];
  let differentFakeMissionList: Mission[];
  const VUETIFY = new Vuetify();

  beforeEach(() => {
    defaultFakeMissionList = [
      {
        time_stamp: '1',
        flight_duration: 2,
        number_of_drones: 0,
        is_simulated: true,
        total_distance: 0,
        map: '',
      } as Mission,
    ];
    differentFakeMissionList = [
      {
        time_stamp: '2',
        flight_duration: 3,
        number_of_drones: 1,
        is_simulated: false,
        total_distance: 1,
        map: '',
      } as Mission,
    ];

    fetchMock.resetMocks();
    fetchMock.mockResponse(JSON.stringify(defaultFakeMissionList), {
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
    expect(wrapper.vm['missions']).toEqual(defaultFakeMissionList);
    expect(wrapper.vm['showedMissions']).toEqual(defaultFakeMissionList);
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
    fetchMock.mockResponse(JSON.stringify(differentFakeMissionList), {
      status: SERVER_CONSTANTS.HTTP_OK,
    });
    await wrapper.vm['getCompletedMissions']();
    expect(getCompletedMissionsSpy).toHaveBeenCalled();
    expect(wrapper.vm['missions']).toEqual(differentFakeMissionList);
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

    const SPY = spyOn(wrapper.vm, 'getMissionLogs').and.callThrough();
    const DEFAULT_ID = 'id';
    const DEFAULT_MISSION = {_id: 'id'} as Mission;
    wrapper.vm['openLogId'] = DEFAULT_ID;
    wrapper.vm['setLogsMenu'](false, DEFAULT_MISSION);
    expect(wrapper.vm['openLogId']).not.toEqual(DEFAULT_ID);

    const VALID_MISSION = {_id: 'id2', logs: [['1', '2']]} as Mission;
    wrapper.vm['setLogsMenu'](true, VALID_MISSION);
    expect(wrapper.vm['openLogId']).toEqual('id2');

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

    const RES = await wrapper.vm['getMissionLogs'](FAKE_MISSION_ID);
    expect(RES).toEqual(FAKE_MISSION_RESULT);
  });

  it('should set mission map', () => {
    const GET_MISSION_SPY = spyOn(wrapper.vm, 'getMissionMaps').and.returnValue(
      new Promise(resolve => {
        resolve(defaultFakeMissionList[0]);
      })
    );
    wrapper.vm['setMissionMaps'](false, defaultFakeMissionList[0]);
    expect(GET_MISSION_SPY).not.toHaveBeenCalled();

    wrapper.vm['setMissionMaps'](true, defaultFakeMissionList[0]);
    expect(GET_MISSION_SPY).not.toHaveBeenCalled();

    wrapper.vm['setMissionMaps'](true, {
      ...defaultFakeMissionList[0],
      map: undefined as unknown as string,
    });
    expect(GET_MISSION_SPY).toHaveBeenCalled();
  });

  it('should get mission maps', async () => {
    const DATA = 'data';
    fetchMock.resetMocks();
    fetchMock.mockResponse(JSON.stringify(DATA), {
      status: SERVER_CONSTANTS.HTTP_OK,
    });
    const RESULT = await wrapper.vm['getMissionMaps'](
      defaultFakeMissionList[0]
    );
    expect(RESULT).toEqual(DATA);
  });
});
