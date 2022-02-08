<template>
  <v-container fill-height fluid>
    <v-row align="center" justify="center">
      <div v-if="!this.$store.state.missionStatus.isMissionStarted">
        <v-col align-self="center" class="text-center">
          <div class="switch-center">
            <v-switch v-model="simulatedMission" label="Simulation"> </v-switch>
          </div>
          <v-btn large primary to="/Mission"> Démarrer mission </v-btn>
        </v-col>
      </div>
      <div v-else>
        <v-col align-self="center" class="text-center">
          <div class="switch-center">
            <v-switch v-model="simulatedMission" disabled label="Simulation">
            </v-switch>
          </div>
          <v-btn large primary to="/Mission">Mission</v-btn>
        </v-col>
      </div>
    </v-row>
  </v-container>
</template>

<script lang="ts">
import {Component, Vue} from 'vue-property-decorator';
import {ACCESSOR} from '@/store';

@Component({})
export default class Home extends Vue {
  set simulatedMission(isSimulated: boolean) {
    if (!ACCESSOR.missionStatus.isMissionStarted)
      ACCESSOR.missionStatus.isMissionSimulated = isSimulated;
  }

  get simulatedMission(): boolean {
    return ACCESSOR.missionStatus.isMissionSimulated;
  }
}
</script>

<style scoped>
.switch-center {
  display: flex;
  justify-content: center;
}
</style>
