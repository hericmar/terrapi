import {get, writable} from 'svelte/store';

export const page = writable('Dashboard');

export const user = writable({});

export const configs = writable([]);

export const findConfig = (clientId) => {
    return get(configs).find((config) => config.clientID === clientId)
}

const physicalQuantities = [
    "temperature",
    "humidity"
]

export function processData(res, data) {
    let measurements = data;
    measurements = [...measurements].reverse()

    // TODO empty arrays

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
}
