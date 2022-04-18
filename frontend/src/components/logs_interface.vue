<template>
  <div style="height: 200px; position: relative">
    <div
      style="
        position: absolute;
        inset: 0px;
        padding-left: 20px;
        overflow-y: scroll;
        display: flex;
        flex-direction: column-reverse;
      "
    >
      <p
        v-for="item in this.logs"
        :key="item[0]"
        style="
          color: black;
          margin: 0px;
          font-family: Consolas, Monaco, monospace;
          font-size: 13px;
          z-index: 10;
        "
      >
        <b>{{ item[0] }}</b> {{ item[1] }}
      </p>
    </div>
  </div>
</template>

<script lang="ts">
import {Component, Vue} from 'vue-property-decorator';
import {SOCKETIO_GET_LOGS} from '@/communication/server_constants';
@Component({})
export default class LogsInterface extends Vue {
  private logs: Array<[string, string]> = [];

  private beforeCreate() {
    SOCKETIO_GET_LOGS.on('get_logs', (logs: Array<[string, string]>) => {
      logs = logs.reverse();
      this.logs.unshift(...logs);
      this.$forceUpdate();
    });

    SOCKETIO_GET_LOGS.open();
  }
  private destroyed() {
    SOCKETIO_GET_LOGS.removeAllListeners().close();
  }
}
</script>
