<template>
  <div>
    <v-card style="min-width: min-content">
      <v-app-bar
        id="scrolling-techniques-4"
        class="app"
        dense
        fade-img-on-scroll
        hide-on-scroll
      >
        <v-col>
          <v-toolbar-title>Historique</v-toolbar-title>
        </v-col>

        <div class="mr-16" style="float: right">
          <template>
            <p
              class="mt-6"
              style="
                color: black;
                float: left;

                white-space: nowrap;
              "
            >
              Recherche de missions par date
            </p>
            <v-menu
              ref="menu"
              v-model="isSearchMenuOpen"
              :close-on-content-click="false"
              min-width="auto"
              offset-y
              :return-value.sync="date"
              style="float: right; white-space: nowrap"
              transition="scale-transition"
            >
              <template v-slot:activator="{on, attrs}">
                <v-text-field
                  v-model="date"
                  v-bind="attrs"
                  class="mt-5"
                  prepend-icon="mdi-calendar"
                  style="float: right"
                  v-on="on"
                  @click:prepend="() => (isSearchMenuOpen = true)"
                ></v-text-field>
              </template>

              <v-date-picker v-model="date" scrollable style="float: right">
                <v-spacer></v-spacer>
                <v-btn color="primary" text @click="isSearchMenuOpen = false">
                  Annuler
                </v-btn>
                <v-btn
                  color="primary"
                  text
                  @click="
                    $refs.menu.save(date);
                    filterByDate();
                  "
                >
                  Filtrer par date
                </v-btn>
              </v-date-picker>
            </v-menu>
          </template>
        </div>

        <template v-slot:extension>
          <div id="tabs">
            <v-tabs v-model="currentTab">
              <v-tab @click="changeFilterMode(filter.all)">Tous</v-tab>
              <v-tab @click="changeFilterMode(filter.simulated)"
                >Simulation</v-tab
              >
              <v-tab @click="changeFilterMode(filter.physical)">Embarqué</v-tab>
              <v-tab @click="changeFilterMode(filter.date)">Date</v-tab>
              <v-spacer></v-spacer>
            </v-tabs>
          </div>
        </template>
      </v-app-bar>

      <v-sheet class="overflow-y-auto pa-4" style="min-width: min-content">
        <div
          v-if="showedMissions.length != 0"
          class="ma-1"
          style="float: right; min-width: min-content"
        >
          <p class="ma-2" style="color: black; float: left">
            Triage des missions affichées
          </p>

          <v-icon v-if="isAscending" class="ma-2" @click="swapOrder()">
            mdi-arrow-down
          </v-icon>
          <v-icon v-else class="ma-2" @click="swapOrder()">
            mdi-arrow-up
          </v-icon>

          <v-select
            v-model="currentFilter"
            attach
            auto
            class="pa-0"
            :items="sorts"
            scrollable
            style="max-width: 300px; float: right; display: flex"
            @change="sortMissions()"
          ></v-select>
        </div>
        <div v-for="item in showedMissions" :key="item._id" class="ma-5">
          <v-card
            class="pa-5"
            elevation="4"
            outlined
            style="width: 100%; min-width: fit-content; overflow-x: auto"
          >
            <v-list-item-title v-if="item.is_simulated" class="text-h6 mb-3">
              Mission simulée
            </v-list-item-title>
            <v-list-item-title v-else class="text-h6 mb-3">
              Mission physique
            </v-list-item-title>

            <v-list-item-title class="text-h6 mb-3">
              Date de complétion: {{ item.time_stamp }}
            </v-list-item-title>
            <p class="mb-3">
              <strong> Temps de vol:</strong>
              {{ parseFloat(item.flight_duration).toFixed(3) }} s
            </p>
            <p class="mb-3">
              <strong> Nombre de drones:</strong> {{ item.number_of_drones }}
            </p>

            <p class="mb-3">
              <strong> Distance parcourue totale:</strong>
              {{ parseFloat(item.total_distance).toFixed(3) }} m
            </p>

            <div
              style="
                display: flex;
                flex-direction: row;

                width: 100%;
                justify-content: space-between;
              "
            >
              <v-btn
                v-if="openMapId != item._id"
                class="ma-2"
                color="indigo"
                outlined
                @click="setMissionMaps(true, item)"
              >
                Afficher carte
              </v-btn>
              <v-btn
                v-if="openMapId == item._id"
                class="ma-2"
                color="indigo"
                outlined
                @click="setMissionMaps(false, item)"
              >
                Fermer carte
              </v-btn>
              <div v-if="openMapId == item._id">
                <v-btn
                  v-for="(map, index) in item.maps"
                  :key="map"
                  @click="indexDrone = index"
                >
                  <p v-if="index == 0">Carte générale</p>
                  <p v-else>Carte du drone no {{ index }}</p>
                </v-btn>
                <div
                  class="ma-10"
                  style="
                    display: flex;
                    flex-direction: column;
                    align-items: center;
                  "
                >
                  <h2 style="color: DimGray">Carte Générale</h2>

                  <img class="Map" :src="item.map" />
                </div>
              </div>

              <v-btn
                v-if="openLogId != item._id"
                class="ma-2"
                color="indigo"
                outlined
                @click="setLogsMenu(true, item)"
              >
                Afficher logs
              </v-btn>
              <v-btn
                v-if="openLogId === item._id"
                class="ma-2"
                color="indigo"
                outlined
                @click="setLogsMenu(false)"
              >
                Fermer logs
              </v-btn>
            </div>

            <v-virtual-scroll
              v-if="openLogId === item._id"
              id="scroll"
              :elevation="20"
              height="300"
              item-height="20"
              :items="item.logs"
              style="min-width: 100%; overflow-x: scroll; white-space: nowrap"
            >
              <template v-slot:default="{item}">
                <v-list-item :key="item[0]">
                  <p style="display: inline-block; white-space: nowrap">
                    {{ item[0] }} {{ item[1] }}
                  </p>
                </v-list-item>
              </template>
            </v-virtual-scroll>
          </v-card>
        </div>
      </v-sheet>
    </v-card>
    <v-navigation-drawer
      app
      :mini-variant="this.$vuetify.breakpoint.smAndDown && miniVariant"
      permanent
      touchless
    >
      <div id="top">
        <v-list v-if="this.$vuetify.breakpoint.smAndDown" dense>
          <v-list-item @click="miniVariant = !miniVariant">
            <v-list-item-icon>
              <v-icon color="blue">mdi-format-list-bulleted</v-icon>
            </v-list-item-icon>
            <v-list-item-title></v-list-item-title>
          </v-list-item>
        </v-list>
        <v-list-item to="/">
          <v-list-item-icon>
            <v-icon color="blue">mdi-home</v-icon>
          </v-list-item-icon>
          <v-list-item-title>Menu principal</v-list-item-title>
        </v-list-item>
        <v-divider></v-divider>
      </div>

      <template id="bottom" v-slot:append>
        <v-divider></v-divider>
      </template>
    </v-navigation-drawer>
  </div>
</template>

<style scoped>
.app >>> .v-toolbar__content {
  padding-right: 0px !important;
}
.v-menu__content {
  z-index: 100 !important;
}

.v-select-list {
  z-index: 100 !important;
}
.animated {
  animation-fill-mode: none;
}
.Map {
  border: 1px solid black;
}

#tabs {
  filter: invert(100%) sepia(85%) saturate(78%) hue-rotate(203deg)
    brightness(112%) contrast(92%);
}
</style>

<script lang="ts">
import {Component, Vue} from 'vue-property-decorator';
import {ServerCommunication} from '@/communication/server_communication';
import {Mission} from '@/utils/mission';
import {TIME_MULTIPLIER} from '@/communication/server_constants';
import Map from '@/components/map.vue';
import {MapData} from '@/utils/map_constants';
enum Filters {
  all,
  simulated,
  physical,
  date,
}

@Component({components: {Map}})
export default class CompletedMissions extends Vue {
  private missions: Mission[] = [];
  private showedMissions: Mission[] = [];
  private currentMap = '';
  private indexDrone = 0;
  private openMapId = ' ';
  private miniVariant = true;
  private mode: Filters = Filters.all;
  private isSearchMenuOpen = false;
  private currentTab = 0;
  private openLogId = ' ';
  private droneIndex = 0;
  private filter = Filters;
  private currentFilter = 'Aucun';
  private isAscending = true;
  private baseMap: MapData[] = [];
  private sorts = [
    'Aucun',
    'Nombre de drones',
    'Distance parcourue',
    'Temps de vol',
    'Date',
  ];

  private date: string = new Date(
    Date.now() - new Date().getTimezoneOffset() * TIME_MULTIPLIER
  )
    .toISOString()
    .substr(0, 10);

  private changeFilterMode(mode: Filters): void {
    this.mode = mode;
    this.openLogId = '';
    this.openMapId = '';
    this.getCompletedMissions().then(() => {
      this.updateFilteredMissions();
      this.sortMissions();
    });
  }

  private updateFilteredMissions(): void {
    switch (this.mode) {
      case Filters.all:
        this.showedMissions = this.missions;
        break;

      case Filters.simulated:
        this.showedMissions = this.missions.filter(
          (mission: Mission) => mission.is_simulated
        );
        break;

      case Filters.physical:
        this.showedMissions = this.missions.filter(
          (mission: Mission) => !mission.is_simulated
        );
        break;

      case Filters.date:
        this.showedMissions = this.missions.filter(
          (mission: Mission) =>
            mission.time_stamp.substring(0, 10) === this.date
        );
        break;
    }
  }

  private beforeCreate() {
    ServerCommunication.getCompletedMissions()
      .then(res => res.json())
      .then(data => {
        this.missions = data;
        this.showedMissions = this.missions;
      });
  }

  private async getCompletedMissions(): Promise<void> {
    return ServerCommunication.getCompletedMissions()
      .then(res => res.json())
      .then(data => {
        this.missions = data;
      });
  }

  private filterByDate(): void {
    const DATE_TAB_INDEX = 3;
    this.currentTab = DATE_TAB_INDEX;
    this.changeFilterMode(Filters.date);
  }

  private sortMissions(): void {
    switch (this.currentFilter) {
      case this.sorts[0]:
        this.updateFilteredMissions();
        break;

      case this.sorts[1]:
        this.showedMissions.sort((a: Mission, b: Mission) => {
          return this.sortMissionNumberParameters(
            a.number_of_drones,
            b.number_of_drones
          );
        });
        break;

      case this.sorts[2]:
        this.showedMissions.sort((a: Mission, b: Mission) => {
          return this.sortMissionNumberParameters(
            a.total_distance,
            b.total_distance
          );
        });
        break;

      case this.sorts[3]:
        this.showedMissions.sort((a: Mission, b: Mission) => {
          return this.sortMissionNumberParameters(
            a.flight_duration,
            b.flight_duration
          );
        });
        break;

      case this.sorts[4]:
        this.showedMissions.sort((a: Mission, b: Mission) => {
          if (this.isAscending) return a.time_stamp < b.time_stamp ? -1 : 1;
          else return a.time_stamp > b.time_stamp ? -1 : 1;
        });
        break;
    }
  }

  private sortMissionNumberParameters(a: number, b: number): number {
    return this.isAscending ? a - b : b - a;
  }

  private swapOrder() {
    this.isAscending = !this.isAscending;
    this.sortMissions();
  }

  private setLogsMenu(logs: boolean, item: Mission) {
    if (logs) {
      if (item.logs !== undefined) {
        this.openLogId = item._id;
        return;
      }
      this.getMissionLogs(item).then((mission: Mission) => {
        item.logs = mission.logs;
        this.openLogId = mission._id;
      });
    } else this.openLogId = ' ';
  }

  private getMissionLogs(item: Mission): Promise<Mission> {
    return ServerCommunication.getSpecificMissionLogs(item._id)
      .then(res => res.json())
      .then((data: Mission) => {
        return data;
      });
  }
  private setMissionMaps(setMenu: boolean, item: Mission) {
    if (setMenu) {
      if (item.map !== undefined) {
        this.openMapId = item._id;

        this.$nextTick(() => {
          this.currentMap = item.map;
        });
        return;
      }
      this.getMissionMaps(item).then((mission: Mission) => {
        this.openMapId = mission._id;
        item.map = mission.map;

        this.$nextTick(() => {
          this.currentMap = mission.map;
        });
      });
    } else {
      this.openMapId = ' ';
      this.currentMap = ' ';
    }
  }

  private getMissionMaps(item: Mission): Promise<Mission> {
    return ServerCommunication.getSpecificMissionMaps(item._id)
      .then(res => res.json())
      .then((data: Mission) => {
        return data;
      });
  }
}
</script>
