<template>
  <v-card
    class="pa-2 mb-6"
    width="100%"
    :loading="isLoading"
  >
    <v-toolbar
      color="rgba(0, 0, 0, 0)"
    >
      <v-toolbar-title
        class="text-h6"
      >
        {{ clientName }}
      </v-toolbar-title>

      <template v-slot:append>

      </template>
    </v-toolbar>

    <v-card-subtitle>
      {{ now.toLocaleDateString() }}
    </v-card-subtitle>

    <v-card-text>
      <canvas :id="`chart-${clientId}`"></canvas>
    </v-card-text>

    <v-card-actions>
      <v-btn
        v-if="isLoggedIn"
        size="small"
        @click="showConfig = true"
      >Settings</v-btn>

      <v-spacer></v-spacer>

      <v-btn
        size="small"
        color="surface-variant"
        @click="onPrevClick"
      >
        <v-icon>mdi-chevron-left</v-icon> previous
      </v-btn>
      <v-btn
        size="small"
        color="surface-variant"
        @click="onNextClick"
      >
        next <v-icon>mdi-chevron-right</v-icon>
      </v-btn>
    </v-card-actions>
  </v-card>

  <v-row justify="center">
    <v-dialog
      v-model="showConfig"
      width="auto"
      persistent
    >
      <ConfigCard
        :client-id="clientId"
        :on-close="onConfigClose"
      />
    </v-dialog>
  </v-row>
</template>

<script lang="ts">
import {computed, onMounted, ref} from "vue";
import api from "@/api";
import {Chart, ChartItem} from "chart.js";
import ConfigCard from "@/components/ConfigCard.vue";
import {useMainStore} from "@/store";

export default {
  components: {ConfigCard},
  props: {
    clientId: String,
    clientName: String
  },
  setup(props: any) {
    const now = ref(new Date())
    const isLoading = ref(true)
    const showConfig = ref(false)

    let chart: Chart | null = null
    let records = null

    const mainStore = useMainStore()

    const isLoggedIn = computed(() => mainStore.isLoggedIn)

    const onConfigClose = () => {
      showConfig.value = false
    }

    const onPrevClick = () => {
      now.value.setDate(now.value.getDate() - 1);
      now.value = new Date(now.value)

      fetchRecords()
    }

    const onNextClick = () => {
      now.value.setDate(now.value.getDate() + 1);
      now.value = new Date(now.value)

      fetchRecords()
    }

    const createChart = () => {
      const ctx = document.getElementById(`chart-${props.clientId}`);

      chart = new Chart(ctx as ChartItem, {
        type: 'line',
        data: {
          // datasets are set int updateChart function
          datasets: []
        },
        options: {
          parsing: {
            xAxisKey: 'timestamp',
            yAxisKey: 'value'
          },
          interaction: {
            mode: 'nearest',
            axis: 'x',
            intersect: false
          },
          plugins: {
            decimation: {
              algorithm: 'lttb',
              enabled: true,
              samples: 2400
            },
          },
          scales: {
            x: {
              type: 'time',
              time: {
                parser: 'X',
                round: 'minute',
                unit: 'second',
                tooltipFormat: 'HH:mm:ss',
              },
              ticks: {
                maxTicksLimit: 13,
                stepSize: 2400,
                callback: (value, index, ticks) => {
                  // https://www.youtube.com/watch?v=KBPOgUu882o
                  const date = new Date(value);
                  return date.toLocaleTimeString()
                }
              }
            }
          }
        }
      });
    }

    const updateChart = (records: any, start: Date, end: Date) => {
      let datasets = []

      for (const [key, value] of Object.entries(records.measurements)) {
        // we have to multiply unix timestamp by 1000, JS uses milliseconds.
        (value as Array<any>).forEach((item: any) => {
          item.timestamp *= 1000;
        });

        datasets.push({
          label: key,
          data: value,
          radius: 0
        })
      }

      // update chart

      // @ts-ignore
      chart.config.data.datasets = datasets
      // @ts-ignore
      chart.config.options.scales.x.min = start.getTime()
      // @ts-ignore
      chart.config.options.scales.x.max = end.getTime()

      chart?.update()
    }

    const fetchRecords = () => {
      const startOfDay = new Date(now.value.getFullYear(), now.value.getMonth(), now.value.getDate(), 0, 0, 0);

      // Set the time to one minute before midnight (23:59:59) for the current date
      const endOfDay = new Date(now.value.getFullYear(), now.value.getMonth(), now.value.getDate(), 24, 0, 0);

      api.listRecordsForClient(props.clientId, startOfDay, endOfDay)
        .then((response) => {
          isLoading.value = false;
          updateChart(response, startOfDay, endOfDay)
        })
    }

    onMounted(() => {
      createChart()
      fetchRecords()
    })

    return {
      isLoggedIn,

      now,
      isLoading,

      showConfig,
      onConfigClose,

      onPrevClick,
      onNextClick,
    }
  }
}
</script>
