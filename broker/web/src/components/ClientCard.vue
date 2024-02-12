<template>
  <v-card
    class="pa-3"
    max-width="400px"
  >
    <v-card-title>{{ formTitle }}</v-card-title>
    <v-container>
      <v-form
        ref="form"
        @submit.prevent="onAction"
      >
        <v-text-field
          v-model="name"
          ref="nameInputRef"
          style="min-width: 300px"
          label="Client name"
          variant="outlined"
          :rules="[v => !!v || 'Cannot be empty']"
        ></v-text-field>

        <v-text-field
          v-model="clientId"
          v-if="!!clientId"
          label="Client ID"
          variant="outlined"
          readonly
        ></v-text-field>

        <v-text-field
          v-model="token"
          v-if="!!token"
          label="Token"
          variant="outlined"
          persistent-hint
          hint="Copy this token and store it somewhere safe. You will not be able to see it again."
          readonly
        ></v-text-field>
      </v-form>
    </v-container>

    <v-card-actions v-if="client">
      <v-spacer></v-spacer>
      <v-btn
        color="red"
        @click="deleteClientDialog = true"
      >
        Delete
      </v-btn>
    </v-card-actions>

    <v-card-actions>
      <v-spacer></v-spacer>
      <v-btn
        @click="onClose"
      >
        Close
      </v-btn>

      <!-- Action button -->
      <v-btn
        @click="onAction"
      >
        {{ client ? "Save" : "Create" }}
      </v-btn>
    </v-card-actions>
  </v-card>

  <!-- Delete Client Dialog -->
  <v-dialog
    v-model="deleteClientDialog"
    width="auto"
    persistent
  >
    <v-card>
      <v-card-title>Delete client</v-card-title>
      <v-card-text>
        Are you sure you want to delete this client?
      </v-card-text>
      <v-card-actions>
        <v-spacer></v-spacer>
        <v-btn
          @click="deleteClientDialog = false"
        >
          Cancel
        </v-btn>
        <v-btn
          color="red"
          @click="onDelete"
        >
          Delete
        </v-btn>
      </v-card-actions>
    </v-card>
  </v-dialog>
</template>

<script setup lang="ts">
import {nextTick, onMounted, ref} from "vue";
import {useMainStore} from "@/store";
import {Client} from "@/models";
import {useRouter} from "vue-router";

const props = defineProps({
  client: {
    type: Object || null,
    default: null
  },
  onClose: {
    type: Function,
    default() {
      return () => {}
    }
  }
})

const store = useMainStore()

// Local reference to the client object in the store.
const client = ref(undefined as Client | undefined)

const formTitle = ref("Create client")
const form = ref(null)

const name = ref("")
const clientId = ref("")
const token = ref("")

const nameInputRef = ref()

const populateForm = (data: Client | undefined) => {
  if (data) {
    client.value = data

    formTitle.value = "Edit client"
    name.value = (' ' + client.value.name).slice(1)
    clientId.value = (' ' + client.value.client_id).slice(1)

    // TODO: Token is not visible in the edit dialog.
    // token.value = (' ' + props.client.token).slice(1)
  }
}

if (props.client) {
  const data = store.clients.find((c) => c.client_id === props.client.client_id)
  populateForm(data)
}

// Represents the action button, which can be either "Create" or "Edit".
const onAction = async () => {
  // @ts-ignore
  let { valid } = await form.value.validate()
  if (valid) {
    if (client.value) {
      onUpdate()
    } else {
      onCreate()
    }
  }
}

const onCreate = async () => {
  await store.createClient(name.value)
    .then(newClient => {
      populateForm(newClient)
      // router.push({name: 'Environment', params: {clientId: newClient.client_id}})
    })
    .catch(err => {
      console.log(err)
    });

  props.onClose()
}

const onUpdate = async () => {
  if (client.value) {
    await store.renameClient(client.value.client_id, name.value)
    props.onClose()
  }
}

// Delete

const router = useRouter()

const deleteClientDialog = ref(false)

const onDelete = () => {
  if (!client.value) {
    return
  }

  store.deleteClient(client.value.client_id)
    .then(_ => {
      props.onClose()
      router.push({name: 'Home'})
    })
    .catch(_ => {
      props.onClose()
    })
}

// Lifecycle hooks

onMounted(() => {
  nextTick(() => {
    nameInputRef.value.focus()
  })
})
</script>
