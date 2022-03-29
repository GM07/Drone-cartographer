<template>
  <div id="content">
    <div id="files">
      <button id="te" class="primary" @click="save"></button>

      <treeselect
        id="tree"
        :alwaysOpen="true"
        :flatten-search-results="true"
        name="Files"
        noChildrenText="Empty"
        :options="options"
        :searchable="false"
      />
    </div>
    <div id="file" @keydown.ctrl.83.prevent.stop="save">
      <CodeEditor
        ref="codeEditor"
        border_radius="0px"
        :copy_code="true"
        :display_language="false"
        height="100%"
        :language_selector="true"
        :languages="[['cpp', 'C++']]"
        :value="fileContent"
        width="100%"
        z_index="3"
        @input="save"
      ></CodeEditor>
    </div>
  </div>
</template>

<style>
#content {
  height: 100vh;
  display: flex;
}

#te {
  width: 100px;
  height: 20px;
}

.vue-treeselect__menu {
  border: none;
  color: #d4d4d4;
  background-color: #252526;
}

.vue-treeselect__option--highlight {
  color: #d4d4d4;
  background-color: #add6ff14;
}

.vue-treeselect--single .vue-treeselect__option--selected,
.vue-treeselect--single .vue-treeselect__option--selected:hover {
  color: #d4d4d4;
  background-color: #add6ff26;
}

.vue-treeselect--open.vue-treeselect--open-below .vue-treeselect__control {
  border-bottom-left-radius: 0;
  border-bottom-right-radius: 0;
  border-top-left-radius: 0;
  border-top-right-radius: 0;
}

.vue-treeselect__control {
  display: none;
}

#files {
  width: 15%;
  border-color: #3a3d41;
  background-color: #252526;
}

#file {
  width: 100%;
  height: 100%;
}

.hljs {
  background-color: #1e1e1e;
}
</style>
<script lang="ts">
import {Component, Vue} from 'vue-property-decorator';
import FileEditor from '@/components/file_editor.vue';
import Treeselect from '@riophae/vue-treeselect';
import '@riophae/vue-treeselect/dist/vue-treeselect.css';
import {ServerCommunication} from '@/communication/server_communication';
import CodeEditor from '@/components/code_editor/code_editor.vue';

require('highlight.js');

@Component({
  components: {FileEditor, CodeEditor, Treeselect},
})
export default class Editor extends Vue {
  public attemptedLimitedConnection = false;
  public value = null;
  public fileContent = 'test';
  public options = [
    {
      id: 'a',
      label: 'src',
      children: [
        {
          id: 'aa',
          label: 'embedded.cc',
        },
        {
          id: 'ab',
          label: 'navigationSystem.cc',
        },
      ],
    },
    {
      id: 'app',
      label: 'appMain.cc',
    },
    {
      id: 'folder',
      label: 'include',
      children: [],
    },
  ];

  constructor() {
    super();
  }

  async created(): Promise<void> {
    this.fileContent = '// this is a comment';
    const RESPONSE = await ServerCommunication.getFiles();
    const NEW_FILE_CONTENT = await RESPONSE.json();
    this.changeFileContent(NEW_FILE_CONTENT['test.cpp']);
  }

  public save(): void {
    console.log('Saving');
  }

  // private beforeCreate() {
  //   ServerCommunication.getFiles().then((response: Response) => {
  //     response.json().then(res => {
  //       this.changeFileContent(res['test.cpp']);
  //     });
  //   });
  // }

  private changeFileContent(value: string): void {
    this.fileContent = value;
    // eslint-disable-next-line @typescript-eslint/no-explicit-any
    (this.$refs.codeEditor as any).$forceUpdate();
    // this.$forceUpdate();
    console.log('new content : ', this.fileContent);
  }
}
</script>
