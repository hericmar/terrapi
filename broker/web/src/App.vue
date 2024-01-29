<template>
  <router-view />
</template>

<script lang="ts" setup>
import {useMainStore} from "@/store";
import {Chart, registerables} from 'chart.js';
import 'chartjs-adapter-moment';

// This is required for the chart.js to work, otherwise it will throw an error:
// time is not a registered scale
Chart.register(...registerables);

const mainStore = useMainStore();
mainStore.init().then(() => {
  console.log('Main store initialized')
})
</script>
