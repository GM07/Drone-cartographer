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
