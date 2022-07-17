<script>
    import Chart from 'chart.js/auto';
    import {onMount} from "svelte";
    import {useParams} from "svelte-navigator";

    export let clientConfig;

    const params = useParams();

    let requestLimit = 20;
    let requestOffset = 0;

    let measurements = [];

    let times = [];
    let datasets = [];
    let datasetsMap = {};

    let data = {}

    const charts = [];

    function prepareData(data) {
        measurements = data;
        measurements = [...measurements].reverse()

        const sensorsCount = Object.keys(datasetsMap).length;

        for (let i = 0; i < sensorsCount; ++i) {
            if (times[i].length !== 0) {
                times[i].splice(0, times[i].length)
            }
            if (datasets[i].data.length) {
                datasets[i].data.splice(0, datasets[i].data.length)
            }
        }

        for (const m of measurements) {
            if (datasetsMap[m.sensorName] === undefined) {
                continue;
            }
            let dataset = datasets[datasetsMap[m.sensorName]];

            times[datasetsMap[m.sensorName]].push(m.timestamp);
            dataset.data.push(m.value);
        }
    }

    async function fetchData() {
        const response = await fetch(`http://127.0.0.1:8000/api/measurement/${$params.id}?offset=${requestOffset}&limit=${requestLimit}`);
        const {data} = await response.json();

        if (data) {
            prepareData(data)
            return data.length
        } else {
            return 0;
        }
    }

    onMount(async () => {
        /*
        if (!clientConfig) {
            const response = await fetch('http://127.0.0.1:8000/api/config');
            const {data} = await response.json();
            configs = data;
            clientConfig = findConfig($params.id);
        }
         */

        for (const sensor of clientConfig.sensors) {
            datasetsMap[sensor.name] = datasets.length;
            times.push([])
            datasets.push({
                label: sensor.name,
                data: []
            });
        }

        const sensorsCount = Object.keys(datasetsMap).length;
        requestLimit = sensorsCount * 100;

        await fetchData()

        let i = 0
        for (const sensor of clientConfig.sensors) {
            const chart = new Chart(`chart${sensor.name}`, {
                type: "line",
                data: {
                    labels: times[i],
                    datasets: [datasets[i]]
                },
                options: {}
            })
            charts.push(chart);
            ++i;
        }
    })

    async function handleClickPrev() {
        requestOffset += requestLimit;

        const rowsAffected = await fetchData() === 0;
        if (rowsAffected) {
            requestOffset -= requestLimit;
        }

        charts.forEach((chart) => chart.update());
    }

    async function handleClickNext() {
        if (requestOffset === 0) {
            return;
        }

        requestOffset -= requestLimit;
        if (requestOffset < 0) {
            requestOffset = 0;
        }

        await fetchData();

        charts.forEach((chart) => chart.update());
    }
</script>

<h1>{clientConfig.name} </h1>
<p>{clientConfig.clientID}</p>

{#each clientConfig.sensors as sensor}
<canvas id="{`chart${sensor.name}`}" style="width:100%;max-width:700px"></canvas>
{/each}

<button on:click={handleClickPrev}>&lt;</button>
<button on:click={handleClickNext}>&gt;</button>

<table>
    <tr>
        <th>Time</th>
        <th>Sensor</th>
        <th>Value</th>
    </tr>
    {#each [...measurements].reverse() as m}
    <tr>
        <td>{m.timestamp}</td>
        <td>{m.sensorName}</td>
        <td>{m.value}</td>
    </tr>
    {/each}
</table>
