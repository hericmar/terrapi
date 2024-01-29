import {Chart, ChartItem} from "chart.js";
import {Record} from "@/models";

/**
 * `chart-${clientId}-${quantity}` must exists in document!
 * @param clientId
 * @param quantity
 */
export const createChart = (clientId: string, quantity: number): Chart => {
  const ctx = document.getElementById(`chart-${clientId}-${quantity}`);
  if (!ctx) {
    throw new Error(`Element with id chart-${clientId}-${quantity} not found!`)
  }

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

export const updateChart = (chart: Chart, records: Array<Record>, start: Date, end: Date) => {
  const datasets = []

  // Group records by src value and physical quantity
  const map = new Map<string, Array<Record>>();
  records.forEach((item: Record) => {
    /*
    // we have to multiply unix timestamp by 1000, JS uses milliseconds.
    (value as Array<any>).forEach((item: any) => {
      item.timestamp *= 1000;
    });
     */

    let key = `${item.src}`;
    if (item.src.includes("#")) {
      key = item.src.split("#")[0]
    }

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

  chart?.update()
}
