import { writable } from 'svelte/store';

export const user = writable({});

const physicalQuantities = [
    "temperature",
    "humidity"
]

export function prepareData(res, config) {
    /*
    for (const sensor of config.sensors) {
        res.datasetsMap[sensor.name] = res.datasets.length;
        res.times.push([])

        res.datasets.push({
            label: sensor.name,
            data: []
        });
    }
     */
}

export function processData(res, data) {
    let measurements = data;
    measurements = [...measurements].reverse()

    for (const m of measurements) {
        const label = `${m.sensorName}.${physicalQuantities[m.physicalQuantity]}`;

        if (res.datasetsMap[label] === undefined) {
            res.datasetsMap[label] = res.datasets.length;
            res.times.push([])
            res.datasets.push({
                label: label,
                data: []
            })
        }

        const i = res.datasetsMap[label];

        res.times[i].push(m.timestamp);
        res.datasets[i].data.push(m.value);
    }

    /*
    const sensorsCount = Object.keys(res.datasetsMap).length;

    for (let i = 0; i < sensorsCount; ++i) {
        if (res.times[i].length !== 0) {
            res.times[i].splice(0, res.times[i].length)
        }
        if (res.datasets[i].data.length) {
            res.datasets[i].data.splice(0, res.datasets[i].data.length)
        }
    }

    for (const m of measurements) {
        if (res.datasetsMap[m.sensorName] === undefined) {
            continue;
        }
        let dataset = res.datasets[res.datasetsMap[m.sensorName]];

        res.times[res.datasetsMap[m.sensorName]].push(m.timestamp);
        dataset.data.push(m.value);
    }
     */
}
