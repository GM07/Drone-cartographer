import {Vec2d} from '@/utils/vec2d';

export const MM_TO_CM = 10;
export const M_TO_CM = 100;
export const MAP_SIZE = 250;

export const EMPTY_MAP: MapData = {
  id: '',
  position: [],
  sensors: {front: -20, right: -20, back: -20, left: -20},
};

export interface MapData {
  id: string;
  position: number[];
  sensors: Sensors;
}

export interface Sensors {
  front: number;
  right: number;
  back: number;
  left: number;
}

export const OPTIONS = {
  autoSize: true,
  title: {
    text: '',
  },
  series: [
    {
      type: 'scatter',
      //title: 'Drone',
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
      //title: 'Périmètre',
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
  legend: {
    enabled: false,
  },
};
