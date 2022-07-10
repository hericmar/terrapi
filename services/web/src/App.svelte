<script>
	import {onMount} from "svelte";

	export let configs = [];
	onMount(async () => {
		const response = await fetch('http://127.0.0.1:8000/api/config')
		const {data} = await response.json();
		configs = data
	})
</script>

<main>
	<ul>
		{#each configs as config}
			<li>{config.name}</li>
		{/each}
	</ul>
</main>

<style>
	main {
		text-align: center;
		padding: 1em;
		max-width: 240px;
		margin: 0 auto;
	}

	h1 {
		color: #ff3e00;
		text-transform: uppercase;
		font-size: 4em;
		font-weight: 100;
	}

	@media (min-width: 640px) {
		main {
			max-width: none;
		}
	}
</style>