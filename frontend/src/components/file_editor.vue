<template>
  <div id="content" @keydown.ctrl.83.prevent.stop="save">
    <CodeEditor
      v-model="fileContent"
      border_radius="0px"
      :copy_code="true"
      :display_language="false"
      :language_selector="true"
      :languages="[['cpp', 'C++']]"
      width="100%"
      z_index="3"
      @input="save"
    ></CodeEditor>
  </div>
</template>

<style lang="scss">
#content {
  width: 100%;
  height: 100vm;
  overflow-y: scroll;
}

.hljs {
  background-color: #1e1e1e;
}
</style>

<script lang="ts">
import {Component, Prop, Vue, Watch} from 'vue-property-decorator';
import CodeEditor from '@/components/code_editor/code_editor.vue';

require('highlight.js');

@Component({
  components: {CodeEditor},
})
export default class FileEditor extends Vue {
  @Prop() private fileContent;
  public attemptedLimitedConnection = false;

  constructor() {
    super();
  }

  @Watch('fileContent', {immediate: true})
  public update(): void {
    console.log('fileContent changed');
    console.log(this.fileContent);
  }

  public save(): void {
    console.log(this.fileContent);
  }
}
</script>
