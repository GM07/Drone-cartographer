<template>
  <div id="container" style="height: 100%; position: relative">
    <div id="actions">
      <v-btn
        class="ma-2 white--text action-button"
        color="orange"
        :loading="loading3"
        x-small
        @click="recompile"
      >
        Recompile
        <v-icon dark right> mdi-cog-play </v-icon>
      </v-btn>
      <v-btn
        class="ma-2 white--text action-button"
        color="green"
        :loading="loading3"
        x-small
        @click="recompile"
      >
        Flash
        <v-icon dark right> mdi-play </v-icon>
      </v-btn>
    </div>
    <div id="console">
      <vue-custom-scrollbar class="scroll-area" :settings="settings">
        <div
          ref="console"
          style="
            position: absolute;
            inset: 0px;
            background-color: #1e1e1e;
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
      </vue-custom-scrollbar>
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
  height: 90%;
  width: 100%;
  position: absolute;
  top: 10%;
}

#actions {
  height: 10%;
  display: flex;
  flex-direction: row;
}

.action-button {
  margin: 1%;
}
</style>

<script lang="ts">
import {Component, Prop, Vue} from 'vue-property-decorator';
import SocketIO, {Socket} from 'socket.io-client';
import {SERVER_ADDRESS} from '@/communication/server_constants';
import {DefaultEventsMap} from 'socket.io/dist/typed-events';
import {ServerCommunication} from '@/communication/server_communication';
@Component({})
export default class RemoteCommandOutput extends Vue {
  @Prop() private namespace!: string;

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

  public recompile(): void {
    ServerCommunication.recompile();
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
