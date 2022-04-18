<template>
  <div style="display: flex; flex-direction: column; align-items: center">
    <h2 style="padding-top: 20px">Carte {{ this.mapName }}</h2>
    <canvas ref="canvas" class="canvas-style"></canvas>
  </div>
</template>

<style scoped>
.canvas-style {
  width: 65vh;
  height: 65vh;
  border: 2px solid gray;
  margin: 20px 10px;
}
</style>

<script lang="ts">
import {Component, Vue, Prop, Watch} from 'vue-property-decorator';
import {Vec2d} from '@/utils/vec2d';
import {
  MapData,
  MM_TO_CM,
  M_TO_CM,
  MAP_SIZE,
  DEGREE_TO_RAD,
} from '@/utils/map_constants';

import {SOCKETIO_MAP_DATA} from '@/communication/server_constants';
import {DroneData} from '@/communication/drone';

@Component({})
export default class Map extends Vue {
  @Prop() public indexDrone!: number;
  @Prop() public droneList!: DroneData[];
  @Prop() public maps!: HTMLCanvasElement[];

  private dronePos: Vec2d[] = [new Vec2d(0, 0)];
  private mapSize: Vec2d = new Vec2d(0, 0);
  private canvasSize: Vec2d = new Vec2d(0, 0);

  private hasBeenMounted = false;
  private mapName = 'générale';

  constructor() {
    super();
  }

  @Watch('indexDrone', {immediate: true})
  public onIndexChange(): void {
    if (!this.hasBeenMounted) return;

    if (this.maps.length === 0) this.createMaps();

    this.mapName =
      this.indexDrone === 0
        ? 'générale'
        : this.droneList[this.indexDrone - 1].name;

    const CANVAS = this.$refs.canvas as HTMLCanvasElement;
    const CANVAS_CTX = CANVAS.getContext('2d') as CanvasRenderingContext2D;

    CANVAS_CTX.clearRect(0, 0, CANVAS.width, CANVAS.height);
    CANVAS_CTX.drawImage(this.maps[this.indexDrone], 0, 0);
  }

  public onMapChange(data: MapData, idx: number): void {
    if (this.maps.length !== this.droneList.length + 1) this.createMaps();
    if (this.droneList.length === 0) return;

    const DATA_INDEX = this.findDroneIdx(data) - 1;

    const COS_ORIENTATION = Math.cos(
      this.droneList[DATA_INDEX].startingOrientation * DEGREE_TO_RAD
    );
    const SIN_ORIENTATION = Math.sin(
      this.droneList[DATA_INDEX].startingOrientation * DEGREE_TO_RAD
    );

    // Get new drone position
    const ROTATED_POS = new Vec2d(
      data.position[0] * M_TO_CM * COS_ORIENTATION -
        data.position[1] * M_TO_CM * SIN_ORIENTATION +
        this.droneList[DATA_INDEX].startingXPos * M_TO_CM,
      data.position[0] * M_TO_CM * SIN_ORIENTATION +
        data.position[1] * M_TO_CM * COS_ORIENTATION +
        this.droneList[DATA_INDEX].startingYPos * M_TO_CM
    );

    this.dronePos = [ROTATED_POS];

    // Show new drone position
    (this.maps[idx].getContext('2d') as CanvasRenderingContext2D).fillStyle =
      'red';
    const NEW_POS = this.toCanvasPos(this.dronePos[0]);
    (this.maps[idx].getContext('2d') as CanvasRenderingContext2D).fillRect(
      NEW_POS.x,
      NEW_POS.y,
      2,
      2
    );

    const TEMPARRAYPERIM: Vec2d[] = [];

    const POS_X = this.dronePos[0].x;
    const POS_Y = this.dronePos[0].y;

    const FRONT = data.sensors.front / MM_TO_CM;
    const FRONT_VEC = new Vec2d(1, 0);
    const LEFT = data.sensors.left / MM_TO_CM;
    const LEFT_VEC = new Vec2d(0, 1);
    const BACK = data.sensors.back / MM_TO_CM;
    const BACK_VEC = new Vec2d(-1, 0);
    const RIGHT = data.sensors.right / MM_TO_CM;
    const RIGHT_VEC = new Vec2d(0, -1);
    const CANVAS = this.$refs.canvas as HTMLCanvasElement;
    const CANVAS_CTX = CANVAS.getContext('2d') as CanvasRenderingContext2D;

    if (data.sensors.front > 0)
      TEMPARRAYPERIM.push(
        new Vec2d(
          POS_X +
            (FRONT_VEC.x * COS_ORIENTATION - FRONT_VEC.y * SIN_ORIENTATION) *
              FRONT,
          POS_Y +
            (FRONT_VEC.x * SIN_ORIENTATION + FRONT_VEC.y * COS_ORIENTATION) *
              FRONT
        )
      );
    if (data.sensors.right > 0)
      TEMPARRAYPERIM.push(
        new Vec2d(
          POS_X +
            (RIGHT_VEC.x * COS_ORIENTATION - RIGHT_VEC.y * SIN_ORIENTATION) *
              RIGHT,
          POS_Y +
            (RIGHT_VEC.x * SIN_ORIENTATION + RIGHT_VEC.y * COS_ORIENTATION) *
              RIGHT
        )
      );
    if (data.sensors.back > 0)
      TEMPARRAYPERIM.push(
        new Vec2d(
          POS_X +
            (BACK_VEC.x * COS_ORIENTATION - BACK_VEC.y * SIN_ORIENTATION) *
              BACK,
          POS_Y +
            (BACK_VEC.x * SIN_ORIENTATION + BACK_VEC.y * COS_ORIENTATION) * BACK
        )
      );
    if (data.sensors.left > 0)
      TEMPARRAYPERIM.push(
        new Vec2d(
          POS_X +
            (LEFT_VEC.x * COS_ORIENTATION - LEFT_VEC.y * SIN_ORIENTATION) *
              LEFT,
          POS_Y +
            (LEFT_VEC.x * SIN_ORIENTATION + LEFT_VEC.y * COS_ORIENTATION) * LEFT
        )
      );

    for (const VEC of TEMPARRAYPERIM) {
      (this.maps[idx].getContext('2d') as CanvasRenderingContext2D).fillStyle =
        'blue';
      const WALL_POS = this.toCanvasPos(VEC);
      (this.maps[idx].getContext('2d') as CanvasRenderingContext2D).fillRect(
        WALL_POS.x,
        WALL_POS.y,
        3,
        3
      );
    }

    CANVAS_CTX.drawImage(this.maps[this.indexDrone], 0, 0);
  }

  public findDroneIdx(data: MapData): number {
    let idx = 0;
    for (let i = 0; i < this.droneList.length; i++) {
      if (this.droneList[i].name === data.id) {
        idx = i + 1;
      }
    }
    return idx;
  }

  public createMaps(): void {
    for (let i = this.maps.length; i <= this.droneList.length; i++) {
      const CANVAS = document.createElement('canvas') as HTMLCanvasElement;
      CANVAS.width = this.canvasSize.x * 2;
      CANVAS.height = this.canvasSize.y * 2;
      this.maps.push(CANVAS);
    }
  }

  public drawOnCanvas(data: MapData): void {
    this.onMapChange(data, 0);
    this.onMapChange(data, this.findDroneIdx(data));
  }

  public toCanvasPos(pos: Vec2d): Vec2d {
    let canvasX = (pos.x * this.canvasSize.x) / this.mapSize.x;
    let canvasY = (pos.y * this.canvasSize.y) / this.mapSize.y;
    canvasX += this.canvasSize.x;
    canvasY = this.canvasSize.y - canvasY;

    return new Vec2d(canvasX, canvasY);
  }

  mounted(): void {
    const CANVAS = this.$refs.canvas as HTMLCanvasElement;
    CANVAS.height = CANVAS.offsetHeight;
    CANVAS.width = CANVAS.offsetWidth;

    // Compute sizes used for changing units
    this.canvasSize = new Vec2d(CANVAS.width / 2, CANVAS.height / 2);
    if (this.canvasSize.x > this.canvasSize.y) {
      this.mapSize.y = MAP_SIZE; // FOR NOW WE HAVE AN ARENA OF 5m
      this.mapSize.x = (this.canvasSize.x * this.mapSize.y) / this.canvasSize.y;
    } else {
      this.mapSize.x = MAP_SIZE; // FOR NOW WE HAVE AN ARENA OF 5m
      this.mapSize.y = (this.canvasSize.y * this.mapSize.x) / this.canvasSize.x;
    }

    const CANVAS_CTX = CANVAS.getContext('2d') as CanvasRenderingContext2D;
    CANVAS_CTX.clearRect(0, 0, CANVAS.width, CANVAS.height);

    this.hasBeenMounted = true;
  }

  beforeCreate(): void {
    SOCKETIO_MAP_DATA.on('getMapData', data => {
      data.forEach((mapData: MapData) => {
        this.drawOnCanvas(mapData);
      });
    });

    SOCKETIO_MAP_DATA.on('clear_all_maps', isMissionLaunched => {
      if (isMissionLaunched) {
        this.maps.forEach((map: HTMLCanvasElement) => {
          (map.getContext('2d') as CanvasRenderingContext2D).clearRect(
            0,
            0,
            map.width,
            map.height
          );
        });
        const CANVAS = this.$refs.canvas as HTMLCanvasElement;
        const CANVAS_CTX = CANVAS.getContext('2d') as CanvasRenderingContext2D;
        CANVAS_CTX.clearRect(0, 0, CANVAS.width, CANVAS.height);
      }
    });

    SOCKETIO_MAP_DATA.open();
  }

  private destroyed() {
    SOCKETIO_MAP_DATA.removeAllListeners().close();
  }
}
</script>
