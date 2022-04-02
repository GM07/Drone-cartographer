<template>
  <div id="content">
    <vue-resizable
      id="resizableDiv"
      :active="['r']"
      :width="400"
      @resize:move="resizeMove"
    >
      <div id="files">
        <vue-custom-scrollbar class="scroll-area" :settings="settings">
          <treeselect
            id="tree"
            :always-open="true"
            :alwaysOpen="treeOpen"
            :default-expand-level="10"
            :disableBranchNodes="true"
            :flatten-search-results="true"
            :maxHeight="NaN"
            name="Files"
            noChildrenText="Empty"
            :options="options"
            :searchable="false"
            @select="onFileSelected"
          />
        </vue-custom-scrollbar>
      </div>
    </vue-resizable>
    <div id="file" ref="fileDiv" @keydown.ctrl.83.prevent.stop="save">
      <div id="editor" class="scroll-area">
        <CodeEditor
          ref="codeEditor"
          border_radius="0px"
          :copy_code="false"
          :display_language="false"
          height="100%"
          :language_selector="false"
          :languages="[['cpp', 'C++']]"
          :value="fileContent"
          width="100%"
          z_index="3"
          @input="save"
        ></CodeEditor>
      </div>

      <div id="terminal">
        <remote-command-output namespace="/recompileSimulation">
        </remote-command-output>
      </div>
    </div>
  </div>
</template>

<style>
#content {
  height: 100vh;
  background-color: #252526;
  display: flex;
  flex-direction: row;
}

.vue-treeselect__menu {
  border: none;
  color: #d4d4d4;
  background-color: #252526;
}

.vue-treeselect__option--highlight {
  color: #d4d4d4;
  background-color: #37373d50;
}

.vue-treeselect--single .vue-treeselect__option--selected,
.vue-treeselect--single .vue-treeselect__option--selected:hover {
  color: #d4d4d4;
  background-color: #37373dff;
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
  width: 100%;
}

.scroll-area {
  width: 100%;
  height: 100vh;
}

#editor {
  width: 100%;
  height: 80%;
}

#terminal {
  width: 100%;
  height: 20%;
  background-color: #1e1e1e;
  border-top: 1px solid #d7ebff7a;
}

#file {
  height: 100%;
  display: flex;
  flex-direction: column;
  width: calc(100% - 400px);
}
</style>
<script lang="ts">
import {Component, Vue} from 'vue-property-decorator';
import Treeselect from '@riophae/vue-treeselect';
import '@riophae/vue-treeselect/dist/vue-treeselect.css';
import {ServerCommunication} from '@/communication/server_communication';
import CodeEditor from '@/components/code_editor/code_editor.vue';
import RemoteCommandOutput from '@/components/remote_command_output.vue';
import vueCustomScrollbar from 'vue-custom-scrollbar';
import VueResizable from 'vue-resizable';
import 'vue-custom-scrollbar/dist/vueScrollbar.css';

require('highlight.js');

class TreeNode {
  public id: string;
  public label: string;
  public children: TreeNode[] | undefined;

  constructor(id: string, label: string, children?: TreeNode[]) {
    this.id = id;
    this.label = label;
    this.children = children;
  }

  public clone(): TreeNode {
    const CHILDREN = this.children?.map(node => node.clone());
    return new TreeNode(this.id, this.label, CHILDREN);
  }
}

@Component({
  components: {
    CodeEditor,
    Treeselect,
    RemoteCommandOutput,
    vueCustomScrollbar,
    VueResizable,
  },
})
export default class Editor extends Vue {
  public attemptedLimitedConnection = false;
  public value = null;
  public fileContent = 'test';
  public files: Map<string, string> = new Map();
  public options: TreeNode[] = [];
  public treeOpen = false;
  public fileWidth = 1000;
  public settings = {
    suppressScrollY: false,
    suppressScrollX: false,
    wheelPropagation: false,
  };

  constructor() {
    super();
  }

  async created(): Promise<void> {
    this.fileContent = '// this is a comment';
    const RESPONSE = await ServerCommunication.getFiles();
    this.computeHierarchy(RESPONSE);
    // this.changeFileContent();
  }

  public computeHierarchy(response: Response): void {
    const PATHS: string[] = response['keys'];
    const VALUES: string[] = response['values'];

    this.files.clear();
    for (let i = 0; i < PATHS.length; i++) {
      this.files.set(PATHS[i], VALUES[i]);
    }

    const FIRST_NODE = new TreeNode('/', '/', []);

    for (const PATH of PATHS) {
      const SUBPATHS: string[] = PATH.split('/');
      let lastNode = FIRST_NODE;
      for (const SUBPATH of SUBPATHS.slice(1)) {
        const ID = PATH.substring(0, PATH.indexOf(SUBPATH) + SUBPATH.length);

        if (SUBPATH.indexOf('.') !== -1) {
          // File
          lastNode.children?.push(new TreeNode(ID, SUBPATH));
        } else if (lastNode.children !== undefined) {
          // Folder
          const CURRENT_NODE = new TreeNode(ID, SUBPATH, []);
          const INDEX = lastNode.children?.findIndex(node => {
            return node.label === CURRENT_NODE.label;
          });
          if (INDEX !== -1) {
            // Current node in current directory
            lastNode = lastNode.children[INDEX];
          } else {
            // Current node not in current directory
            // Adding it to current directory
            lastNode.children.push(CURRENT_NODE);
            lastNode = CURRENT_NODE;
          }
        }
      }
    }

    this.options = [FIRST_NODE]; // Start at /INF3995 instead of /workspaces
    this.treeOpen = true;
  }

  public onFileSelected(node: TreeNode): void {
    if (this.files.has(node.id)) {
      this.changeFileContent(this.files.get(node.id)!); // We already verify that the id is in the map... Compiler is dumb
    }
  }

  public resizeMove(data): void {
    const ELEM = document.getElementById('file') as HTMLElement;
    const WIDTH = 'calc(100% - ' + data.width + 'px)'; // Pro CSS
    ELEM.style.width = WIDTH;
  }

  public save(): void {
    console.log('Saving');
  }

  public recompile(): void {
    ServerCommunication.recompile();
  }

  private changeFileContent(value: string): void {
    this.fileContent = value;
    // eslint-disable-next-line @typescript-eslint/no-explicit-any
    (this.$refs.codeEditor as any).$forceUpdate();
    // this.$forceUpdate();
  }
}
</script>
