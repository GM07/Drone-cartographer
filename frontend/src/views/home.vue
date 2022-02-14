<template>
  <v-container fill-height fluid>
    <v-row align="center" justify="center">
      <div v-if="!this.$store.state.missionStatus.isMissionStarted">
        <v-col align-self="center" class="text-center">
          <v-btn large primary to="/Mission"> Configurer mission </v-btn>
        </v-col>
      </div>
      <div v-else>
        <v-col align-self="center" class="text-center">
          <v-btn large primary to="/Mission">Mission</v-btn>
        </v-col>
      </div>
    </v-row>
  </v-container>
</template>

<script lang="ts">
import {Component, Vue} from 'vue-property-decorator';

@Component({})
export default class Home extends Vue {}
</script>
