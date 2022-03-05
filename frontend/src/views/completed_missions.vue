<template>
  <div>
    <v-card class="overflow-hidden">
      <v-app-bar
        id="scrolling-techniques-4"
        dense
        fade-img-on-scroll
        hide-on-scroll
      >
        <v-toolbar-title>Historique</v-toolbar-title>

        <v-spacer></v-spacer>
        <v-spacer></v-spacer>
        <v-spacer></v-spacer>
        <v-spacer></v-spacer>
        <v-spacer></v-spacer>
        <v-spacer></v-spacer>
        <v-spacer></v-spacer>
        <v-spacer></v-spacer>
        <v-spacer></v-spacer>

        <template>
          <v-menu
            ref="menu"
            v-model="menu"
            :close-on-content-click="false"
            min-width="auto"
            offset-y
            :return-value.sync="date"
            style="right: 0px; text-align: right; min-width: 200px"
            transition="scale-transition"
          >
            <template v-slot:activator="{on, attrs}">
              <v-text-field
                v-model="date"
                v-bind="attrs"
                label="Picker in menu"
                prepend-icon="mdi-calendar"
                style="right: 0px; text-align: right"
                v-on="on"
              ></v-text-field>
            </template>

            <v-date-picker v-model="date" no-title scrollable>
              <v-spacer></v-spacer>
              <v-btn color="primary" text @click="menu = false">
                Annuler
              </v-btn>
              <v-btn color="primary" text @click="filterByDate(date)">
                Filtrer par date
              </v-btn>
            </v-date-picker>
          </v-menu>
        </template>
        <template v-slot:extension>
          <v-tabs v-model="currentTab">
            <v-tab @click="changeMode('all')">Tous</v-tab>
            <v-tab @click="changeMode('simulated')">Simulation</v-tab>
            <v-tab @click="changeMode('physical')">Embarqué</v-tab>
            <v-tab @click="changeMode('date')">Date</v-tab>
          </v-tabs>

          <div>
            <v-select
              v-model="baseFilter"
              :items="filters"
              style="right: 0px; text-align: right; width: 200px"
            ></v-select>
          </div>
        </template>
      </v-app-bar>

      <v-sheet
        id="scrolling-techniques-4"
        class="overflow-y-auto pa-4"
        min-height="96px"
      >
        <div
          v-for="item in this.getFilteredMissions()"
          :key="item._id"
          class="ma-5"
        >
          <v-card class="pa-5" elevation="4" outlined>
            <v-list-item-title v-if="item.is_simulated" class="text-h6 mb-3">
              Mission Simulé
            </v-list-item-title>
            <v-list-item-title v-if="!item.is_simulated" class="text-h6 mb-3">
              Mission Physique
            </v-list-item-title>

            <v-list-item-title class="text-h6 mb-3">
              Date de complétion: {{ item.time_stamp }}
            </v-list-item-title>

            <v-list-item-title class="text-h7 mb-3">
              <strong> Identifiant:</strong> {{ item._id }}
            </v-list-item-title>

            <v-btn class="ma-2" color="indigo" outlined>
              Plus d'informations
            </v-btn>
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

<script lang="ts">
import {Component, Vue} from 'vue-property-decorator';
import {ServerCommunication} from '@/communication/server_communication';
import NavigationCommands from '@/components/navigation_commands.vue';
import {Mission} from '@/utils/mission';

@Component({components: {NavigationCommands}})
export default class CompletedMissions extends Vue {
  public missions: Mission[] = [];
  public mode = 'all';
  public menu = false;
  public currentTab = 0;
  public baseFilter = 'Aucun';
  public filters = [
    'Aucun',
    'Nombre de drones',
    'Distance parcourue',
    'Temps de vol',
  ];
  public date: string = new Date(
    Date.now() - new Date().getTimezoneOffset() * 60000
  )
    .toISOString()
    .substr(0, 10);

  public changeMode(mode: string): void {
    this.getCompletedMissions();
    this.mode = mode;
  }

  public getFilteredMissions(): Mission[] {
    switch (this.mode) {
      case 'all':
        return this.sortMissions(this.missions);

      case 'simulated':
        return this.sortMissions(
          this.missions.filter((mission: Mission) => mission.is_simulated)
        );

      case 'physical':
        return this.sortMissions(
          this.missions.filter((mission: Mission) => !mission.is_simulated)
        );

      case 'date':
        return this.sortMissions(
          this.missions.filter(
            (mission: Mission) =>
              mission.time_stamp.substring(0, 10) === this.date
          )
        );

      default:
        return this.missions;
    }
  }
  private beforeCreate() {
    ServerCommunication.getCompletedMissions()
      .then(res => res.json())
      .then(data => {
        this.missions = data;
      });
  }

  private async getCompletedMissions(): Promise<void> {
    return ServerCommunication.getCompletedMissions()
      .then(res => res.json())
      .then(data => {
        this.missions = data;
      });
  }
  private filterByDate(date: string): void {
    this.currentTab = 3;
    this.date = date;
    this.changeMode('date');
  }

  private sortMissions(missions: Mission[]): Mission[] {
    console.log(this.baseFilter);
    switch (this.baseFilter) {
      case this.filters[0]:
        return missions;
      case this.filters[1]:
        return missions.sort((a, b) => {
          return a.number_of_drones - b.number_of_drones;
        });
      case this.filters[2]:
        return missions.sort((a, b) => {
          return a.total_distance - b.total_distance;
        });
      case this.filters[3]:
        return missions.sort((a, b) => {
          return a.flight_duration - b.flight_duration;
        });
      default:
        return missions;
    }
  }
}
</script>
