<script>
    import Chart from 'chart.js/auto';
    import {onMount} from "svelte";
    import {useParams} from "svelte-navigator";

    export let clientConfig;

    const params = useParams();

    let measurements = {};

    let times = [];
    let datasets = [];
    let datasetsMap = {};

    let data = {}

    for (const sensor of clientConfig.sensors) {
        datasetsMap[sensor.name] = datasets.length;
        times.push([])
        datasets.push({
            label: sensor.name,
            data: []
        });
    }

    onMount(async () => {
        const response = await fetch(`http://127.0.0.1:8000/api/measurement/${$params.id}`);
        const {data} = await response.json();
        if (data) {
            measurements = data;

            for (const m of measurements) {
                if (datasetsMap[m.sensorName] === undefined) {
                    continue;
                }
                let dataset = datasets[datasetsMap[m.sensorName]];
                times[datasetsMap[m.sensorName]].push(m.timestamp);
                dataset.data.push(m.value);
            }
        }

        console.log(clientConfig)

        let i = 0
        for (const sensor of clientConfig.sensors) {
            new Chart(`chart${sensor.name}`, {
                type: "line",
                data: {
                    labels: times[i],
                    datasets: [datasets[i]]
                },
                options: {}
            })
            ++i;
        }
    })
</script>

<h1>{clientConfig.name} </h1>
<p>{clientConfig.clientID}</p>

{#each clientConfig.sensors as sensor}
<canvas id="{`chart${sensor.name}`}" style="width:100%;max-width:700px"></canvas>
{/each}

<table>
    <tr>
        <th>Time</th>
        <th>Sensor</th>
        <th>Value</th>
    </tr>
    <!--
    {#each measurements as m}
    <tr>
        <td>{m.timestamp}</td>
        <td>{m.sensorName}</td>
        <td>{m.value}</td>
    </tr>
    {/each}
    -->
</table>
