<template>
  <div>
    <ag-charts-vue
      :key="(this.options.series[1].data, this.options.series[0].data)"
      :options="options"
      style="height: 85vh"
    ></ag-charts-vue>
  </div>
</template>

<script lang="ts">
import {Component, Vue, Prop} from 'vue-property-decorator';
import {AgChartsVue} from 'ag-charts-vue';
import {Vec2d} from '../utils/vec2d';
import {OPTIONS} from '../utils/map_constants';
import SocketIO from 'socket.io-client';
import {
  SERVER_ADDRESS,
  MAP_DATA_NAMESPACE,
} from '@/communication/server_constants';

@Component({components: {AgChartsVue}})
export default class Map extends Vue {
  @Prop() readonly NDRONES!: number;
  readonly SOCKETIO = SocketIO(SERVER_ADDRESS + MAP_DATA_NAMESPACE);
  private options = OPTIONS;

  constructor() {
    super();
    this.SOCKETIO.on('getMapData', data => {
      this.changeData(data);
    });
  }

  public changeData(array: number[]): void {
    const TEMPARRAYDRONES: Vec2d[] = [];
    const TEMPARRAYPERIM: Vec2d[] = [];
    let arrayIdx = 0;

    for (let i = 0; i < this.NDRONES * 2; i++) {
      // Times 2 because position is a 2d vector
      TEMPARRAYDRONES[arrayIdx] = new Vec2d(array[i], array[++i]);
      arrayIdx++;
    }
    arrayIdx = 0;
    for (let i = this.NDRONES * 2; i < array.length; i++) {
      TEMPARRAYPERIM[arrayIdx] = new Vec2d(array[i], array[++i]);
      arrayIdx++;
    }

    this.options.series[0].data = TEMPARRAYDRONES;
    this.options.series[1].data = TEMPARRAYPERIM;
  }
}
</script>
