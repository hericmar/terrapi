import {Chart, ChartItem} from "chart.js";
import {Measurement, Quantity} from "@/models";

const CHART_CONFIG = {
  [Quantity.Humidity]: {
    min: 0,
    max: 100,
    unit: '%'
  },
  [Quantity.Signal]: {
    min: 0,
    max: 2,
    unit: ''
  },
  [Quantity.Temperature]: {
    min: 0,
    max: 60,
    unit: '°C'
  },
  "default": {
    min: 0,
    max: 100,
    unit: ''
  }
}

/**
 * `chart-${clientId}-${quantity}` must exists in document!
 * @param clientId
 * @param quantity
 */
export const createChart = (clientId: string, quantity: Quantity): Chart => {
  const ctx = document.getElementById(`chart-${clientId}-${quantity}`);
  if (!ctx) {
    throw new Error(`Element with id chart-${clientId}-${quantity} not found!`)
  }

  const config = CHART_CONFIG[quantity] || CHART_CONFIG["default"]

  return new Chart(ctx as ChartItem, {
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
            round: 'second',
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
        },
        y: {
          min: config.min,
          max: config.max,
          ticks: {
            callback: (value, index, ticks) => {
              return `${value}${config.unit}`
            }
          }
        }
      }
    }
  });
}

export const updateChart = (chart: Chart, records: Array<Measurement>, start: Date, end: Date) => {
  const datasets = []

  // Group records by src value and physical quantity
  const map = new Map<string, Array<Measurement>>();
  records.forEach((item: Measurement) => {
    // we have to multiply unix timestamp by 1000, JS uses milliseconds.
    item.timestamp *= 1000;

    const key = `${item.source}`;
    const collection = map.get(key);
    if (!collection) {
      map.set(key, [item]);
    } else {
      collection.push(item);
    }
  });

  for (const [key, value] of map.entries()) {
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

  chart.update()
}
