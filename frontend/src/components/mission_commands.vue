<template>
  <v-list dense nav>
    <v-list-item
      :disabled="
        this.$store.state.missionStatus.isMissionStarted ||
        isLaunchMissionSelected
      "
      v-on:click="launchMission()"
    >
      <v-list-item-title class="title">Lancer la mission</v-list-item-title>
    </v-list-item>
    <v-list-item
      :disabled="
        !this.$store.state.missionStatus.isMissionStarted ||
        isTerminateMissionSelected
      "
      v-on:click="terminateMission()"
    >
      <v-list-item-title class="title">Terminer la mission</v-list-item-title>
    </v-list-item>
  </v-list>
</template>

<script lang="ts">
import {Component, Vue} from 'vue-property-decorator';
import {ACCESSOR} from '@/store';
import {ServerCommunication} from '@/communication/server_communication';
import {HTTP_OK} from '@/communication/server_constants';

@Component({})
export default class MissionCommands extends Vue {
  public isLaunchMissionSelected = false;
  public isTerminateMissionSelected = false;

  public launchMission(): void {
    if (ACCESSOR.missionStatus.isMissionStarted) return;
    this.isLaunchMissionSelected = true;

    ServerCommunication.launchMission(ACCESSOR.missionStatus.isMissionSimulated)
      .then(response => {
        if (response.status === HTTP_OK) {
          ACCESSOR.missionStatus.isMissionStarted = true;
        }
      })
      .catch(error => console.error(error))
      .finally(() => {
        this.isLaunchMissionSelected = false;
      });
  }

  public terminateMission(): void {
    if (!ACCESSOR.missionStatus.isMissionStarted) return;
    this.isTerminateMissionSelected = true;

    ServerCommunication.terminateMission()
      .then(response => {
        if (response.status === HTTP_OK) {
          ACCESSOR.missionStatus.isMissionStarted = false;
        }
      })
      .finally(() => {
        this.isTerminateMissionSelected = false;
      });
  }
}
</script>
