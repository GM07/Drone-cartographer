<template>
  <div>
    <v-virtual-scroll
      id="scroll"
      :elevation="20"
      height="200"
      item-height="20"
      :items="this.logs"
    >
      <template v-slot:default="{item}">
        <v-list-item :key="item[0]">
          <v-list-item-content>
            <p>{{ item[0] }} {{ item[1] }}</p>
          </v-list-item-content>
        </v-list-item>
      </template>
    </v-virtual-scroll>
  </div>
</template>

<script lang="ts">
import {Component, Vue} from 'vue-property-decorator';
import {SOCKETIO_GET_LOGS} from '@/communication/server_constants';
const SCROLL_DIFFERENCE = 240;
@Component({})
export default class LogsInterface extends Vue {
  private logs: Array<[string, string]> = [];

  private beforeCreate() {
    SOCKETIO_GET_LOGS.open();

    SOCKETIO_GET_LOGS.on('get_logs', (logs: Array<[string, string]>) => {
      let isBottom = false;

      this.logs.push(...logs);
      const SCROLLBAR = document.getElementById('scroll');
      if (SCROLLBAR) {
        isBottom =
          SCROLLBAR.scrollTop + SCROLL_DIFFERENCE >= SCROLLBAR.scrollHeight;

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
