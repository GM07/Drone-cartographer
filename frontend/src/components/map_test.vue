<template>
  <div>
    <canvas id="planet-chart"></canvas>
    <button v-on:click="changeData(PlanetChart)">
      Click me to change data!
    </button>
  </div>
</template>

<script>
import Chart from 'chart.js';
import planetChartData from './map_test.js';

const CTX = document.getElementById('planet-chart');
export default {
  name: 'PlanetChart',
  data() {
    return {
      planetChartData: planetChartData,
    };
  },
  mounted() {
    const CTX = document.getElementById('planet-chart');
    new Chart(CTX, this.planetChartData);
  },
  /*methods: {
    changeData() {
      const DATA = this.planetChartData.data;
      console.log('hey from changeData!');
      DATA.planetChartData.data.datasets[0].data = [1, 2];
      this.data = DATA;
    },
  },*/
};

function changeData(chart) {
  chart.data.datasets[0].data = [0, 0];
  chart.update();
}
</script>
