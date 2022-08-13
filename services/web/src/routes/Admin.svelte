<script>
    import {user} from '../stores.js';
    import PasswordReader from "../components/PasswordReader.svelte";
    import {fetchClients} from "../api";

    let clients = [];

    let userValue;

    user.subscribe(value => {
        userValue = value;
    });

    function isLoggedIn() {
        return userValue.password !== null && userValue.password !== undefined;
    }

    //--------------------------------------------------------------------------
    let inputPassword = '';
    let inputMessage = '';

    async function onSubmit(e) {
        inputMessage = '';

        const response = await fetchClients(inputPassword);

        if (response.status === 200) {
            inputMessage = '';
            user.set({
                password: inputPassword
            });
            const {data} = await response.json();
            clients = data;
        } else {
            inputMessage = 'Špatné heslo.';
        }
    }

    function hidePassword(password) {

    }

    function togglePassword(client) {
        client.showPassword = !client.showPassword;
    }
</script>

<section id="admin">
    <h1>Administrace</h1>

    {#if !userValue.password}
        <p>Pro pokračování je nutné se přihlásit administrátorským účtem.</p>

        <form id="login-form" on:submit|preventDefault={onSubmit}>
            <input
                type="password"
                id="password"
                name="password"
                placeholder="Heslo"
                required
                bind:value={inputPassword}>

            <button>Přihlásit</button>

            <p>{inputMessage}</p>
        </form>
    {/if}

    {#if userValue.password}
    <table>
        <thead>
            <tr>
                <td>Název zařízení</td>
                <td>ID</td>
                <td>Heslo</td>
            </tr>
        </thead>
        <tbody>
        {#each clients as client}
            <tr>
                <td>{client.name}</td>
                <td>{client.ID}</td>
                <td>
                    <span>
                        <PasswordReader password={client.password} />
                    </span>
                </td>
            </tr>
        {/each}
        </tbody>
    </table>
    {/if}
</section>

<style>
    section#admin {
        text-align: center;
    }

    #login-form {
        display: inline-block;
    }

    table thead {
        font-weight: bold;
    }
</style>
