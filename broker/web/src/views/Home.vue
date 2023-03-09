<template>
  <h1></h1>
  <div>
    <canvas id="chart"></canvas>
  </div>
</template>

<script lang="ts" setup>
import {Chart, ChartItem, registerables} from 'chart.js';
import {onMounted} from 'vue';
import 'chartjs-adapter-moment';

Chart.register(...registerables);

onMounted(() => {
  const ctx = document.getElementById('chart');

  // we have to multiply unix timestamp by 1000, JS uses milliseconds.
  const records = {
    "measurements": {
      "dht11": [
        {
          "value": 12.0,
          "timestamp": 1678300403000
        },
        {
          "value": 14.0,
          "timestamp": 1678303816000
        },
        {
          "value": 12.0,
          "timestamp": 1678307603000
        },
        {
          "value": 14.0,
          "timestamp": 1678311016000
        }
      ]
    },
    "events": {
      "humidifier": [
        {
          "state": false,
          "timestamp": 1678311169
        },
        {
          "state": true,
          "timestamp": 1678311186
        },
        {
          "state": false,
          "timestamp": 1678311169
        },
        {
          "state": true,
          "timestamp": 1678311186
        },
        {
          "state": false,
          "timestamp": 1678311169
        },
        {
          "state": true,
          "timestamp": 1678311186
        }
      ]
    }
  }

  new Chart(ctx as ChartItem, {
    type: 'line',
    data: {
      datasets: [{
        label: "dht11",
        data: records.measurements["dht11"],
      }]
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
            round: 'hour',
            unit: 'second',
            tooltipFormat: 'MMM Do, \'YY'
          }
        }
      }
    }
  });
})
</script>
