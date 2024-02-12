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

  <v-card
    class="pa-4 my-4"
    v-for="(type, i) in chartTypes"
    :key="i"
  >
    <v-toolbar color="rgba(0, 0, 0, 0)">
      <v-toolbar-title class="text-h6">
        {{ CODE_TO_NAME.get(type as number) }}
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
  <v-card class="pa-2 my-4">
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
import {computed, defineProps, nextTick, onBeforeUpdate, onMounted, onUnmounted, PropType, ref, watch} from "vue";
import {Client} from "@/models";
import {CODE_TO_NAME, DataSets, DEVICE_STATE_TO_NAME, fetchRecords, processRecords} from "@/records/data";
import {createChart, updateChart} from "@/records/chart";
import {onBeforeRouteUpdate, useRoute, useRouter} from "vue-router";
import {useMainStore} from "@/store";
import ClientCard from "@/components/ClientCard.vue";
import {getTitle} from "@/router";

/*
const props = defineProps({
  client: {
    type: Object as PropType<Client>,
    required: true
  }
})
 */

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

const chartTypes = ref<Array<number | string>>([])

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
  chartTypes.value.splice(0, chartTypes.value.length)

  fetchRecords(clientId, now.value).then((records) => {
    events.value = records.events.map((e) => `${new Date(e.timestamp *= 1000).toLocaleTimeString()}: ${e.source} ${DEVICE_STATE_TO_NAME.get(e.state)}`)

    const groupedRecords: DataSets = processRecords(records)

    // This will trigger a re-render and create charts.
    chartTypes.value.push(...groupedRecords.measurements.keys())

    nextTick(() => {
      // Required, because the chart canvas is not yet rendered.
      chartTypes.value.forEach((type) => {
        const chart = createChart(clientId, type as number)

        const records = groupedRecords.measurements.get(type as number)
        if (!records) {
          return
        }

        const startOfDay = new Date(now.value.getFullYear(), now.value.getMonth(), now.value.getDate(), 0, 0, 0);
        const endOfDay = new Date(now.value.getFullYear(), now.value.getMonth(), now.value.getDate(), 24, 0, 0);
        updateChart(chart, records, startOfDay, endOfDay)
      })
    })
  })
}

// autorefresh

let timer: any = undefined;
if (mainStore.settings.autoRefresh) {
  timer = setInterval(() => {
    getData()
  }, mainStore.settings.refreshRate * 1000)
}

watch(() => mainStore.settings.refreshRate, (value) => {
  if (mainStore.settings.autoRefresh) {
    clearInterval(timer)
    timer = setInterval(() => {
      getData()
    }, value * 1000)
  }
})

watch(() => mainStore.settings.autoRefresh, (value) => {
  if (value) {
    timer = setInterval(() => {
      getData()
    }, mainStore.settings.refreshRate * 1000)
  } else {
    clearInterval(timer)
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
