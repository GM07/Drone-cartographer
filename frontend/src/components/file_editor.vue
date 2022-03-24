<template>
  <div id="content" @keydown.ctrl.83.prevent.stop="save">
    <CodeEditor
      border_radius="0px"
      :copy_code="true"
      :display_language="false"
      :language_selector="true"
      :languages="[['cpp', 'C++']]"
      v-bind:value="content"
      width="100%"
      z_index="3"
      @changeValue="changeValue($event)"
      @input="save"
    ></CodeEditor>
  </div>
</template>

<style lang="scss">
#content {
  width: 100%;
  height: 100vm;
}

.hljs {
  background-color: #1e1e1e;
}
</style>

<script lang="ts">
import {Component, Vue} from 'vue-property-decorator';
import CodeEditor from '@/components/code_editor/code_editor.vue';

require('highlight.js');

@Component({
  components: {CodeEditor},
})
export default class FileEditor extends Vue {
  public attemptedLimitedConnection = false;
  public content;

  constructor() {
    super();
    this.content = '// This is your code in C++';
  }

  changeValue(value: string): void {
    this.content = value;
  }

  save(): void {
    console.log(this.content);
  }
}
</script>
