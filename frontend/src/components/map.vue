<template>
  <div id="app">
    <ag-charts-vue :key="options.data" :options="options"></ag-charts-vue>
    <button v-on:click="changeData([1, 1, 2, 2, 3, 3])">
      Click me to change the data!
    </button>
  </div>
</template>

<script lang="ts">
import {Component, Vue} from 'vue-property-decorator';
import {AgChartsVue} from 'ag-charts-vue';
import {Vec2d} from '../../src/utils/vec2d';
import SocketIO from 'socket.io-client';
import {
  SERVER_ADDRESS,
  MAP_DATA_NAMESPACE,
} from '@/communication/server_constants';

@Component({components: {AgChartsVue}})
export default class Map extends Vue {
  readonly SOCKETIO = SocketIO(SERVER_ADDRESS + MAP_DATA_NAMESPACE);
  public options = {
    title: {
      text: "Carte d'exploration des drones",
    },
    data: [new Vec2d(1, 1)],
    series: [
      {
        type: 'scatter',
        xKey: 'x',
        yKey: 'y',
        showInLegend: false,
      },
    ],
    axes: [
      {
        type: 'number',
        position: 'bottom',
      },
      {
        type: 'number',
        position: 'left',
      },
    ],
  };

  constructor() {
    super();
    this.SOCKETIO.on('getMapData', data => {
      this.changeData(data);
    });
  }

  public changeData(array: number[]): void {
    const TEMPARRAY: Vec2d[] = [];
    let arrayIdx = 0;
    for (let i = 0; i < array.length; i++) {
      TEMPARRAY[arrayIdx] = new Vec2d(array[i], array[++i]);
      arrayIdx++;
    }
    this.options.data = TEMPARRAY;
  }
}
</script>
