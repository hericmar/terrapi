<template>
  <div
    class="mt-8 w-100 text-center text-grey-darken-2"
  >
    <div>
      Welcome to TerraPi!
    </div>
    <div>
      Select a client from the left menu to view its data.
    </div>
    <div
      v-if="clients.length === 0"
    >
      No clients configured, {{ !isLoggedIn ? "login and " : "" }}create a new one.
    </div>
  </div>
</template>

<script lang="ts" setup>
import {computed, onMounted} from 'vue';
import {useMainStore} from "@/store";

const mainStore = useMainStore()
const isLoggedIn = computed(() => mainStore.isLoggedIn)
const clients = computed(() => mainStore.clients)

onMounted(() => {
  mainStore.fetchClients()
})
</script>
