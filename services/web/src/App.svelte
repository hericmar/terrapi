<script>
	import {onMount} from "svelte";
	import {Router, Route, Link} from "svelte-navigator"
	import Admin from "./routes/Admin.svelte";
	import Client from "./routes/Client.svelte";

	export let configs = [];
	onMount(async () => {
		const response = await fetch('http://127.0.0.1:8000/api/config')
		const {data} = await response.json();
		configs = data
	})

	export const findConfig = (clientId) => {
		return configs.find((config) => config.clientID === clientId)
	}
</script>

<Router>
	<header>
		<nav>
			<ul>
				{#each configs as config}
					<li><Link to="{`client/${config.clientID}`}">{config.name}</Link></li>
				{/each}

				<li class="line"></li>

				<li><Link to="admin">Administrace</Link></li>
			</ul>
		</nav>
	</header>

	<main>
		<Route path="client/:id" let:params>
			<Client clientConfig="{findConfig(params.id)}"/>
		</Route>

		<Route path="admin">
			<Admin />
		</Route>

		<Route path="*">
			...
		</Route>
	</main>
</Router>

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