<template>
  <div>
    <v-app-bar app>
      <v-btn-toggle mandatory>
        <v-btn v-on:click="setSelected(-1)"> General </v-btn>
        <div v-for="(drone, index) in droneList" :key="index">
          <v-btn v-on:click="setSelected(index)">
            {{ drone.name }}
          </v-btn>
          <v-icon
            size="12"
            style="
              position: absolute;
              z-index: 1;
              margin-left: -13px;
              margin-top: 2px;
            "
            @click="deleteDrone(index)"
            >mdi-close-circle</v-icon
          >
        </div>
      </v-btn-toggle>
      <v-btn-toggle>
        <v-btn
          v-if="isUserControlling()"
          active-class="no-active"
          @click.stop="isDroneMenuOpen = true"
        >
          +
        </v-btn>
      </v-btn-toggle>
    </v-app-bar>

    <drone-menu
      :droneList="droneList"
      :isDroneMenuOpen="isDroneMenuOpen"
      @addDrone="addDrone"
      @setDroneMenuOpen="setDroneMenuOpen"
    ></drone-menu>

    <v-navigation-drawer app permanent touchless>
      <div id="top">
        <NavigationCommands
          :accessStatus="accessStatus"
          :droneList="droneList"
        />
        <v-divider></v-divider>
        <DroneCommands
          v-if="isDroneCommandsEnabled()"
          :droneid="getSelectedDrone()"
        />
      </div>

      <template id="bottom" v-slot:append>
        <v-divider></v-divider>
        <MissionCommands
          v-if="isUserControlling()"
          :accessStatus="accessStatus"
          :droneList="droneList"
        />
        <v-list dense nav>
          <v-list-item v-if="isConnected()">
            <v-list-item-icon>
              <v-icon color="blue">mdi-access-point-network</v-icon>
            </v-list-item-icon>
            <v-list-item-title>Connecté</v-list-item-title>
          </v-list-item>
          <v-list-item v-else>
            <v-list-item-icon>
              <v-icon color="red">mdi-access-point-network-off</v-icon>
            </v-list-item-icon>
            <v-list-item-title>Déconnecté</v-list-item-title>
          </v-list-item>
        </v-list>
      </template>
    </v-navigation-drawer>
  </div>
</template>

<style scoped>
.v-btn--active.no-active:not(:hover)::before {
  opacity: 0 !important;
}
</style>

<script lang="ts">
import {Component, Vue} from 'vue-property-decorator';
import DroneCommands from '@/components/drone_commands.vue';
import MissionCommands from '@/components/mission_commands.vue';
import NavigationCommands from '@/components/navigation_commands.vue';
import DroneMenu from '@/components/drone_menu.vue';
import {SOCKETIO_LIMITED_ACCESS} from '@/communication/server_constants';
import {AccessStatus} from '@/communication/access_status';
import {Drone} from '@/communication/drone';
import {ServerCommunication} from '@/communication/server_communication';

@Component({
  components: {DroneCommands, MissionCommands, NavigationCommands, DroneMenu},
})
export default class Mission extends Vue {
  public droneList: Drone[] = [];
  public attemptedLimitedConnexion = false;
  public dialog = false;
  public isDroneMenuOpen = false;
  public accessStatus = {
    isMissionSimulated: false,
    isUserControlling: false,
  } as AccessStatus;
  private chosenOption = -1;

  public deleteDrone(index: number): void {
    this.droneList.splice(index, 1);
    ServerCommunication.setDrone(
      this.droneList,
      this.accessStatus.isMissionSimulated
    );
  }

  public setSelected(index: number): void {
    this.chosenOption = index;
  }

  public setDroneMenuOpen(value: boolean): void {
    this.isDroneMenuOpen = value;
  }

  public addDrone(drone: Drone): void {
    this.droneList.push({...drone});
    ServerCommunication.setDrone(
      this.droneList,
      this.accessStatus.isMissionSimulated
    );
  }

  public isDroneCommandsEnabled(): boolean {
    return this.chosenOption !== -1 && this.accessStatus.isUserControlling;
  }

  public isUserControlling(): boolean {
    return this.accessStatus.isUserControlling;
  }

  public getSelectedDrone(): string {
    if (this.chosenOption !== -1 && this.droneList.length > 0) {
      return this.droneList[this.chosenOption].name;
    }
    return '';
  }

  public isConnected(): boolean {
    return SOCKETIO_LIMITED_ACCESS.connected;
  }

  private beforeCreate(): void {
    SOCKETIO_LIMITED_ACCESS.on(
      'update_status',
      (accessStatus: AccessStatus) => {
        this.accessStatus = accessStatus;
      }
    );

    SOCKETIO_LIMITED_ACCESS.on('disconnect', () => {
      this.accessStatus.isUserControlling = false;
    });

    SOCKETIO_LIMITED_ACCESS.open();
  }

  private destroyed() {
    SOCKETIO_LIMITED_ACCESS.removeAllListeners().close();
  }
}
</script>
