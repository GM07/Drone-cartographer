<template>
  <v-list nav>
    <v-list-item to="/">
      <v-list-item-icon>
        <v-icon color="blue">mdi-home</v-icon>
      </v-list-item-icon>
      <v-list-item-title class="title">Menu principal</v-list-item-title>
    </v-list-item>
  </v-list>
</template>

<script lang="ts">
import {Component, Vue} from 'vue-property-decorator';

@Component({})
export default class NavigationCommands extends Vue {}
</script>
