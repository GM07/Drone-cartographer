<template>
  <v-app>
    <v-main>
      <router-view />
    </v-main>
  </v-app>
</template>

<style lang="sass">
@import '../node_modules/typeface-roboto/index.css'
</style>
<script lang="ts">
import Vue from 'vue';

export default Vue.extend({
  name: 'App',
  data: () => ({
    //
  }),
});
</script>
