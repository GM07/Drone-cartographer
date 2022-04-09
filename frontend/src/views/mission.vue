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

          <NavigationCommands :accessStatus="accessStatus" />
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
        <div id="bottomSidebar" justify-end>
          <v-divider></v-divider>
          <MissionCommands
            v-if="isUserControlling()"
            :accessStatus="accessStatus"
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
              v-for="(droneData, index) in droneList"
              :key="index"
              cols="auto"
            >
              <v-item v-slot="{toggle}">
                <v-card
                  class="card-container"
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
                  <v-card-text class="pt-1 pb-1">
                    {{ droneData.name }}
                  </v-card-text>
                  <v-card-text class="pt-1 pb-1">
                    État: {{ droneData.state }}
                  </v-card-text>
                  <v-card-text class="pt-1 pb-1">
                    Batterie:
                    {{ (droneData.batteryLevel * 100).toFixed(2) }}%
                  </v-card-text>
                  <v-card-text class="pt-1 pb-1">
                    Position en x:
                    {{ droneData.xPos.toFixed(4) }}
                  </v-card-text>
                  <v-card-text class="pt-1 pb-1">
                    Position en y:
                    {{ droneData.yPos.toFixed(4) }}
                  </v-card-text>
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

      <div>
        <Map
          :droneList="droneList"
          :indexDrone="indexDrone"
          :mapData="mapData"
          :mapName="mapName"
          :missionLaunched="missionLaunched"
        />
      </div>
    </div>

    <DroneMenu
      :droneList="droneList"
      :isDroneMenuOpen="isDroneMenuOpen"
      @addDrone="addDrone"
      @setDroneMenuOpen="setDroneMenuOpen"
    />

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

.card-container {
  min-width: 205px;
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
  SOCKETIO_MAP_DATA,
} from '@/communication/server_constants';
import {AccessStatus} from '@/communication/access_status';
import {
  DroneData,
  NewDroneData,
  DEFAULT_DRONE_DATA,
} from '@/communication/drone';
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
  public droneList: DroneData[] = [];
  public accessStatus = {
    isMissionSimulated: false,
    isUserControlling: false,
  } as AccessStatus;

  private mapData: MapData[][] = [[EMPTY_MAP]];
  private indexDrone = 0;
  private mapName = 'générale';
  private missionLaunched = false;

  private chosenOption = -1;

  constructor() {
    super();
    this.SOCKETIO.on('getMapData', data => {
      this.changeData(data);
    });
  }

  public changeData(data: MapData): void {
    this.droneList.forEach(() => this.mapData.push([EMPTY_MAP]));
    const TEMP_MAP_DATA: MapData[][] = this.fillEmptyMap();

    for (let j = 0; j < this.mapData.length; j++) {
      TEMP_MAP_DATA[j].push(...this.mapData[j]);
    }

    if (TEMP_MAP_DATA[0][0].id === '') TEMP_MAP_DATA[0][0] = data;
    else TEMP_MAP_DATA[0].push(data);

    for (let i = 0; i < this.droneList.length; i++) {
      if (this.droneList[i].name === data.id) {
        if (TEMP_MAP_DATA[++i][0].id === '') TEMP_MAP_DATA[i][0] = data;
        else TEMP_MAP_DATA[i].push(data);
      }
    }
    this.mapData = TEMP_MAP_DATA;
  }

  public fillEmptyMap(): MapData[][] {
    const MAP_DATA: MapData[][] = [];
    for (let i = 0; i < this.mapData.length; i++) {
      MAP_DATA[i] = [EMPTY_MAP];
    }
    return MAP_DATA;
  }

  public deleteDrone(index: number): void {
    if (this.chosenOption >= index) {
      this.chosenOption = -1;
    }
    this.droneList.splice(index, 1);

    ServerCommunication.setDrone(
      this.getNewDrones(),
      this.accessStatus.isMissionSimulated
    );
  }

  public setSelected(index: number): void {
    this.mapData[this.indexDrone] = [EMPTY_MAP];

    this.chosenOption = index;
    this.indexDrone = index + 1;

    this.mapName =
      this.indexDrone !== 0
        ? this.droneList[this.chosenOption].name
        : 'générale';
  }

  public setDroneMenuOpen(value: boolean): void {
    this.isDroneMenuOpen = value;
  }

  public getNewDrones(): NewDroneData[] {
    return this.droneList.map(droneData => {
      return {
        name: droneData.name,
        startingXPos: droneData.startingXPos,
        startingYPos: droneData.startingYPos,
        startingOrientation: droneData.startingOrientation,
      } as NewDroneData;
    });
  }

  public addDrone(drone: NewDroneData): void {
    this.droneList.push({...DEFAULT_DRONE_DATA, ...drone} as DroneData);
    this.droneList[this.droneList.length - 1].xPos = drone.startingXPos;
    this.droneList[this.droneList.length - 1].yPos = drone.startingYPos;
    this.setStartingPoint(drone, this.droneList.length - 1);
    ServerCommunication.setDrone(
      this.getNewDrones(),
      this.accessStatus.isMissionSimulated
    );

    this.mapData.push([EMPTY_MAP]);
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

  private setStartingPoint(newDroneData: NewDroneData, index: number) {
    this.droneList[index].startingXPos = newDroneData.startingXPos;
    this.droneList[index].startingYPos = newDroneData.startingYPos;
    this.droneList[index].startingOrientation =
      newDroneData.startingOrientation;
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

    SOCKETIO_DRONE_STATUS.on(
      'update_drone_status',
      (droneDataList: Array<DroneData>) => {
        droneDataList.forEach(droneData => {
          for (let i = 0; i < this.droneList.length; i++) {
            if (this.droneList[i].name === droneData.name) {
              Vue.set(this.droneList, i, {
                ...this.droneList[i],
                ...droneData,
              } as DroneData);
            }
          }
        });
      }
    );

    SOCKETIO_LIMITED_ACCESS.on('droneList', (droneList: Array<DroneData>) => {
      this.droneList = droneList.map(droneValue => {
        return {...DEFAULT_DRONE_DATA, ...droneValue} as DroneData;
      });
    });

    SOCKETIO_MAP_DATA.on('getAllMapData', res => {
      console.log('map');
      const TEMP_MAP_DATA: MapData[][] = [[EMPTY_MAP]];
      this.droneList.forEach(() => TEMP_MAP_DATA.push([EMPTY_MAP]));
      this.mapData = TEMP_MAP_DATA;
      res.forEach((droneData: MapData) => {
        this.changeData(droneData);
      });
    });

    SOCKETIO_MAP_DATA.on('clear_all_maps', isMissionLaunched => {
      if (isMissionLaunched) {
        const TEMP_MAP_DATA: MapData[][] = [[EMPTY_MAP]];
        this.droneList.forEach(() => TEMP_MAP_DATA.push([EMPTY_MAP]));
        this.mapData = TEMP_MAP_DATA;
      }
      this.missionLaunched = isMissionLaunched;
    });

    SOCKETIO_LIMITED_ACCESS.open();
    SOCKETIO_DRONE_STATUS.open();
    SOCKETIO_MAP_DATA.open();
  }

  private destroyed() {
    SOCKETIO_LIMITED_ACCESS.removeAllListeners().close();
    SOCKETIO_DRONE_STATUS.removeAllListeners().close();
    SOCKETIO_MAP_DATA.removeAllListeners().close();
  }
}
</script>
