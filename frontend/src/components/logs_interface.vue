<template>
  <v-virtual-scroll
    id="scroll"
    height="100"
    item-height="20"
    :items="this.logs"
    style="z-index: 10"
  >
    <template v-slot:default="{item}">
      <v-list-item :key="item[0]">
        <v-list-item-content>
          <v-list-item-title>
            <strong> {{ item }}</strong>
          </v-list-item-title>
        </v-list-item-content>
      </v-list-item>

      <v-divider></v-divider>
    </template>
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
      const SCROLLBAR = document.getElementById('scroll');
      let isBottom = false;
      this.logs.push(...logs);

      if (SCROLLBAR) {
        isBottom = SCROLLBAR.scrollTop + 100 === SCROLLBAR.scrollHeight;

        if (isBottom)
          this.$nextTick(() => {
            SCROLLBAR.scrollTop = SCROLLBAR.scrollHeight;
          });
      }
    });
  }
  private destroyed() {
    SOCKETIO_GET_LOGS.removeAllListeners().close();
  }
}
</script>
