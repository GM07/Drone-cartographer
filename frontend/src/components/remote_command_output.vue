<template>
  <v-virtual-scroll
    ref="console"
    bench="5"
    item-height="20"
    :items="this.output"
  >
    <template v-slot:default="{item}">
      <v-list-item v-if="item[0] === 'stdout'">
        <p style="color: white">{{ item[1] }}</p>
      </v-list-item>
      <v-list-item v-if="item[0] === 'stderr'">
        <p style="color: red">{{ item[1] }}</p>
      </v-list-item>
    </template>
  </v-virtual-scroll>
</template>

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
      this.output.push(['stdout', stdout]);
    });

    this.socket.on('stderr', (stderr: string) => {
      this.hasErrors = true;
      this.output.push(['stderr', stderr]);
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
