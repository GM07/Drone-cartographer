import {Vec2d} from '@/utils/vec2d';

export const OPTIONS = {
  title: {
    text: "Carte d'exploration des drones",
  },
  series: [
    {
      type: 'scatter',
      title: 'Drone',
      data: [] as Vec2d[],
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
      data: [] as Vec2d[],
      xKey: 'x',
      yKey: 'y',
      marker: {
        fill: 'rgba(123,145,222,1)',
        stroke: '#56659b',
      },
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
  padding: {
    top: 10,
    right: 40,
    bottom: 40,
    left: 40,
  },
};
