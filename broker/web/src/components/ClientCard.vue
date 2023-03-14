<template>
  <v-card
    class="pa-3"
    max-width="400px"
  >
    <v-card-title>Create client</v-card-title>
    <v-container>
      <v-form ref="form">
        <v-text-field
          v-model="name"
          style="min-width: 300px"
          label="Client name"
          variant="outlined"
          :rules="[v => !!v || 'Cannot be empty']"
        ></v-text-field>

        <v-text-field
          v-model="clientId"
          v-if="!isEditing"
          label="Client ID"
          variant="outlined"
          readonly
          :disabled="!!clientId"
        ></v-text-field>

        <v-text-field
          v-model="token"
          v-if="!isEditing"
          label="Token"
          variant="outlined"
          readonly
          :disabled="!!token"
        ></v-text-field>
      </v-form>
    </v-container>
    <v-card-actions>
      <v-spacer></v-spacer>
      <v-btn
        @click="onClose"
      >
        Close
      </v-btn>
      <v-btn
        @click="onCreate"
      >
        Create
      </v-btn>
    </v-card-actions>
  </v-card>
</template>

<script setup lang="ts">
import {ref} from "vue";
import api from "@/api";

const props = defineProps({
  onClose: {
    type: Function,
    default() {
      return () => {}
    }
  },
  editModel: {
    type: Object,
    default() {
      return null
    }
  }
})

const form = ref(null)
const isEditing = ref(props.editModel !== null)

const name = ref("")
const clientId = ref("")
const token = ref("")

// Create

const onCreate = async () => {
  // @ts-ignore
  let { valid } = await form.value.validate()
  if (valid) {
    api.createClient(name.value)
      .then(result => {
        clientId.value = result.client_id
        token.value = result.password
      })
  }
}

// Edit

</script>
