<template>
  <div class="wrapper">
    <ag-charts-vue :options="options" style="height: 85vh"></ag-charts-vue>
  </div>
</template>

<script lang="ts">
import {Component, Vue, Prop} from 'vue-property-decorator';
import {AgChartsVue} from 'ag-charts-vue';
import {Vec2d} from '../utils/vec2d';
import {OPTIONS, MapData} from '../utils/map_constants';
import SocketIO from 'socket.io-client';

import {
  SERVER_ADDRESS,
  MAP_DATA_NAMESPACE,
} from '@/communication/server_constants';

@Component({components: {AgChartsVue}})
export default class Map extends Vue {
  @Prop() readonly NDRONES!: number;
  readonly SOCKETIO = SocketIO(SERVER_ADDRESS + MAP_DATA_NAMESPACE, {
    transports: ['websocket', 'polling'],
  });
  private options = OPTIONS;

  constructor() {
    super();
    this.SOCKETIO.on('getMapData', data => {
      this.changeData(data);
    });
  }

  public changeData(data: MapData): void {
    console.log(data);
    const TEMPARRAYDRONES: Vec2d[] = [
      new Vec2d(data.position[0] * 100, data.position[1] * 100),
    ];
    const TEMPARRAYPERIM: Vec2d[] = [];
    if (data.sensors.front > 0)
      TEMPARRAYPERIM.push(
        new Vec2d(
          data.position[0] * 100 + data.sensors.front / 10,
          data.position[1] * 100
        )
      );
    if (data.sensors.right > 0)
      TEMPARRAYPERIM.push(
        new Vec2d(
          data.position[0] * 100,
          data.position[1] - data.sensors.right / 10
        )
      );
    if (data.sensors.back > 0)
      TEMPARRAYPERIM.push(
        new Vec2d(
          data.position[0] * 100 - data.sensors.back / 10,
          data.position[1] * 100
        )
      );
    /*if (data.sensors.left > 0)
      TEMPARRAYPERIM.push(
        new Vec2d(
          data.position[0] * 100,
          data.position[1] * 100 + data.sensors.left / 10
        )
      );*/

    const TEMP_OPTIONS = Vue.lodash.cloneDeep(this.options);
    TEMP_OPTIONS.series[0].data = TEMPARRAYDRONES;
    TEMP_OPTIONS.series[1].data.push(...TEMPARRAYPERIM);

    this.options = TEMP_OPTIONS;
  }
}
</script>
