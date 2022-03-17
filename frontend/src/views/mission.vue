<template>
  <div>
    <v-navigation-drawer
      app
      :mini-variant="this.$vuetify.breakpoint.smAndDown && miniVariant"
      permanent
      touchless
    >
      <v-layout column dense fill-height justify-space-between>
        <div id="top">
          <v-list v-if="this.$vuetify.breakpoint.smAndDown" dense>
            <v-list-item @click="miniVariant = !miniVariant">
              <v-list-item-icon>
                <v-icon color="blue">mdi-format-list-bulleted</v-icon>
              </v-list-item-icon>
              <v-list-item-title></v-list-item-title>
            </v-list-item>
          </v-list>

          <NavigationCommands
            :accessStatus="accessStatus"
            :droneList="droneList"
          />
          <v-divider></v-divider>
          <DroneCommands
            v-if="isDroneCommandsEnabled()"
            :accessStatus="accessStatus"
            :droneid="getSelectedDrone()"
          />

          <v-list>
            <v-list-item @click="isLogsMenuOpen = !isLogsMenuOpen">
              <v-list-item-icon>
                <v-icon color="blue">mdi-note-text</v-icon>
              </v-list-item-icon>
              <v-list-item-title v-if="!isLogsMenuOpen"
                >Ouvrir logs</v-list-item-title
              >
              <v-list-item-title v-if="isLogsMenuOpen"
                >Fermer logs</v-list-item-title
              >
            </v-list-item>
          </v-list>
        </div>
        <div id="bottom" justify-end>
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
        </div>
      </v-layout>
    </v-navigation-drawer>

    <v-item-group>
      <v-container fluid style="border-bottom: 1px solid grey">
        <v-row dense no-gutters style="flex-wrap: nowrap; overflow-x: auto">
          <v-col class="d-flex" cols="auto">
            <v-item v-slot="{toggle}">
              <v-card
                :color="chosenOption === -1 ? 'primary' : ''"
                elevation="2"
                @click="
                  toggle();
                  setSelected(-1);
                "
              >
                <v-card-text>General</v-card-text>
              </v-card>
            </v-item>
          </v-col>
          <v-col v-for="(drone, index) in droneList" :key="index" cols="auto">
            <v-item v-slot="{toggle}">
              <v-card
                :color="chosenOption === index ? 'primary' : ''"
                elevation="2"
                @click="
                  toggle();
                  setSelected(index);
                "
              >
                <v-card-actions v-if="isUserControlling()">
                  <v-btn outlined rounded text @click.stop="deleteDrone(index)">
                    <v-icon>mdi-close-circle</v-icon>
                  </v-btn>
                </v-card-actions>
                <v-card-text>{{ drone.name }}</v-card-text>
              </v-card>
            </v-item>
          </v-col>
          <v-col class="d-flex" cols="auto">
            <v-item v-if="isUserControlling()">
              <v-card
                active-class="no-active"
                elevation="2"
                @click.stop="isDroneMenuOpen = true"
              >
                <v-card-text>+</v-card-text>
              </v-card>
            </v-item>
          </v-col>
        </v-row>
      </v-container>
    </v-item-group>

    <drone-menu
      :droneList="droneList"
      :isDroneMenuOpen="isDroneMenuOpen"
      @addDrone="addDrone"
      @setDroneMenuOpen="setDroneMenuOpen"
    ></drone-menu>

    <div
      v-if="isLogsMenuOpen"
      id="LogsInterfaceContainer"
      class="mb-10"
      style="position: absolute; bottom: 0; width: 100%; min-width: 1400px"
    >
      <div id="LogTitle">
        <h3 class="ma-3">Logs</h3>
        <v-icon class="ma-3" color="black" @click="setLogsMenuOpen(false)"
          >mdi-close</v-icon
        >
      </div>
      <div id="logs">
        <LogsInterface />
      </div>
    </div>
  </div>
</template>

<style scoped>
.v-btn--active.no-active:not(:hover)::before {
  opacity: 0 !important;
}

.logs-menu {
  position: absolute;
  bottom: 0;
  max-width: 100vw;
  width: 100%;
}

#LogTitle {
  display: flex;
  flex-direction: row;
  justify-content: space-between;
}

#logs {
  border: 10px;
  float: left;
  width: 100%;
  overflow-x: auto;
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
import LogsInterface from '@/components/logs_interface.vue';
import {ServerCommunication} from '@/communication/server_communication';

@Component({
  components: {
    DroneCommands,
    MissionCommands,
    NavigationCommands,
    DroneMenu,
    LogsInterface,
  },
})
export default class Mission extends Vue {
  public miniVariant = true;
  public attemptedLimitedConnexion = false;
  public dialog = false;
  public isDroneMenuOpen = false;
  public isLogsMenuOpen = false;
  public droneList: Drone[] = [];
  public accessStatus = {
    isMissionSimulated: false,
    isUserControlling: false,
  } as AccessStatus;
  private chosenOption = -1;

  public deleteDrone(index: number): void {
    if (this.chosenOption >= index) {
      this.chosenOption = -1;
    }
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
      (accessStatus: AccessStatus, callback) => {
        this.accessStatus = accessStatus;
        if (callback !== undefined) callback();
      }
    );

    SOCKETIO_LIMITED_ACCESS.on('disconnect', () => {
      this.accessStatus.isUserControlling = false;
    });

    SOCKETIO_LIMITED_ACCESS.on('droneList', (droneList: Drone[]) => {
      this.droneList = droneList;
    });

    SOCKETIO_LIMITED_ACCESS.open();
  }

  private destroyed() {
    SOCKETIO_LIMITED_ACCESS.removeAllListeners().close();
  }
}
</script>
