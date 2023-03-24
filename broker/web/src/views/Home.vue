<template>
  <v-container class="d-flex justify-center">
    <div class="d-flex flex-wrap chart-cards-container">
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

  <div class="d-flex" justify="center">
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
import {Chart, registerables} from 'chart.js';
import {computed, onMounted, ref} from 'vue';
import 'chartjs-adapter-moment';
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
}

.chart-cards-container {
  gap: 1rem;
}

.add-new-card {
  display: flex;
  justify-content: center;
  align-items: center;
  height: 300px;
}

@media (max-width: 600px) {
  .chart-cards-container {
    width: 100%;
  }

  .chart-card {
    width: 100%;
  }
}

@media (min-width: 600px) and (max-width: 1300px) {
  .chart-cards-container {
    width: 600px;
  }

  .chart-card {
    width: 550px;
  }
}

@media (min-width: 1300px) {
  .chart-cards {
    width: 1300px;
  }

  .chart-card {
    width: 550px;
  }
}
</style>
