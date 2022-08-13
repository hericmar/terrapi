<script>
	import {onMount} from "svelte";
	import {page, configs} from "./stores"
	import Admin from "./routes/Admin.svelte";
	import Client from "./routes/Client.svelte";
	import NotFound from "./routes/NotFound.svelte";
	import {fetchConfigs} from "./api";

	let pages = [
		{id: "Admin", name: "Admin", component: Admin}
	]

	onMount(async () => {
		const response = await fetchConfigs();
		const {data} = await response.json();

		$configs = data

		for (const config of $configs) {
			pages = [...pages, {
				id: `Client#${config.clientID}`,
				name: config.name,
				component: Client
			}];
		}
	});

	const getComponent = function () {
		try {
			return pages.find((p) => p.id === $page).component;
		} catch (e) {
			return NotFound;
		}
	}
</script>

<header>
	<nav>
		<ul>
			{#each $configs as config}
				<li>
					<a href="/" on:click|preventDefault={() => $page = `Client#${config.clientID}`}>{config.name}</a>
				</li>
			{/each}

			<li class="line"></li>

			<li>
				<a href="/" on:click|preventDefault={() => $page = 'Admin'}>Administrace</a>
			</li>
		</ul>
	</nav>
</header>

<main>
	<svelte:component this={getComponent()} />
</main>

<style>
	nav {
		margin: 16px;
	}

	nav ul {
		padding: 0
	}

	nav ul li {
		display: block;
		padding: 8px;
	}

	nav ul li.line {
		height: 1px;
		margin: 4px 0;
		padding: 0;
		background-color: #cccccc;
	}

	main {
		text-align: center;
		padding: 1em;
		max-width: 240px;
		margin: 0 auto;
	}

	@media (min-width: 640px) {
		main {
			max-width: none;
		}
	}
</style>