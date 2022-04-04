<template>
  <div style="display: flex; flex-direction: column; align-items: center">
    <h2 style="color: DimGray">Carte : {{ this.mapData.id.slice(16) }}</h2>
    <canvas
      ref="canvas"
      style="
        width: 50vh;
        height: 50vh;
        border: 2px solid gray;
        margin: 20px 10px;
      "
    ></canvas>
  </div>
</template>

<script lang="ts">
import {Component, Vue, Prop, Watch} from 'vue-property-decorator';
import {Vec2d} from '../utils/vec2d';
import {
  OPTIONS,
  MapData,
  MM_TO_CM,
  M_TO_CM,
  MAP_SIZE,
} from '../utils/map_constants';
import SocketIO from 'socket.io-client';

import {
  SERVER_ADDRESS,
  MAP_DATA_NAMESPACE,
} from '@/communication/server_constants';

@Component({})
export default class Map extends Vue {
  @Prop() public mapData!: MapData;
  readonly SOCKETIO = SocketIO(SERVER_ADDRESS + MAP_DATA_NAMESPACE, {
    transports: ['websocket', 'polling'],
  });
  private options = OPTIONS;
  private dronePos: Vec2d[] = [new Vec2d(0, 0)];
  private mapSize: Vec2d = new Vec2d(0, 0);
  private canvasSize: Vec2d = new Vec2d(0, 0);

  constructor() {
    super();
  }

  @Watch('mapData', {immediate: true})
  public onMapChange(): void {
    console.log(this.mapData);
    // Get the canvas
    const CANVAS = this.$refs.canvas as HTMLCanvasElement;
    const CANVAS_CTX = CANVAS.getContext('2d') as CanvasRenderingContext2D;

    // Get new drone position
    this.dronePos = [
      new Vec2d(this.mapData.position[0] * 100, this.mapData.position[1] * 100),
    ];

    // Show new drone position
    CANVAS_CTX.fillStyle = 'red';
    const NEW_POS = this.toCanvasPos(this.dronePos[0]);
    CANVAS_CTX.fillRect(NEW_POS.x, NEW_POS.y, 1, 1);

    const TEMPARRAYPERIM: Vec2d[] = [];
    if (this.mapData.sensors.front > 0)
      TEMPARRAYPERIM.push(
        new Vec2d(
          this.mapData.position[0] * M_TO_CM +
            this.mapData.sensors.front / MM_TO_CM,
          this.mapData.position[1] * M_TO_CM
        )
      );
    if (this.mapData.sensors.right > 0)
      TEMPARRAYPERIM.push(
        new Vec2d(
          this.mapData.position[0] * M_TO_CM,
          this.mapData.position[1] * M_TO_CM -
            this.mapData.sensors.right / MM_TO_CM
        )
      );
    if (this.mapData.sensors.back > 0)
      TEMPARRAYPERIM.push(
        new Vec2d(
          this.mapData.position[0] * M_TO_CM -
            this.mapData.sensors.back / MM_TO_CM,
          this.mapData.position[1] * M_TO_CM
        )
      );
    if (this.mapData.sensors.left > 0)
      TEMPARRAYPERIM.push(
        new Vec2d(
          this.mapData.position[0] * M_TO_CM,
          this.mapData.position[1] * M_TO_CM +
            this.mapData.sensors.left / MM_TO_CM
        )
      );

    for (const VEC of TEMPARRAYPERIM) {
      CANVAS_CTX.fillStyle = 'blue';
      const WALL_POS = this.toCanvasPos(VEC);
      CANVAS_CTX.fillRect(WALL_POS.x, WALL_POS.y, 3, 3);
    }

    this.options.series[0].data = this.dronePos;
    this.options.series[1].data.push(...TEMPARRAYPERIM);
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
  }
}
</script>
