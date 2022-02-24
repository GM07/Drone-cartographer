<template>
  <div>
    <v-app-bar v-if="isDroneListLoaded" app>
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
        <NavigationCommands :accessStatus="accessStatus" />
        <v-divider></v-divider>
        <DroneCommands
          v-if="isDroneCommandsEnabled()"
          :droneid="getSelectedDrone()"
        />
      </div>

      <template id="bottom" v-slot:append>
        <v-divider></v-divider>
        <MissionCommands
          v-if="isMissionCommandsEnabled()"
          :accessStatus="accessStatus"
        />
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
import {SOCKETIO_LIMITED_ACCESS} from '@/communication/server_constants';
import {AccessStatus} from '@/communication/access_status';

@Component({
  components: {DroneCommands, MissionCommands, NavigationCommands},
})
export default class Mission extends Vue {
  public droneList = ['General'];
  public isDroneListLoaded = false;
  public attemptedLimitedConnexion = false;
  public accessStatus = {
    isMissionSimulated: false,
    isUserControlling: false,
  } as AccessStatus;
  private chosenOption = 0;

  public setSelected(index: number): void {
    this.chosenOption = index;
  }

  public isDroneCommandsEnabled(): boolean {
    return this.chosenOption !== 0 && this.accessStatus.isUserControlling;
  }

  public isMissionCommandsEnabled(): boolean {
    return this.accessStatus.isUserControlling;
  }

  public getSelectedDrone(): string {
    return this.droneList[this.chosenOption];
  }

  private beforeCreate(): void {
    SOCKETIO_LIMITED_ACCESS.on(
      'update_status',
      (accessStatus: AccessStatus) => {
        this.accessStatus = accessStatus;
      }
    );

    SOCKETIO_LIMITED_ACCESS.open();

    ServerCommunication.getConnectedDrones()
      .then(resp => resp.json())
      .then(data => {
        this.droneList.push(...data);
      })
      .catch(error => {
        console.error(error);
      })
      .finally(() => {
        this.isDroneListLoaded = true;
      });
  }

  private destroyed() {
    SOCKETIO_LIMITED_ACCESS.removeAllListeners().close();
  }
}
</script>
