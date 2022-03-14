import {Component, Vue, Prop} from 'vue-property-decorator';
import {AgChartsVue} from 'ag-charts-vue';
import {Vec2d} from '../utils/vec2d';
import {OPTIONS} from '../utils/map_constants';
import SocketIO from 'socket.io-client';
import {cloneDeep} from 'lodash';
import {
  SERVER_ADDRESS,
  MAP_DATA_NAMESPACE,
} from '@/communication/server_constants';

Vue.component('MAP', {
  template: `
        <div class="wrapper">
          <ag-charts-vue    
          :options="options"></ag-charts-vue>
        </div>
    `,
  components: {
    'ag-charts-vue': AgChartsVue,
  },
  // eslint-disable-next-line @typescript-eslint/explicit-module-boundary-types
  data: function () {
    return {
      options: null,
    };
  },
  // eslint-disable-next-line @typescript-eslint/explicit-module-boundary-types
  created() {
    this.options = OPTIONS;
    this.SOCKETIO = SocketIO(SERVER_ADDRESS + MAP_DATA_NAMESPACE, {
      transports: ['websocket', 'polling'],
    });
    this.SOCKETIO.on('getMapData', data => {
      this.changeData(data);
    });
    this.NDRONES = 1;
  },
  // eslint-disable-next-line @typescript-eslint/explicit-module-boundary-types
  mounted() {},
  methods: {
    // eslint-disable-next-line @typescript-eslint/explicit-module-boundary-types
    changeData(array) {
      const TEMPARRAYDRONES = [];
      const TEMPARRAYPERIM = [];
      let arrayIdx = 0;

      for (let i = 0; i < this.NDRONES * 2; i += 2) {
        TEMPARRAYDRONES[arrayIdx] = new Vec2d(array[i], array[i + 1]);

        const J = this.NDRONES * 2 - i;
        TEMPARRAYPERIM[arrayIdx] = new Vec2d(array[J], array[J + 1]);
        arrayIdx++;
      }

      const OPTIONS1 = cloneDeep(this.options);
      OPTIONS1.series[0].data = TEMPARRAYDRONES;
      OPTIONS1.series[1].data = TEMPARRAYPERIM;

      this.options = OPTIONS;
      console.log(this.options.series[0].data);
    },
  },
});
