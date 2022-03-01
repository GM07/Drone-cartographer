<template>
  <v-container fill-height fluid>
    <v-row align="center" class="full-height" fluid justify="center">
      <v-col align-self="center" class="text-center">
        <div v-if="!this.$store.state.missionStatus.isSomeoneControlling">
          <v-btn class="ma-4" large primary @click="configureMission()">
            Configurer mission
          </v-btn>
        </div>
        <div v-else>
          <v-btn class="ma-4" large primary to="/Mission">Mission</v-btn>
        </div>

        <v-btn class="ma-4" large primary to="/Historique"
          >Historique de Mission
        </v-btn>
      </v-col>
    </v-row>
  </v-container>
</template>

<script lang="ts">
import {Component, Vue} from 'vue-property-decorator';
import {ROUTER} from '@/router';

@Component({})
export default class Home extends Vue {
  public configureMission(): void {
    ROUTER.push('/Mission');
  }
}
</script>
