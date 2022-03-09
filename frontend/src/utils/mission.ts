export interface Point2d {
  x: number;
  y: number;
}

export interface DroneSensors {
  front: number;
  back: number;
  right: number;
  left: number;
}

export enum DroneState {
  IDLE = 0 , 
  TAKING_OFF = 1,
  LANDING = 2,
  DEAD = 3,
  EXPLORING = 4,
  CRASHED = 5,
}

export interface DroneData {
  sensors: DroneSensors;
  position: Point2d;
  battery_level: number;
  state: DroneState;
}

export interface Logs {
  drone_info: [[DroneData]];
  commands: [string];
}

export interface Mission {
  time_stamp: string;
  flight_duration: number;
  number_of_drones: number;
  is_simulated: boolean;
  total_distance: number;
  maps: [[Point2d]];
  logs: Logs;
}
