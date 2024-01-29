<template>
  <v-main
    class="bg-grey-lighten-4 d-flex flex-column justify-space-between"
    style="min-height: 100vh"
  >
    <v-container class="mb-auto px-lg-16">
      <router-view :key="route.fullPath" />
    </v-container>
    <v-footer
      class="bg-grey-lighten-4 text-caption mt-auto"
    >
      <v-col class="text-center text-grey">
        © <a class="text-decoration-none text-grey" href="https://phire.cz">Phire</a> 2023–{{ new Date().getFullYear() }}
      </v-col>
    </v-footer>
  </v-main>
</template>

<script lang="ts" setup>
  import {useRoute} from "vue-router";

  const route = useRoute()
</script>
