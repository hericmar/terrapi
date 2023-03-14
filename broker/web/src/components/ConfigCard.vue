<template>
  <v-card
    :loading="loading"
  >
    <v-toolbar
      color="rgba(0, 0, 0, 0)"
    >
      <v-toolbar-title
        class="text-h6"
      >

      </v-toolbar-title>

      <template v-slot:append>
        <v-btn
          icon="mdi-close"
          color="grey-darken-1"
          @click="onClose"
        ></v-btn>
      </template>
    </v-toolbar>

    <v-card-text class="overflow-auto">
      <pre
        class="language-toml border-b-md"
      >{{ config }}</pre>
    </v-card-text>
  </v-card>
</template>

<script lang="ts">
import Prism from 'prismjs';
import 'prismjs/components/prism-toml'
import 'prismjs/themes/prism.css'
import {onMounted, ref} from "vue";
import api from "@/api";

export default {
  props: {
    clientId: String,
    onClose: Function
  },
  setup(props: any) {
    const loading = ref(true)
    const config = ref("")

    onMounted(() => {
      api.fetchConfig(props.clientId)
        .then((result) => {
          loading.value = false;
          config.value = result.config

          Prism.highlightAll()
        })
    })

    return {
      loading,
      config
    }
  }
}
</script>
