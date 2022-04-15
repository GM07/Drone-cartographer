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
  IDLE = 0,
  TAKING_OFF = 1,
  LANDING = 2,
  DEAD = 3,
  EXPLORING = 4,
  RETURNING_TO_BASE = 5,
  CRASHED = 5,
}

export interface DroneData {
  sensors: DroneSensors;
  position: Point2d;
  battery_level: number;
  state: DroneState;
}

export interface Mission {
  _id: string;
  time_stamp: string;
  flight_duration: number;
  number_of_drones: number;
  is_simulated: boolean;
  total_distance: number;
  map: string;
  logs: Array<[string, string]>;
}
