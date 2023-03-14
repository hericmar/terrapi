<template>
  <v-card
    :loading="loading"
    min-width="400px"
  >
    <v-toolbar
      color="rgba(0, 0, 0, 0)"
    >
      <div
        v-if="!isEditing"
        class="ml-6 text-h6"
      >
        {{ client.name }}
      </div>

      <v-text-field
        class="ml-6"
        ref="nameInputRef"
        v-if="isEditing"
        v-model="client.name"
        variant="underlined"
        @focusout="onEditEnd"
        @keydown.enter="onEditEnd"
      ></v-text-field>

      <v-btn
        v-if="!isEditing"
        class="ml-4"
        icon="mdi-pencil"
        color="grey-darken-1"
        @click="onEditStart"
      ></v-btn>

      <v-spacer></v-spacer>

      <v-btn
        icon="mdi-close"
        color="grey-darken-1"
        @click="onClose"
      ></v-btn>
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
import {computed, nextTick, onMounted, ref} from "vue";
import api from "@/api";
import {useMainStore} from "@/store";

export default {
  props: {
    clientId: String,
    onClose: Function
  },
  setup(props: any) {
    const loading = ref(true)
    const config = ref("")

    const mainStore = useMainStore();
    const client = computed(() => {
      return mainStore.clients.find(item => {
        // @ts-ignore
        return item.client_id === props.clientId
      })
    })

    const nameInputRef = ref()
    const isEditing = ref(false)
    const onEditStart = () => {
      isEditing.value = true
      nextTick(() => {
        nameInputRef.value.focus()
      })
    }
    const onEditEnd = () => {
      isEditing.value = false
      // @ts-ignore
      mainStore.renameClient(props.clientId, client.value.name)
    }

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
      config,
      client,
      isEditing,
      onEditStart,
      onEditEnd,
      nameInputRef
    }
  }
}
</script>
