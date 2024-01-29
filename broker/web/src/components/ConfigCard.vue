// This is old client detail card.
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
      <h3 class="mb-6 font-weight-regular">Credentials</h3>
      <v-text-field
        v-model="client.client_id"
        label="Client ID"
        variant="outlined"
        readonly
      ></v-text-field>

      <v-text-field
        v-model="clientPassword"
        label="Client password"
        :type="showClientPassword ? 'text' : 'password'"
        variant="outlined"
        readonly
      >
        <template v-slot:append>
          <v-btn
            class="mt-n3"
            variant="text"
            icon="mdi-eye"
            @click="showClientPassword = !showClientPassword"
          ></v-btn>

          <v-btn
            class="mt-n3"
            variant="text"
            icon="mdi-plus"
            disabled
          ></v-btn>
        </template>
      </v-text-field>

      <h3 class="my-6 font-weight-regular">Configuration</h3>
      <pre
        class="language-toml mb-8 border-b-md"
      >{{ config }}</pre>

      <h3 class="my-6 font-weight-regular">Danger zone</h3>
      <v-expansion-panels class="mb-8">
        <v-expansion-panel>
          <v-expansion-panel-title>Delete client</v-expansion-panel-title>
          <v-expansion-panel-text>
            <div class="d-flex align-center mt-2">
              <span>This action cannot be reverted.</span>
              <v-spacer></v-spacer>
              <v-btn
                color="red"
                flat
                @click="deleteDialog = true"
              >Delete</v-btn>
            </div>
          </v-expansion-panel-text>
        </v-expansion-panel>
      </v-expansion-panels>
    </v-card-text>

    <v-dialog v-model="deleteDialog" max-width="300px">
      <v-card>
        <v-card-text>Do you really want to delete <b>{{ client.name }}</b>?</v-card-text>
        <v-card-actions>
          <v-spacer></v-spacer>
          <v-btn
            color="red"
            @click="onDelete"
          >Yes</v-btn>
          <v-btn @click="deleteDialog = false">No</v-btn>
        </v-card-actions>
      </v-card>
    </v-dialog>
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

    const clientPassword = ref("asdf")
    const showClientPassword = ref(false)

    const deleteDialog = ref(false)

    const mainStore = useMainStore();
    const client = computed(() => {
      let found = mainStore.clients.find(item => {
        return item.client_id === props.clientId
      })
      if (!found) {
        found = { client_id: "", token: "", name: ""}
      }
      return found
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

    const onDelete = () => {
      mainStore.deleteClient(props.clientId)
        .then(() => {
          deleteDialog.value = false;
          props.onClose()
        })
    }

    onMounted(() => {
      mainStore.fetchConfig(props.clientId)
        .then(result => {
          loading.value = false;
          config.value = result.config
          clientPassword.value = result.token

          if (!config.value) {
            config.value = "# No client config uploaded.\n" +
              "# Make sure your client is running,\n" +
              "# and it has valid client id and token set."
          }

          Prism.highlightAll()
        })
        .catch(_ => {
          props.onClose()
        })
    })

    return {
      loading,
      config,

      clientPassword,
      showClientPassword,

      client,
      isEditing,
      onEditStart,
      onEditEnd,
      nameInputRef,

      deleteDialog,
      onDelete,
    }
  }
}
</script>
