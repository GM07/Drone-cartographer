<template>
  <div
    id="content-drone-data-card"
    :class="selected ? 'selected' : 'unselected'"
    @click="$emit('click')"
  >
    <div id="topBar">
      <div id="batterie">
        <v-icon
          :class="selected ? 'selected-image' : 'unselected-image'"
          medium
        >
          {{ computeBatteryIconOutput() }}
        </v-icon>

        <p id="batterie-text" class="stateInfo">
          {{ (droneData.batteryLevel * 100).toFixed(0) }}%
        </p>
      </div>
      <v-btn
        v-if="userControlling && !getMissionStatus()"
        :color="selected ? 'white' : '#2196f3'"
        icon
        x-small
        @click.stop="$emit('deleteDrone')"
      >
        <v-icon>mdi-close-circle</v-icon>
      </v-btn>
    </div>
    <div
      id="drone-image-container"
      :class="selected ? 'selected-image' : 'unselected-image'"
    >
      <div id="drone-image">
        <img src="../assets/drone.svg" />
      </div>
    </div>
    <div id="info">
      <p class="stateInfo">{{ droneData.name + '\n' }}</p>
      <p class="stateInfo">État: {{ droneData.state + '\n' }}</p>
      <p class="stateInfo">
        {{
          '\nPosition : (' +
          droneData.xPos.toFixed(2) +
          ' , ' +
          droneData.yPos.toFixed(2) +
          ')'
        }}
      </p>
    </div>
  </div>
</template>

<style lang="scss">
#-drone-data-card {
  border-right: 1px solid #1976d211;
  border-top: 1px solid #1976d211;
  border-bottom: 1px solid #1976d211;
}

.selected {
  background-color: #2196f3;
  color: white !important;
}

.unselected {
  background-color: white !important;
  color: black !important;
}

.unselected:hover {
  background-color: #edf6fd !important;
  cursor: pointer;
}

#info {
  padding-left: 48px;
  padding-right: 48px;
  align-items: center;
}

#divider {
  padding: 12px 12px;
  color: #1976d211;
}

#drone-image {
  width: 40px;
  height: 40px;
}

#drone-image-container {
  display: flex;
  align-items: center;
  justify-content: center;
  text-align: center;
  padding: 12px 12px;
}

.unselected-image {
  filter: invert(0%);
}

.selected-image {
  filter: invert(100%);
}

#content-drone-data-card {
  display: flex;
  min-height: 54px; // Size of General button
  min-width: 100px;
  flex-direction: column;
  text-align: center;
}

#batterie {
  display: flex;
  justify-items: center;
  margin-bottom: 0px !important;
}

#batterie-text {
  display: flex;
  justify-items: center;
  align-items: center;
  margin-bottom: 0px !important;
}

#topBar {
  padding-left: 12px;
  padding-right: 12px;
  padding-top: 12px;
  display: flex;
  justify-content: space-between;
  align-items: center;
}

.stateInfo {
  text-align: center;
  font-size: 12px;
  margin-bottom: 8px !important;
}
</style>

<script lang="ts">
import Vue from 'vue';
import Component from 'vue-class-component';
import {Prop, Watch} from 'vue-property-decorator';
import {DroneData} from '@/communication/drone';
import {ACCESSOR} from '@/store/index';

@Component({})
export default class DroneDataCard extends Vue {
  @Prop() droneData!: DroneData;
  @Prop() selected;
  @Prop() userControlling;

  constructor() {
    super();
  }

  @Watch('droneData.batteryLevel')
  public computeBatteryIconOutput(): string {
    const ICON = 'mdi-battery';
    const MAX_VALUE = 100;
    const STEP = 10;
    const BATTERY = this.droneData.batteryLevel * MAX_VALUE;

    if (BATTERY > MAX_VALUE - STEP) {
      return ICON;
    }

    if (BATTERY < STEP) {
      return 'mdi-battery-outline';
    }

    console.log('fixed : ', +Math.floor(BATTERY / STEP).toFixed(0) * STEP);
    const UPDATED_ICON =
      ICON + '-' + +Math.floor(BATTERY / STEP).toFixed(0) * STEP;
    console.log(UPDATED_ICON);
    return UPDATED_ICON;
  }

  public getMissionStatus(): boolean {
    return ACCESSOR.missionStatus.isMissionStarted;
  }
}
</script>
