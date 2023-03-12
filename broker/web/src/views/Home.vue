<template>
  <v-container>
    <ChartCard
      v-for="client in clients"
      :key="client.client_id"
      :client-name="client.name"
      :client-id="client.client_id"
    />
  </v-container>
</template>

<script lang="ts" setup>
import {Chart, registerables} from 'chart.js';
import {nextTick, onMounted, ref} from 'vue';
import 'chartjs-adapter-moment';
import api from "@/api";
import ChartCard from "@/components/ChartCard.vue";

Chart.register(...registerables);

const clients = ref([])

onMounted(() => {
  api.listClientPreview()
    .then((result) => {
      clients.value = result
    })
})
</script>
