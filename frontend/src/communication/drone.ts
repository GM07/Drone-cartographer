export interface Drone {
  name: string;
  xPos: number;
  yPos: number;
}

export interface DroneStatus {
  drone: Drone;
  status: string;
}
