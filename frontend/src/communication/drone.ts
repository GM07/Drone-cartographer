export interface NewDroneData {
  name: string;
  startingXPos: number;
  startingYPos: number;
  startingOrientation: number;
}

export interface DroneData {
  name: string;
  sensorFront: number;
  sensorLeft: number;
  sensorBack: number;
  sensorRight: number;
  xPos: number;
  yPos: number;
  zPos: number;
  batteryLevel: number;
  state: string;
  startingXPos: number;
  startingYPos: number;
  startingOrientation: number;
}

export const DEFAULT_NEW_DRONE_DATA = {
  name: 'radio://0/80/2M/E7E7E7E761',
  startingXPos: 0,
  startingYPos: 0,
  startingOrientation: 0,
} as DroneData;

export const DEFAULT_DRONE_DATA = {
  name: '',
  sensorFront: -20,
  sensorLeft: -20,
  sensorBack: -20,
  sensorRight: -20,
  xPos: 0,
  yPos: 0,
  zPos: 0,
  batteryLevel: 100,
  state: 'IDLE',
  startingXPos: 0,
  startingYPos: 0,
  startingOrientation: 0,
} as DroneData;
