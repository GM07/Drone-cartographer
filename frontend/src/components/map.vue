<template>
  <div id="app">
    <ag-charts-vue
      :key="(this.options.series[1].data, this.options.series[0].data)"
      :options="options"
    ></ag-charts-vue>
    <!-- :key="series[0]"  -->
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
  readonly NDRONES = 1;
  public data: Vec2d[] = [];
  public posDrones: Vec2d[] = [];
  public options = {
    title: {
      text: "Carte d'exploration des drones",
    },
    //data: this.getData(),
    series: [
      {
        type: 'scatter',
        title: 'Drone',
        data: this.getPosDrones(), //[new Vec2d(2, 2)],
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
        data: this.getData(), //[new Vec2d(1, 1)]
        xKey: 'x',
        yKey: 'y',
        marker: {
          fill: 'rgba(123,145,222,1)',
          stroke: '#56659b',
        },
      },
    ],
    /*series: [
      {
        type: 'scatter',
        xKey: 'x',
        yKey: 'y',
        showInLegend: false,
      },
    ],*/
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
    const TEMPARRAYDRONES: Vec2d[] = [];
    let arrayIdx = 0;
    for (let i = 0; i < this.NDRONES * 2; i++) {
      // Times 2 because position is a 2d vector
      TEMPARRAYDRONES[arrayIdx] = new Vec2d(array[i], array[++i]);
      arrayIdx++;
    }
    this.options.series[0].data = TEMPARRAYDRONES;

    const TEMPARRAYPERIM: Vec2d[] = [];
    arrayIdx = 0;
    for (let i = this.NDRONES * 2; i < array.length; i++) {
      console.log('Drone #', i);
      //let y = i;
      //console.log('New perim point: ', array[y], array[++y]);
      TEMPARRAYPERIM[arrayIdx] = new Vec2d(array[i], array[++i]);
    }
    this.options.series[1].data = TEMPARRAYPERIM;

    console.log('Drone array: ', TEMPARRAYDRONES);
    console.log('Perim array: ', TEMPARRAYPERIM);
  }

  public getData(): Vec2d[] {
    console.log('hey from getData!');
    return this.data;
  }

  public getPosDrones(): Vec2d[] {
    console.log('hey from getPosDrones!');
    return this.posDrones;
  }
}
</script>
