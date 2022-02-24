<template>
  <v-list dense nav>
    <div class="switch-center">
      <v-switch
        v-model="simulatedMission"
        dense
        :disabled="this.$store.state.missionStatus.isMissionStarted"
        label="Simulation"
      >
      </v-switch>
    </div>

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
      v-on:click="returnToBase()"
    >
      <v-list-item-title class="title">Retourner à la base</v-list-item-title>
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
import {Component, Prop, Vue} from 'vue-property-decorator';
import {ACCESSOR} from '@/store';
import {ServerCommunication} from '@/communication/server_communication';
import {AccessStatus} from '@/communication/access_status';

@Component({})
export default class MissionCommands extends Vue {
  @Prop() private accessStatus!: AccessStatus;
  public isLaunchMissionSelected = false;
  public isTerminateMissionSelected = false;
  public isReturnToBaseSelected = false;

  set simulatedMission(isSimulated: boolean) {
    if (!ACCESSOR.missionStatus.isMissionStarted) {
      this.accessStatus.isMissionSimulated = isSimulated;
      ServerCommunication.setMissionType(isSimulated);
    }
  }

  get simulatedMission(): boolean {
    return this.accessStatus.isMissionSimulated;
  }

  public launchMission(): void {
    if (ACCESSOR.missionStatus.isMissionStarted) return;
    this.isLaunchMissionSelected = true;

    const COMMAND_SENT = ServerCommunication.launchMission(
      this.accessStatus.isMissionSimulated,
      () => {
        this.isLaunchMissionSelected = false;
      }
    );
    if (!COMMAND_SENT) {
      this.isLaunchMissionSelected = false;
    }
  }

  public returnToBase(): void {
    if (!ACCESSOR.missionStatus.isMissionStarted) return;
    this.isReturnToBaseSelected = true;

    const COMMAND_SENT = ServerCommunication.returnToBase(() => {
      this.isReturnToBaseSelected = false;
    });

    if (!COMMAND_SENT) {
      this.isReturnToBaseSelected = false;
    }
  }

  public terminateMission(): void {
    if (!ACCESSOR.missionStatus.isMissionStarted) return;
    this.isTerminateMissionSelected = true;

    const COMMAND_SENT = ServerCommunication.terminateMission(() => {
      this.isTerminateMissionSelected = false;
    });

    if (!COMMAND_SENT) {
      this.isTerminateMissionSelected = false;
    }
  }
}
</script>

<style scoped>
.switch-center {
  display: flex;
  justify-content: center;
}
</style>
