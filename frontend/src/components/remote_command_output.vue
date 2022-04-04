<template>
  <div id="container" style="height: 100%; position: relative">
    <div id="console">
      <div
        ref="console"
        style="
          position: absolute;
          inset: 0px;
          padding-left: 20px;
          background-color: #1e1e1e;
          overflow: scroll;
          display: flex;
          flex-direction: column-reverse;
        "
      >
        <p
          v-for="item in this.output"
          :key="item[1]"
          :style="
            item[0] === 'stdout'
              ? 'color: white; margin: 0px; font-family: Consolas, Monaco, monospace;'
              : 'color: red; margin: 0px; font-family: Consolas, Monaco, monospace;'
          "
        >
          {{ item[1] }}
        </p>
      </div>
    </div>
  </div>
</template>

<style>
#content {
  display: flex;
}

#container {
  display: flex;
  flex-wrap: wrap;
  flex-direction: column;
}

#console {
  height: 100%;
  width: 100%;
  position: absolute;
  padding: 10px;
  overflow: hidden;
}

.scroll-area {
  width: 100%;
  height: 100vh;
}

::-webkit-scrollbar {
  width: 0; /* Remove scrollbar space */
  background: transparent; /* Optional: just make scrollbar invisible */
}
</style>

<script lang="ts">
import {Component, Prop, Vue} from 'vue-property-decorator';
import SocketIO, {Socket} from 'socket.io-client';
import {SERVER_ADDRESS} from '@/communication/server_constants';
import {DefaultEventsMap} from 'socket.io/dist/typed-events';
import vueCustomScrollbar from 'vue-custom-scrollbar';
import 'vue-custom-scrollbar/dist/vueScrollbar.css';

@Component({components: {vueCustomScrollbar}})
export default class RemoteCommandOutput extends Vue {
  @Prop() private namespace!: string;
  @Prop() private icon!: string;

  public settings = {
    suppressScrollY: false,
    suppressScrollX: false,
    wheelPropagation: false,
  };
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
