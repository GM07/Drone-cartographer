<template>
  <div>
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

@Component({components: {NavigationCommands}})
export default class CompletedMissions extends Vue {
  private beforeCreate() {
    ServerCommunication.getCompletedMissions()
      .then(res => res.json())
      .then(data => console.log(data));
  }
  private getCompletedMissions(): void {
    ServerCommunication.getCompletedMissions().then(res => console.log(res));
  }
}
</script>
