export interface Point {
  x: number;
  y: number;
}

export interface Mission {
  time_stamp: string;
  temps_de_vol: number;
  nombre_de_drones: number;
  est_simule: boolean;
  distance_totale: number;
  cartes: Point[][];
}
