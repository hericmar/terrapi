<script>
    import Chart from 'chart.js/auto';
    import {onMount} from "svelte";
    import {findConfig, page, processData} from "../stores";
    import {fetchMeasurements} from "../api";

    const clientID = $page.split('#')[1]
    export let clientConfig = findConfig(clientID);

    let requestLimit = 20;
    let requestOffset = 0;

    let measurements = [];

    let res = {
        datasetsMap: {},
        times: [],
        datasets: []
    }

    let data = {}

    const charts = [];

    let sensors = []

    async function fetchData() {
        const response = await fetchMeasurements(clientID, requestOffset, requestLimit);
        const {data} = await response.json();
        measurements = data

        if (data) {
            processData(res, data)
            for (const sensorLabel of Object.keys(res.datasetsMap)) {
                if (!sensors.find(label => label === sensorLabel)) {
                    sensors = [...sensors, sensorLabel]
                }
            }
            return data.length
        } else {
            return 0;
        }
    }

    onMount(async () => {
        const sensorsCount = Object.keys(res.datasetsMap).length;
        // requestLimit = sensorsCount * 100;
        requestLimit = 2;

        await fetchData()

        let i = 0
        for (const sensorLabel of Object.keys(res.datasetsMap)) {
            const chart = new Chart(`chart${sensorLabel}`, {
                type: "line",
                data: {
                    labels: res.times[i],
                    datasets: [res.datasets[i]]
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

<h1>{clientConfig.name} <span style="font-size: 0.5em; color: gray">{clientConfig.clientID}</span></h1>

{#each sensors as sensorLabel}
    <canvas id="{`chart${sensorLabel}`}" style="width:100%;max-width:700px"></canvas>
{/each}

<button on:click={handleClickPrev}>&lt;</button>
<button on:click={handleClickNext}>&gt;</button>

<!--
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
-->
