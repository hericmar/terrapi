<template>
  <!-- see https://stackoverflow.com/questions/55867477/vuetify-2-toolbar-ans-1-navigation-drawer-with-1-toolbar-above-the-navigation-dr -->
  <v-navigation-drawer
    permanent
    v-model="drawer"
  >
    <v-list-item
      :to="{name: 'Home'}"
      :active="false"
      style="height: 3rem"
    >
      <v-icon icon="mdi-circle-slice-4" /> Terrapi
    </v-list-item>

    <v-divider></v-divider>

    <v-list-item
      prepend-icon="mdi-view-dashboard"
      v-for="item in mainStore.clients"
      :key="item.client_id"
      :to="{name: 'Environment', params: {clientId: item.client_id}}"
      :title="item.name"
      link
    ></v-list-item>
    <!--
    <v-list-item prepend-icon="mdi-view-dashboard" title="Home" value="home"></v-list-item>
    <v-list-item prepend-icon="mdi-cog" title="Settings" value="settings"></v-list-item>
    -->
  </v-navigation-drawer>
</template>

<script lang="ts" setup>
  import {useMainStore} from "@/store";
  import {computed} from "vue";

  const mainStore = useMainStore();
  const drawer = computed(() => mainStore.drawer);
</script>
