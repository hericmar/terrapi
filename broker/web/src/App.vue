<template>
  <router-view />
</template>

<script lang="ts" setup>
import {useMainStore} from "@/store";

const mainStore = useMainStore();
mainStore.init();
</script>
