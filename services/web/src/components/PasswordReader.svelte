<script>
    let show = false;
    export let password;

    function hidePassword(str) {
        return "---"
    }

    function toggle() {
        show = !show;
    }
</script>

<span>
    {#if show}
        {password}
    {:else}
        {hidePassword(password)}
    {/if}
    <button on:click={toggle}>Odhalit</button>
</span>
