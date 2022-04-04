export interface Drone {
  name: string;
  xPos: number;
  yPos: number;
  orientation: number;
}

export interface DroneStatus {
  drone: Drone;
  status: string;
}
