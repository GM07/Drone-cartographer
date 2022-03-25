<template>
  <v-layout class="main-container" column fill-height justify-space-between>
    <v-navigation-drawer
      app
      :mini-variant="this.$vuetify.breakpoint.smAndDown && miniVariant"
      permanent
      touchless
    >
      <v-layout column dense fill-height justify-space-between>
        <div id="topSidebar">
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
            :droneList="getDrones()"
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
            <v-list-item @click="recompile()">
              <v-list-item-icon>
                <v-icon color="blue">mdi-refresh</v-icon>
              </v-list-item-icon>
              <v-list-item-title>Recompiler</v-list-item-title>
            </v-list-item>
          </v-list>
        </div>
        <div id="bottomSidebar" justify-end>
          <v-divider></v-divider>
          <MissionCommands
            v-if="isUserControlling()"
            :accessStatus="accessStatus"
            :droneList="getDrones()"
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

    <div id="topContent">
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
            <v-col
              v-for="(droneStatus, index) in droneList"
              :key="index"
              cols="auto"
            >
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
                    <v-btn
                      outlined
                      rounded
                      text
                      @click.stop="deleteDrone(index)"
                    >
                      <v-icon>mdi-close-circle</v-icon>
                    </v-btn>
                  </v-card-actions>
                  <v-card-text>{{ droneStatus.drone.name }}</v-card-text>
                  <v-card-text> État: {{ droneStatus.status }}</v-card-text>
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

      <v-select
        v-bind:items="droneNameList"
        label="Sélectionner les cartes individuelles voulues"
        multiple
        no-data-text="Aucun drone disponible"
        style="padding: 20px"
        v-on:input="limitNumMaps"
      ></v-select>
      <div style="display: flex; flex-direction: row">
        <Map v-bind:mapData="mapData1" />
        <Map v-bind:mapData="mapData2" />
      </div>
    </div>

    <drone-menu
      :droneList="getDrones()"
      :isDroneMenuOpen="isDroneMenuOpen"
      @addDrone="addDrone"
      @setDroneMenuOpen="setDroneMenuOpen"
    ></drone-menu>

    <div v-if="isLogsMenuOpen" justify-end style="width: 100%">
      <v-btn @click="isLogsMenuOpen = !isLogsMenuOpen">
        <v-icon color="black"> mdi-close </v-icon>Logs
      </v-btn>
      <LogsInterface />
    </div>
  </v-layout>
</template>

<style scoped>
.v-btn--active.no-active:not(:hover)::before {
  opacity: 0 !important;
}

.main-container {
  min-width: min-content;
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
import Map from '@/components/map.vue';
import DroneMenu from '@/components/drone_menu.vue';
import {
  SOCKETIO_LIMITED_ACCESS,
  SERVER_ADDRESS,
  MAP_DATA_NAMESPACE,
  SOCKETIO_DRONE_STATUS,
  SOCKETIO_RECOMPILE_SIMULATION,
  SOCKETIO_RECOMPILE_EMBEDDED,
} from '@/communication/server_constants';
import {AccessStatus} from '@/communication/access_status';
import {Drone, DroneStatus} from '@/communication/drone';
import LogsInterface from '@/components/logs_interface.vue';
import {ServerCommunication} from '@/communication/server_communication';
import SocketIO from 'socket.io-client';
import {MapData, EMPTY_MAP} from '@/utils/map_constants';

@Component({
  components: {
    DroneCommands,
    MissionCommands,
    NavigationCommands,
    DroneMenu,
    Map,
    LogsInterface,
  },
})
export default class Mission extends Vue {
  readonly SOCKETIO = SocketIO(SERVER_ADDRESS + MAP_DATA_NAMESPACE, {
    transports: ['websocket', 'polling'],
  });

  public miniVariant = true;
  public attemptedLimitedConnexion = false;
  public dialog = false;
  public isDroneMenuOpen = false;
  public isLogsMenuOpen = false;
  public droneList: DroneStatus[] = [];
  public accessStatus = {
    isMissionSimulated: false,
    isUserControlling: false,
  } as AccessStatus;

  public mapData1: MapData = EMPTY_MAP;
  public mapData2: MapData = EMPTY_MAP;

  private droneNameList: string[] = [];
  private visualizedMaps: string[] = [];

  private chosenOption = -1;

  constructor() {
    super();
    this.SOCKETIO.on('getMapData', data => {
      this.changeData(data);
    });
  }

  public changeData(data: MapData): void {
    const BASE_PATH = this.accessStatus.isMissionSimulated
      ? '/tmp/socket/data'
      : '';

    if (data.id === BASE_PATH + this.visualizedMaps[0]) this.mapData1 = data;
    else if (data.id === BASE_PATH + this.visualizedMaps[1])
      this.mapData2 = data;
  }

  public recompile(): void {
    if (this.accessStatus.isUserControlling) ServerCommunication.recompile();
  }

  public limitNumMaps(input: string[]): void {
    if (input.length > 2) {
      input.pop();
    }
    this.visualizedMaps = input;
  }

  public deleteDrone(index: number): void {
    if (this.chosenOption >= index) {
      this.chosenOption = -1;
    }
    this.droneList.splice(index, 1);

    ServerCommunication.setDrone(
      this.getDrones(),
      this.accessStatus.isMissionSimulated
    );
  }

  public setSelected(index: number): void {
    this.chosenOption = index;
  }

  public setDroneMenuOpen(value: boolean): void {
    this.isDroneMenuOpen = value;
  }

  public getDrones(): Drone[] {
    return this.droneList.map(droneStatus => droneStatus.drone);
  }

  public addDrone(drone: Drone): void {
    this.droneNameList.push(drone.name);
    this.droneList.push({drone: {...drone}, status: 'IDLE'});
    ServerCommunication.setDrone(
      this.getDrones(),
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
      return this.droneList[this.chosenOption].drone.name;
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

    SOCKETIO_RECOMPILE_SIMULATION.on(
      'recompile_simulation',
      (stdout: string) => {
        console.log(stdout);
      }
    );
    SOCKETIO_RECOMPILE_EMBEDDED.on('recompile_embedded', (stdout: string) => {
      console.log(stdout);
    });

    SOCKETIO_LIMITED_ACCESS.on('disconnect', () => {
      this.accessStatus.isUserControlling = false;
    });

    SOCKETIO_DRONE_STATUS.on(
      'update_drone_status',
      (statusList: Array<[string, string]>) => {
        statusList.forEach(statusTuple => {
          for (let i = 0; i < this.droneList.length; i++) {
            console.log(this.droneList[i].drone.name, statusTuple[0]);
            if (this.droneList[i].drone.name === statusTuple[0])
              this.droneList[i].status = statusTuple[1];
          }
        });
      }
    );

    SOCKETIO_LIMITED_ACCESS.on('droneList', (droneList: Drone[]) => {
      this.droneList = droneList.map(droneValue => {
        return {drone: droneValue, status: 'IDLE'} as DroneStatus;
      });
    });

    SOCKETIO_LIMITED_ACCESS.open();
    SOCKETIO_DRONE_STATUS.open();
    SOCKETIO_RECOMPILE_SIMULATION.open();
    SOCKETIO_RECOMPILE_EMBEDDED.open();
  }

  private destroyed() {
    SOCKETIO_LIMITED_ACCESS.removeAllListeners().close();
    SOCKETIO_DRONE_STATUS.removeAllListeners().close();
    SOCKETIO_RECOMPILE_SIMULATION.removeAllListeners().close();
    SOCKETIO_RECOMPILE_EMBEDDED.removeAllListeners().close();
  }
}
</script>
