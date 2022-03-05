<template>
  <div id="app">
    <ag-charts-vue :key="data" :options="options"></ag-charts-vue>
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
  public data: Vec2d[] = [];
  public options = {
    title: {
      text: "Carte d'exploration des drones",
    },
    data: this.getData(),
    /*series: [
      {
        type: 'scatter',
        title: 'Drone',
        data: this.getPosDrones(),
        xKey: 'x',
        yKey: 'y',
        marker: {
          fill: 'rgba(227,111,106,1)',
          stroke: '#9f4e4a',
        },
      },
      {
        type: 'scatter',
        title: 'Périmètre',
        data: this.getData(),
        xKey: 'x',
        yKey: 'y',
        marker: {
          fill: 'rgba(123,145,222,1)',
          stroke: '#56659b',
        },
      },
    ],*/
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
    this.data = TEMPARRAY;
    this.options.data = TEMPARRAY;
  }

  public getData(): Vec2d[] {
    return this.data;
  }

  public getPosDrones(): Vec2d[] {
    return [new Vec2d(1.002, 1.002)];
  }
}
</script>
