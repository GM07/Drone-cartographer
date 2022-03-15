<template>
  <v-list nav>
    <v-list-item @click="returnToMainMenu()">
      <v-list-item-icon>
        <v-icon color="blue">mdi-home</v-icon>
      </v-list-item-icon>
      <v-list-item-title>Menu principal</v-list-item-title>
    </v-list-item>
    <div v-if="attemptedLimitedConnection">
      <v-list-item v-if="canRevokeControl()" @click="revokeMissionControl()">
        <v-list-item-icon>
          <v-icon color="blue">mdi-account-cancel</v-icon>
        </v-list-item-icon>
        <v-list-item-title>Abandonner le contrôle</v-list-item-title>
      </v-list-item>
      <v-list-item
        v-else
        :disabled="isTakeControlDisabled()"
        @click="takeMissionControl()"
      >
        <v-list-item-icon>
          <v-icon color="blue">mdi-account-check</v-icon>
        </v-list-item-icon>
        <v-list-item-title>Prendre contrôle</v-list-item-title>
      </v-list-item>
    </div>
  </v-list>
</template>

<script lang="ts">
import {AccessStatus} from '@/communication/access_status';
import {ServerCommunication} from '@/communication/server_communication';
import {ROUTER} from '@/router';
import {ACCESSOR} from '@/store';
import {Component, Prop, Vue} from 'vue-property-decorator';
import {Drone} from '@/communication/drone';

@Component({})
export default class NavigationCommands extends Vue {
  @Prop() private accessStatus!: AccessStatus;
  @Prop() private droneList!: Drone[];
  public attemptedLimitedConnection = false;

  constructor() {
    super();
    const TIMEOUT = 1000;
    ServerCommunication.takeMissionControlTimeout(
      TIMEOUT,
      this.droneList,
      () => {
        this.setLimitedConnection(true);
      }
    );
  }

  public setLimitedConnection(connection: boolean): void {
    this.attemptedLimitedConnection = connection;
  }

  public canRevokeControl(): boolean {
    return this.accessStatus.isUserControlling;
  }

  public isTakeControlDisabled(): boolean {
    return ACCESSOR.missionStatus.isSomeoneControlling;
  }

  public returnToMainMenu(): void {
    ROUTER.push('/');
  }

  public takeMissionControl(): void {
    if (!ACCESSOR.missionStatus.isSomeoneControlling)
      ServerCommunication.takeMissionControl(this.droneList);
  }

  public revokeMissionControl(): void {
    if (this.accessStatus.isUserControlling)
      ServerCommunication.revokeMissionControl();
  }
}
</script>
