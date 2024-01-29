<template>
  <v-app>
    <SideBar />

    <default-bar />

    <v-snackbar v-model="snackbar">
      {{ mainStore.snackbarMessage }}
      <template v-slot:actions>
        <v-btn
          color="pink"
          variant="text"
          @click="snackbar = false"
        >
          Close
        </v-btn>
      </template>
    </v-snackbar>

    <default-view />

  </v-app>
</template>

<script lang="ts" setup>
import DefaultBar from './AppBar.vue'
import DefaultView from './View.vue'
import SideBar from "@/layouts/default/SideBar.vue";
import {useMainStore} from "@/store";
import {computed} from "vue";

const mainStore = useMainStore();

const snackbar = computed({
  get: () => !!mainStore.snackbarMessage,
  set: (value) => {
    if (!value) {
      mainStore.snackbarMessage = ''
    }
  }
})
</script>
