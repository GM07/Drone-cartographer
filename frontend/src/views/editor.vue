<template>
  <div id="content">
    <div id="files">
      <treeselect
        id="tree"
        :alwaysOpen="true"
        :flat="true"
        :flatten-search-results="true"
        name="Files"
        noChildrenText="Empty"
        :options="options"
        :searchable="false"
      />
    </div>
    <v-layout fill-height>
      <FileEditor></FileEditor>
    </v-layout>
  </div>
</template>

<style>
#content {
  height: 100vh;
  display: flex;
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
</style>
<script lang="ts">
import {Component, Vue} from 'vue-property-decorator';
import FileEditor from '@/components/file_editor.vue';
import Treeselect from '@riophae/vue-treeselect';
import '@riophae/vue-treeselect/dist/vue-treeselect.css';

@Component({
  components: {FileEditor, Treeselect},
})
export default class Editor extends Vue {
  public attemptedLimitedConnection = false;
  public value = null;
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
}
</script>
