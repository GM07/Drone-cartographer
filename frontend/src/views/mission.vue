<template>
  <div>
    <v-app-bar v-if="isLoaded" app>
      <v-btn-toggle mandatory>
        <v-btn
          v-for="(drone, index) in droneList"
          :key="index"
          v-on:click="setSelected(index)"
        >
          {{ drone }}
        </v-btn>
      </v-btn-toggle>
    </v-app-bar>

    <v-navigation-drawer app permanent touchless>
      <div id="top">
        <NavigationCommands />
        <v-divider></v-divider>
        <DroneCommands
          v-if="isDroneSelected"
          :droneid="this.getSelectedDrone()"
        />
      </div>

      <template id="bottom" v-slot:append>
        <v-divider></v-divider>
        <MissionCommands />
      </template>
    </v-navigation-drawer>
  </div>
</template>

<script lang="ts">
import {Component, Vue} from 'vue-property-decorator';
import {ServerCommunication} from '@/communication/server_communication';
import DroneCommands from '@/components/drone_commands.vue';
import MissionCommands from '@/components/mission_commands.vue';
import NavigationCommands from '@/components/navigation_commands.vue';

@Component({
  components: {DroneCommands, MissionCommands, NavigationCommands},
})
export default class Mission extends Vue {
  public droneList = ['General'];
  public isLoaded = false;
  public isDroneSelected = false;
  private chosenOption = 0;

  public setSelected(index: number): void {
    this.chosenOption = index;
    this.isDroneSelected = index !== 0;
  }

  public getSelectedDrone(): string {
    return this.droneList[this.chosenOption];
  }

  private beforeCreate(): void {
    ServerCommunication.getConnectedDrones()
      .then(resp => resp.json())
      .then(data => {
        this.droneList.push(...data);
      })
      .catch(error => {
        console.error(error);
      })
      .finally(() => {
        this.isLoaded = true;
      });
  }
}
</script>
