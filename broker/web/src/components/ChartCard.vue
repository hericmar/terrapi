<template>
  <v-card
    class="ma-4"
    max-width="800px"
    :loading="isLoading"
  >
    <v-card-title>
      {{ clientName }}
    </v-card-title>

    <v-card-subtitle>
      {{ now.toLocaleDateString() }}
    </v-card-subtitle>

    <v-card-text>
      <canvas :id="`chart-${clientId}`"></canvas>
    </v-card-text>

    <v-card-actions>
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
</template>

<script lang="ts">
import {onMounted, ref} from "vue";
import api from "@/api";
import {Chart, ChartItem} from "chart.js";

export default {
  props: {
    clientId: String,
    clientName: String
  },
  setup(props: any) {
    const now = ref(new Date())
    const isLoading = ref(true)

    let chart: Chart | null = null
    let records = null

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
          datasets: []
        },
        options: {
          parsing: {
            xAxisKey: 'timestamp',
            yAxisKey: 'value'
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
                maxTicksLimit: 12,
                stepSize: 50,
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
          data: value
        })
      }

      // @ts-ignore
      chart.config.data.datasets = datasets
      chart.config.options.scales.x.min = start.getTime()
      chart.config.options.scales.x.max = end.getTime()

      chart?.update()
    }

    const fetchRecords = () => {
      const startOfDay = new Date(now.value.getFullYear(), now.value.getMonth(), now.value.getDate(), 0, 0, 0);

      // Set the time to one minute before midnight (23:59:59) for the current date
      const endOfDay = new Date(now.value.getFullYear(), now.value.getMonth(), now.value.getDate(), 23, 59, 59);

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
      now,
      isLoading,

      onPrevClick,
      onNextClick,
    }
  }
}
</script>
