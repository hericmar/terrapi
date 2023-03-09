<template>
  <v-app>
    <SideBar />

    <default-bar />

    <default-view />
  </v-app>
</template>

<script lang="ts" setup>
  import DefaultBar from './AppBar.vue'
  import DefaultView from './View.vue'
  import SideBar from "@/layouts/default/SideBar.vue";
</script>
