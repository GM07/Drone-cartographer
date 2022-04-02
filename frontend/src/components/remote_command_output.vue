<template>
  <div id="content">
  <div style="height: 100%; position: relative">
    <div
      ref="console"
      style="
        position: absolute;
        inset: 0px;
        background-color: black;
        overflow: auto;
        display: flex;
        flex-direction: column-reverse;
      "
    >
      <p
        v-for="item in this.output"
        :key="item[1]"
        :style="
          item[0] === 'stdout'
            ? 'color: white; margin: 0px'
            : 'color: red; margin: 0px'
        "
      >
        {{ item[1] }}
      </p>
    </div>
  </div>
</template>

<style>
#content {
  display: flex;
}

#action {
  margin: 10px;
}
</style>

<script lang="ts">
import {Component, Prop, Vue} from 'vue-property-decorator';
import SocketIO, {Socket} from 'socket.io-client';
import {SERVER_ADDRESS} from '@/communication/server_constants';
import {DefaultEventsMap} from 'socket.io/dist/typed-events';
@Component({})
export default class RemoteCommandOutput extends Vue {
  @Prop() private namespace!: string;

  public isFinished = true;
  public hasErrors = false;

  private output: Array<[string, string]> = [];
  private socket: Socket<DefaultEventsMap, DefaultEventsMap>;

  constructor() {
    super();

    this.socket = SocketIO(SERVER_ADDRESS + this.namespace, {
      transports: ['websocket'],
    }).close();

    this.socket.on('stdout', (stdout: string) => {
      if (stdout === '\n' || this.output.length === 0) {
        this.output.unshift(['stdout', '']);
      } else {
        this.output[0][1] += stdout;
      }
    });

    this.socket.on('stderr', (stderr: string) => {
      this.hasErrors = true;
      if (stderr === '\n' || this.output.length === 0) {
        this.output.unshift(['stderr', '']);
      } else {
        this.output[0][1] += stderr;
      }
    });

    this.socket.on('stop', () => {
      this.isFinished = true;
    });

    this.socket.on('start', () => {
      this.isFinished = false;
      this.hasErrors = false;
    });

    this.socket.open();
  }

  public;

  private updated() {
    const CONSOLE_OUTPUT = (this.$refs.console as Vue).$el as HTMLElement;
    CONSOLE_OUTPUT.scrollTop =
      CONSOLE_OUTPUT.scrollHeight - CONSOLE_OUTPUT.clientHeight;
  }

  private destroyed() {
    this.socket.close();
  }
}
</script>
