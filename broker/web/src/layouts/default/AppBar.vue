<template>
  <v-app-bar
    class="border-b"
    density="compact"
    elevation="0"
  >
    <v-app-bar-nav-icon variant="text" @click="drawer = !drawer"></v-app-bar-nav-icon>

    <span v-if="display.name.value === 'xs'">
      <v-icon
        icon="mdi-circle-slice-4"
        class="mr-1"
      /> Terrapi
    </span>

    <v-spacer></v-spacer>

    <v-btn
      class="mr-1"
      icon
    >
      <v-icon>mdi-sync</v-icon>
      <v-tooltip
        activator="parent"
        location="bottom"
      >
        Refresh
      </v-tooltip>
    </v-btn>

    <v-select
      class="mr-1 text-caption"
      density="compact"
      variant="outlined"
      hide-details
      v-model="refreshRate"
      style="max-width: 150px;"
      :items="[
        {
          label: 'Refresh Off',
          value: 0
        },
        {
          label: '10 s',
          value: 10
        },
        {
          label: '30 s',
          value: 30
        },
        {
          label: '1 min',
          value: 60
        },
        {
          label: '5 min',
          value: 300
        },
        {
          label: '10 min',
          value: 600
        }]"
      item-title="label"
      item-value="value"
    >
    </v-select>

    <v-btn
      v-if="isLoggedIn"
      class="text-none"
    >
      <v-icon class="mr-2">mdi-account</v-icon>
      Admin

      <v-menu activator="parent">
        <v-list density="compact">
          <v-list-item @click="createClientDialog = true">
            <v-icon class="mr-1">mdi-plus</v-icon> Create Client
          </v-list-item>
          <v-divider></v-divider>
          <v-list-item @click="mainStore.logout">Logout</v-list-item>
        </v-list>
      </v-menu>
    </v-btn>
    <v-btn
      v-else
      @click="onOpenDialog"
    >
      Login
    </v-btn>
  </v-app-bar>

  <!-- Login Dialog -->
  <v-dialog
    v-model="dialog"
    persistent
    min-width="400"
    max-width="600"
  >
    <LoginCard
      :on-close="() => { dialog = false }"
    />
  </v-dialog>

  <!-- Create Client Dialog -->
  <div class="d-flex">
    <v-dialog
      v-model="createClientDialog"
      width="auto"
      persistent
    >
      <ClientCard
        :on-close="() => { createClientDialog = false }"
      />
    </v-dialog>
  </div>
</template>

<script lang="ts" setup>
import {computed, ref, watch} from "vue";
import { useMainStore } from '@/store/index'
import LoginCard from "@/components/LoginCard.vue";
import ClientCard from "@/components/ClientCard.vue";
import {useDisplay} from "vuetify";

const dialog = ref(false)

const mainStore = useMainStore()
const drawer = computed({
  get: () => mainStore.drawer,
  set: (value) => mainStore.drawer = value
})
const isLoggedIn = computed(() => mainStore.isLoggedIn)

const display = useDisplay()
watch(display.name, (value) => {
  if (value === 'xs') {
    mainStore.drawer = false
  } else {
    mainStore.drawer = true
  }
})

const createClientDialog = ref(false)

const onOpenDialog = () => {
  dialog.value = true
}

// refresh rate
const refreshRate = computed({
  get: () => mainStore.settings.refreshRate,
  set: (value) => {
    mainStore.updateSettings({
      refreshRate: value
    })
  }
})
</script>
