<template>
  <div style="min-width: 1400px">
    <v-card>
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

        <div style="float: right; margin-right: 150px">
          <template>
            <p style="color: black; float: left; margin: 5px">
              Recherche de missions par date
            </p>
            <v-menu
              ref="menu"
              v-model="isSearchMenuOpen"
              :close-on-content-click="false"
              min-width="auto"
              offset-y
              :return-value.sync="date"
              style="width: 200px; float: right"
              transition="scale-transition"
            >
              <template v-slot:activator="{on, attrs}">
                <v-text-field
                  v-model="date"
                  v-bind="attrs"
                  prepend-icon="mdi-calendar"
                  style="width: 200px,float:right"
                  v-on="on"
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
          <v-tabs v-model="currentTab">
            <v-tab @click="changeFilterMode(filter.all)">Tous</v-tab>
            <v-tab @click="changeFilterMode(filter.simulated)"
              >Simulation</v-tab
            >
            <v-tab @click="changeFilterMode(filter.physical)">Embarqué</v-tab>
            <v-tab @click="changeFilterMode(filter.date)">Date</v-tab>
            <v-spacer></v-spacer>
            <div style="margin-right: 150px">
              <p style="color: black; float: left; margin-top: 5px">
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
                :items="sorts"
                scrollable
                style="width: 200px; float: right"
                @change="sortMissions()"
              ></v-select>
            </div>
          </v-tabs>
        </template>
      </v-app-bar>

      <v-sheet
        id="scrolling-techniques-4"
        class="overflow-y-auto pa-4"
        min-height="96px"
      >
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
            <div
              style="
                display: flex;
                flex-direction: row;

                width: 100%;
                justify-content: space-between;
              "
            >
              <v-btn class="ma-2" color="indigo" outlined>
                Plus d'informations
              </v-btn>
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
              style="min-width: 1400px; overflow-x: auto"
            >
              <template v-slot:default="{item}">
                <v-list-item :key="item[0]">
                  <v-list-item-content>
                    <p>{{ item[0] }} {{ item[1] }}</p>
                  </v-list-item-content>
                </v-list-item>
              </template>
            </v-virtual-scroll>
          </v-card>
        </div>
      </v-sheet>
    </v-card>
    <v-navigation-drawer app permanent touchless>
      <div id="top">
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
</style>

<script lang="ts">
import {Component, Vue} from 'vue-property-decorator';
import {ServerCommunication} from '@/communication/server_communication';
import NavigationCommands from '@/components/navigation_commands.vue';
import {Mission} from '@/utils/mission';
import {TIME_MULTIPLIER} from '@/communication/server_constants';

enum Filters {
  all,
  simulated,
  physical,
  date,
}

@Component({components: {NavigationCommands}})
export default class CompletedMissions extends Vue {
  private missions: Mission[] = [];
  private showedMissions: Mission[] = [];
  private mode: Filters = Filters.all;
  private isSearchMenuOpen = false;
  private currentTab = 0;
  private openLogId = ' ';
  private filter = Filters;
  private currentFilter = 'Aucun';
  private isAscending = true;
  private areLogsOpen = false;
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
      default:
        console.log('The mode variable is not set correctly');
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

      default:
        console.log('The filter is not set to the correct value');
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
    if (logs)
      this.getMissionInfo(item).then((mission: Mission) => {
        item.logs = mission.logs;
        this.openLogId = mission._id;
      });
    else this.openLogId = ' ';
  }
  private getMissionInfo(item: Mission): Promise<Mission> {
    return ServerCommunication.getSpecificMission(item._id)
      .then(res => res.json())
      .then((data: Mission) => {
        return data;
      });
  }
}
</script>
