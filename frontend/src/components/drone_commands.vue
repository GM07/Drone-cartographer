<template>
  <v-list dense nav>
    <v-list-item
      v-if="!this.$store.state.missionStatus.isMissionSimulated"
      v-on:click="identify()"
    >
      <v-list-item-icon>
        <v-icon color="blue">mdi-lightbulb</v-icon>
      </v-list-item-icon>
      <v-list-item-title class="title">Identifier</v-list-item-title>
    </v-list-item>
  </v-list>
</template>

<script lang="ts">
import {Component, Prop, Vue} from 'vue-property-decorator';
import {ServerCommunication} from '@/communication/server_communication';

@Component({})
export default class DroneCommands extends Vue {
  @Prop() private droneid!: string;

  public identify(): void {
    ServerCommunication.identifyDrone(this.droneid).catch(error =>
      console.error(error)
    );
  }
}
</script>
