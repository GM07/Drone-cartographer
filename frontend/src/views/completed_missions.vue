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

        <v-btn icon>
          <v-icon>mdi-magnify</v-icon>
        </v-btn>

        <template v-slot:extension>
          <v-tabs>
            <v-tab @click="changeMode('all')">Tous</v-tab>
            <v-tab @click="changeMode('simulated')">Simulation</v-tab>
            <v-tab @click="changeMode('physical')">Embarqué</v-tab>
          </v-tabs>
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
          <v-card class="pa-5" elevation="4" :loading="loading" outlined>
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
  public searchMode = false;

  public changeMode(mode: string): void {
    this.getCompletedMissions();
    this.mode = mode;
  }

  public getFilteredMissions(): Mission[] {
    console.log(this.missions);
    switch (this.mode) {
      case 'all':
        return this.missions;

      case 'simulated':
        return this.missions.filter((mission: Mission) => mission.is_simulated);

      case 'physical':
        return this.missions.filter(
          (mission: Mission) => !mission.is_simulated
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
}
</script>
