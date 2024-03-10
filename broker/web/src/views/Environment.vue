<template>
  <div class="d-flex align-baseline justify-space-between">
    <div class="d-flex">
      <h1>{{ clientName }}</h1>
      <v-btn
        v-if="isLoggedIn"
        icon
        color="grey"
        variant="text"
        size="small"
        @click="editClient = true"
      >
        <v-icon>mdi-pencil</v-icon>
      </v-btn>
    </div>

    <span>
      <v-btn
        icon
        variant="text"
        size="small"
        @click="onPrevClick"
      >
        <v-icon>mdi-chevron-left</v-icon>
      </v-btn>
      {{ now.toLocaleDateString() }}
      <v-btn
        icon
        variant="text"
        size="small"
        @click="onNextClick"
      >
        <v-icon>mdi-chevron-right</v-icon>
      </v-btn>
    </span>
  </div>

  <!-- TODO: Create separate chart card -->
  <v-card
    class="pa-4 my-4"
    v-for="(type, i) in chartTypes"
    :key="i"
  >
    <v-toolbar color="rgba(0, 0, 0, 0)">
      <v-toolbar-title class="text-h6">
        {{ Quantity[type] }}
      </v-toolbar-title>
    </v-toolbar>
    <canvas style="max-height: 400px" :id="`chart-${clientId}-${type}`"></canvas>
  </v-card>

  <div
    v-if="chartTypes.length === 0"
    class="d-flex mt-8 w-100 text-center bg-grey-lighten-4 text-grey-darken-2 justify-center align-center chart-card"
  >
    No records found.
  </div>

  <!-- Event log -->
  <v-card class="pa-2 my-4" v-if="events.length">
    <v-toolbar color="rgba(0, 0, 0, 0)">
      <v-toolbar-title class="text-h6">
        Event Log
      </v-toolbar-title>
    </v-toolbar>
    <v-list>
      <v-list-item v-for="(event, i) in events" :key="i">
        {{ event }}
      </v-list-item>
    </v-list>
  </v-card>

  <!-- Edit Client Dialog -->
  <v-dialog
    v-model="editClient"
    width="auto"
    persistent
  >
    <ClientCard
      :on-close="() => { editClient = false }"
      :client="client"
    />
  </v-dialog>
</template>

<script lang="ts" setup>
import {computed, nextTick, onMounted, onUnmounted, ref, shallowRef, watch} from "vue";
import {Quantity, Records} from "@/models";
import {DataSets, DEVICE_STATE_TO_NAME, fetchRecords, processRecords} from "@/records/data";
import {createChart, updateChart} from "@/records/chart";
import {useRoute, useRouter} from "vue-router";
import {useMainStore} from "@/store";
import ClientCard from "@/components/ClientCard.vue";
import {getTitle} from "@/router";
import {Chart} from "chart.js";

const route = useRoute()
const router = useRouter()
const clientId: string = route.params.clientId as string

const mainStore = useMainStore();
const clientName = computed(() => mainStore.clients.find((c) => c.client_id === clientId)?.name)

const events = ref<string[]>([])

// now is by default today
const now = computed({
  get: () => mainStore.date,
  set: (value) => mainStore.date = value
})

const routeDate = new Date(Date.parse(route.params.date as string))
if (!Number.isNaN(routeDate.valueOf())) {
  now.value = routeDate
}

const chartTypes = ref<Array<Quantity>>([])
const charts: Map<Quantity, Chart> = new Map()

const onPrevClick = () => {
  const date = new Date(now.value.getTime() - 24 * 60 * 60 * 1000)
  router.push(
    {
      name: 'Environment',
      params: {
        clientId: clientId,
        date: date.toISOString().substring(0, 10)
      }
    }
  )
}

const onNextClick = () => {
  const date = new Date(now.value.getTime() + 24 * 60 * 60 * 1000)
  router.push(
    {
      name: 'Environment',
      params: {
        clientId: clientId,
        date: date.toISOString().substring(0, 10)
      }
    }
  )
}

const getData = () => {
  fetchRecords(clientId, now.value).then((records: Records) => {
    events.value = records.events.map((e) => {
      const datetime = new Date(e.timestamp *= 1000).toLocaleTimeString()
      return `${datetime}: ${e.source} ${DEVICE_STATE_TO_NAME.get(e.state)}`
    })

    const groupedRecords: DataSets = processRecords(records)

    // This will trigger a re-render and create charts.
    const quantities = Array.from(groupedRecords.measurements.keys())
    const newCharts = quantities.filter((quantity) => !chartTypes.value.includes(quantity))
    chartTypes.value.push(...newCharts)

    nextTick(() => {
      // Required, because the chart canvas is not yet rendered.
      newCharts.forEach((quantity) => {
        const chart = createChart(clientId, quantity)
        charts.set(quantity, chart)
      })

      chartTypes.value.forEach((quantity) => {
        const records = groupedRecords.measurements.get(quantity)
        if (!records) {
          return
        }

        const chart = charts.get(quantity)!

        const startOfDay = new Date(now.value.getFullYear(), now.value.getMonth(), now.value.getDate(), 0, 0, 0);
        const endOfDay = new Date(now.value.getFullYear(), now.value.getMonth(), now.value.getDate(), 24, 0, 0);
        updateChart(chart, records, startOfDay, endOfDay)
      })
    })
  })
}

// autorefresh

let timer: any = undefined;
if (mainStore.settings.refreshRate > 0) {
  timer = setInterval(() => {
    getData()
  }, mainStore.settings.refreshRate * 1000)
}

watch(() => mainStore.settings.refreshRate, (value) => {
  clearInterval(timer)
  if (value > 0) {
    timer = setInterval(() => {
      getData()
    }, value * 1000)
  }
})

onUnmounted(() => {
  console.log('unmounted')
  clearInterval(timer)
})

// Direct access to this page.

onMounted(() => {
  const route = useRoute()
  const more = `${clientName.value} (${now.value.toLocaleDateString()})`
  document.title = getTitle(route, more)
  getData()
})

//

const editClient = ref(false)
const client = useMainStore().clients.find((c) => c.client_id === clientId)

//

const isLoggedIn = computed(() => {
  return useMainStore().isLoggedIn
})

</script>
