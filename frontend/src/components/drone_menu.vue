<template>
  <v-dialog v-model="isDroneMenuOpen" max-width="390" persistent>
    <v-form ref="form">
      <v-card>
        <v-card-title>
          <span class="text-h5 justify-center">Nouveau Drone</span>
        </v-card-title>
        <v-card-text>
          <v-container id="container">
            <v-row>
              <v-col cols="12">
                <v-text-field
                  v-model="newDrone.name"
                  label="Adresse du drone"
                  required
                  :rules="[
                    validateDroneExistTwice,
                    validateDroneNameLength,
                    validateDroneNameCharacters,
                    mandatory,
                  ]"
                  v-on:keydown.enter.prevent
                ></v-text-field>
              </v-col>
            </v-row>
            <v-form ref="pos">
              <v-row>
                <v-col cols="12">
                  <v-text-field
                    v-model.number="newDrone.startingXPos"
                    label="Position X"
                    :rules="[
                      validateDistance,
                      validatePosition,
                      v =>
                        !isNaN(parseFloat(Number(v))) ||
                        'Le nombre entré n\'est pas valide',
                    ]"
                    @change="validatePositionForm('x')"
                  ></v-text-field>
                </v-col>
              </v-row>
              <v-row>
                <v-col cols="12">
                  <v-text-field
                    v-model.number="newDrone.startingYPos"
                    label="Position Y"
                    :rules="[
                      validateDistance,
                      validatePosition,
                      v =>
                        !isNaN(parseFloat(Number(v))) ||
                        'Le nombre entré n\'est pas valide',
                    ]"
                    @change="validatePositionForm('y')"
                  ></v-text-field>
                </v-col>
              </v-row>
              <v-row>
                <v-col cols="12">
                  <v-text-field
                    v-model.number="newDrone.startingOrientation"
                    label="Orientation"
                    :rules="[
                      v =>
                        !isNaN(parseFloat(Number(v))) ||
                        'Le nombre entré n\'est pas valide',
                      v =>
                        (v < 360 && v >= 0) ||
                        'La valeur doit être entre 0 et 360',
                    ]"
                    @change="validatePositionForm('o')"
                  ></v-text-field>
                </v-col>
              </v-row>
            </v-form>
            <div id="map-container">
              <div id="map-preview" ref="map">
                <div
                  v-for="drone in this.currentDroneList"
                  :id="drone.name"
                  :key="drone.name"
                  :style="
                    'position: absolute; width: 30px; height: 30px; left: ' +
                    xPosToPixel(drone.startingXPos) +
                    'px; top:' +
                    yPosToPixel(drone.startingYPos) +
                    'px'
                  "
                >
                  <img src="../assets/drone.svg" />
                </div>
                <vue-resizable
                  id="drone"
                  ref="drone"
                  :active="[]"
                  :dragSelector="dragSelector"
                  :fitParent="true"
                  :height="30"
                  :left="x"
                  :top="y"
                  :width="30"
                  y
                  @drag:move="dragMove"
                  ><div id="draggable" class="drag-container">
                    <img src="../assets/drone.svg" />
                  </div>
                </vue-resizable>
              </div>
            </div>
          </v-container>
        </v-card-text>
        <v-card-actions>
          <v-spacer></v-spacer>
          <v-btn color="blue darken-1" text @click="closeMenu"> Fermer </v-btn>
          <v-btn color="blue darken-1" text @click="submit">
            Enregistrer
          </v-btn>
        </v-card-actions>
      </v-card>
    </v-form>
  </v-dialog>
</template>

<style lang="scss" scoped>
#map-container {
  margin-top: 20px;
  display: flex;
  text-align: center;
  justify-content: center;
  align-items: center;
}

#map-preview {
  width: 300px;
  height: 300px;
  position: relative;
  border: 1px solid black;
}

#drone {
  position: absolute;
  top: calc(100% - 15px); // Middle of map
  left: calc(100% - 15px); // Middle of map
  z-index: 2000;
  filter: invert(50%) sepia(20%) saturate(5685%) hue-rotate(175deg)
    brightness(96%) contrast(94%);
}

#draggable {
  width: 100%;
  height: 100%;
}
</style>

<script lang="ts">
import {Component, Vue, Prop, Watch} from 'vue-property-decorator';
import {
  DEFAULT_NEW_DRONE_DATA,
  DroneData,
  NewDroneData,
} from '@/communication/drone';
import VueResizable from 'vue-resizable';
import {round} from 'lodash';

@Component({components: {VueResizable}})
export default class DroneMenu extends Vue {
  private static readonly MAX_RANGE = 2;
  private static readonly DRONE_WIDTH = 30;
  private static readonly DRONE_HEIGHT = 30;
  private static readonly WIDTH = 300 - DroneMenu.DRONE_WIDTH / 2;
  private static readonly HEIGHT = 300 - DroneMenu.DRONE_HEIGHT / 2;
  private static droneCount = 0;
  @Prop() private droneList!: DroneData[];
  @Prop() private isDroneMenuOpen!: boolean;
  @Prop() private isMissionSimulated!: boolean;
  @Prop() private currentDroneList: DroneData[] = [];
  private newDrone: NewDroneData = {...DEFAULT_NEW_DRONE_DATA};
  private dragSelector = '.drag-container';
  private x = 0;
  private y = 0;

  constructor() {
    super();
    window.addEventListener('keydown', (event: KeyboardEvent) => {
      if (event.key === 'Escape') {
        event.preventDefault();
        this.closeMenu();
      } else if (event.key === 'Enter') {
        this.submit();
      }
    });
    this.setDefaultNewDroneData();
  }

  @Watch('isDroneMenuOpen')
  public setDefaultNewDroneData(): void {
    if (!this.isDroneMenuOpen) {
      // this.x = this.xPosToPixel(this.newDrone.startingXPos);
      // this.y = this.yPosToPixel(this.newDrone.startingYPos);
      // this.$forceUpdate();
      return;
    }

    if (this.isMissionSimulated) {
      DroneMenu.droneCount = this.currentDroneList.length;
      this.newDrone.name = 'drone_' + DroneMenu.droneCount++;
      this.newDrone.startingXPos = this.pixelToXPos(this.x);
      this.newDrone.startingYPos = this.pixelToYPos(this.y);
      this.newDrone.startingOrientation = 0;
    }

    this.$forceUpdate();
  }

  // eslint-disable-next-line @typescript-eslint/explicit-module-boundary-types
  public dragMove(data): void {
    this.newDrone.startingXPos = this.pixelToXPos(data.left);
    this.newDrone.startingYPos = this.pixelToYPos(data.top);
  }

  public pixelToXPos(pixel: number): number {
    return (
      round(
        (pixel * 2 + DroneMenu.DRONE_WIDTH - DroneMenu.WIDTH) / DroneMenu.WIDTH,
        2
      ) * DroneMenu.MAX_RANGE
    );
  }

  public pixelToYPos(pixel: number): number {
    return (
      -round(
        (pixel * 2 + DroneMenu.DRONE_HEIGHT - DroneMenu.HEIGHT) /
          DroneMenu.HEIGHT,
        2
      ) * DroneMenu.MAX_RANGE
    );
  }

  public xPosToPixel(pos: number): number {
    return (
      ((pos / DroneMenu.MAX_RANGE) * DroneMenu.WIDTH +
        DroneMenu.WIDTH -
        DroneMenu.DRONE_WIDTH) /
      2
    );
  }

  public yPosToPixel(pos: number): number {
    return (
      ((-pos / DroneMenu.MAX_RANGE) * DroneMenu.HEIGHT +
        DroneMenu.HEIGHT -
        DroneMenu.DRONE_HEIGHT) /
      2
    );
  }

  public submit(): void {
    if (
      (this.$refs.form as Vue & {validate: () => boolean}).validate() &&
      (this.$refs.pos as Vue & {validate: () => boolean}).validate()
    ) {
      this.$emit('addDrone', {...this.newDrone} as NewDroneData);
      this.closeMenu();
      this.resetValidation();
      // this.setDefaultNewDroneData();
      this.newDrone.startingXPos = this.pixelToXPos(this.x);
      this.newDrone.startingYPos = this.pixelToYPos(this.y);
    }
  }

  public closeMenu(): void {
    this.$emit('setDroneMenuOpen', false);
  }

  public resetValidation(): void {
    (
      this.$refs.form as Vue & {resetValidation: () => boolean}
    ).resetValidation();
  }

  public validatePositionForm(metric = ''): void {
    if (metric === 'x') {
      this.x = this.xPosToPixel(this.newDrone.startingXPos);
      this.$forceUpdate();
    }
    if (metric === 'y') {
      this.y = this.yPosToPixel(this.newDrone.startingYPos);
      this.$forceUpdate();
    }
    (this.$refs.pos as Vue & {validate: () => boolean}).validate();
  }

  public validateDroneExistTwice(): boolean | string {
    return (
      this.droneList.filter(drone => {
        return drone.name === this.newDrone.name;
      }).length === 0 || 'Ce drone a déjà été créé'
    );
  }

  public validateDroneNameLength(v: string): boolean | string {
    return (
      (v.length < 40 && v.length > 0) ||
      'Le nom du drone doit être entre 0 et 40 caractères'
    );
  }

  public validateDistance(): boolean | string {
    return (
      this.droneList.filter(drone => {
        return (
          Math.sqrt(
            Math.pow(drone.startingXPos - this.newDrone.startingXPos, 2) +
              Math.pow(drone.startingYPos - this.newDrone.startingYPos, 2)
          ) < 0.5
        );
      }).length === 0 ||
      'Un drone existant est trop près de la position choisie'
    );
  }

  public validatePosition(v: number): boolean | string {
    return (
      (v <= DroneMenu.MAX_RANGE && v >= -DroneMenu.MAX_RANGE) ||
      'La position du drone doit être dans les limites [-' +
        DroneMenu.MAX_RANGE +
        ', ' +
        DroneMenu.MAX_RANGE +
        ']'
    );
  }

  public validateDroneNameCharacters(v: string): boolean | string {
    return (
      v.match('^[a-zA-Z0-9_:/]*$') !== null ||
      'Le nom du drone contient des caractères invalides'
    );
  }

  public mandatory(v: unknown): boolean | string {
    return !!v || 'Ce champ est obligatoire';
  }
}
</script>
