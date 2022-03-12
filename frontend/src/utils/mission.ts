export interface Point {
  x: number;
  y: number;
}

export interface Mission {
  time_stamp: string;
  flight_duration: number;
  number_of_drones: number;
  is_simulated: boolean;
  total_distance: number;
  maps: [[Point]];
}
