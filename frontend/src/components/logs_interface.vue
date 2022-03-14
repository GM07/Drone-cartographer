<template>
  <v-virtual-scroll height="100" item-height="20">
    <div v-for="log in logs" :key="log[0]">Test</div>
  </v-virtual-scroll>
</template>

<script lang="ts">
import {Component, Vue} from 'vue-property-decorator';
import {SOCKETIO_GET_LOGS} from '@/communication/server_constants';

@Component({})
export default class LogsInterface extends Vue {
  private logs: Array<[string, string]> = [];

  private beforeCreate() {
    SOCKETIO_GET_LOGS.open();

    SOCKETIO_GET_LOGS.on('get_logs', (logs: Array<[string, string]>) => {
      this.logs.push(...logs);
      console.log(this.logs);
    });
  }
  private destroyed() {
    SOCKETIO_GET_LOGS.removeAllListeners().close();
  }
}
</script>
