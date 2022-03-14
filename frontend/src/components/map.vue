<template>
  <div class="wrapper">
    <ag-charts-vue :options="options"></ag-charts-vue>
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

  public changeData(data: any): void {
    console.log('Data received: ', data);
    console.log('Data position x : ', data.position[0]);
    console.log('Data position x : ', data.position[1]);
    //console.log('Map data: ', MAP_DATA);
    const TEMPARRAYDRONES: Vec2d[] = [
      new Vec2d(data.position[0], data.position[1]),
    ];
    const TEMPARRAYPERIM: Vec2d[] = [
      new Vec2d(data.position[0] + data.sensors[0], data.position[1]),
      new Vec2d(data.position[0], data.position[1] - data.sensors[1]),
      new Vec2d(data.position[0] - data.sensors[2], data.position[1]),
      new Vec2d(data.position[0], data.position[1] + data.sensors[3]),
    ];

    /*for (let i = 0; i < this.NDRONES * 2; i += 2) {
      TEMPARRAYDRONES[arrayIdx] = new Vec2d(array[i], array[i + 1]);

      const J = this.NDRONES * 2 - i;
      TEMPARRAYPERIM[arrayIdx] = new Vec2d(array[J], array[J + 1]);
      arrayIdx++;
    }*/

    const OPTIONS1 = Vue.lodash.cloneDeep(this.options);
    OPTIONS1.series[0].data = TEMPARRAYDRONES;
    OPTIONS1.series[1].data.push(...TEMPARRAYPERIM);

    this.options = OPTIONS1;
    console.log(this.options.series[0].data);
  }
}
</script>
