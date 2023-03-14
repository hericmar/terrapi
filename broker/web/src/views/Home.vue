<template>
  <v-container class="d-flex justify-center pa-2 mt-2">
    <div class="d-inline-flex flex-wrap chart-cards">
      <div
        class="chart-card"
        v-for="client in clients"
        :key="client.client_id"
      >
        <ChartCard
          :client-name="client.name"
          :client-id="client.client_id"
        />
      </div>

      <div
        v-if="isLoggedIn"
        class="bg-grey-lighten-4 add-new-card chart-card"
      >
        <v-btn
          variant="outlined"
          size="x-large"
          color="grey-darken-1"
          icon
          @click="createClientDialog = true"
        >
          <v-icon>mdi-plus</v-icon>
        </v-btn>
      </div>
    </div>
  </v-container>

  <v-row justify="center">
    <v-dialog
      v-model="createClientDialog"
      width="auto"
      persistent
    >
      <ClientCard
        :on-close="() => { createClientDialog = false }"
      />
    </v-dialog>
  </v-row>
</template>

<script lang="ts" setup>
import {Chart, registerables} from 'chart.js';
import {computed, onMounted, ref} from 'vue';
import 'chartjs-adapter-moment';
import api from "@/api";
import ChartCard from "@/components/ChartCard.vue";
import ClientCard from "@/components/ClientCard.vue";
import {useMainStore} from "@/store";

Chart.register(...registerables);

const createClientDialog = ref(false)

const mainStore = useMainStore()
const isLoggedIn = computed(() => mainStore.isLoggedIn)
const clients = computed(() => mainStore.clients)

onMounted(() => {
  mainStore.fetchClients()
})
</script>

<style>

.chart-card {
  margin-bottom: 0.5rem;
}

.add-new-card {
  display: flex;
  justify-content: center;
  align-items: center;
  height: 300px;
}

@media (max-width: 600px) {
  .chart-cards {
    width: 100%;
  }

  .chart-card {
    width: 100%;
  }
}

@media (min-width: 600px) and (max-width: 1300px) {
  .chart-cards {
    width: 600px;
  }

  .chart-card {
    width: 550px;
    margin: 0 0.5rem;
  }
}

@media (min-width: 1300px) {
  .chart-cards {
    width: 1300px;
  }

  .chart-card {
    width: 550px;
    margin: 0 0.5rem;
  }
}
</style>
