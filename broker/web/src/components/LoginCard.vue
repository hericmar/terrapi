<template>
  <v-card
    class="pa-3"
    :loading="loading"
  >
    <v-card-title>Login</v-card-title>
    <v-card-subtitle>Fill in the admin password.</v-card-subtitle>
    <v-container>
      <v-form
        ref="form"
        class="mt-2"
        @submit.prevent="onLogin"
      >
        <v-text-field
          v-model="password"
          :error="!!errMessage"
          ref="passwordInputRef"
          type="password"
          label="Password"
          variant="outlined"
          required
          :hint="errMessage"
          persistent-hint
          :rules="[v => !!v || 'Item is required']"
        >
        </v-text-field>
      </v-form>
    </v-container>
    <v-card-actions>
      <v-spacer></v-spacer>
      <v-btn
        color="primary"
        @click="closeDialog"
      >
        Close
      </v-btn>
      <v-btn
        color="primary"
        @click="onLogin"
      >
        Login
      </v-btn>
    </v-card-actions>
  </v-card>
</template>

<script lang="ts" setup>
import {ref} from "vue";
import {useMainStore} from "@/store";

const props = defineProps({
  onClose: {
    type: Function,
    required: true
  }
})

const errMessage = ref("")
const form = ref(null)
const loading = ref(false)
const password = ref("")

const mainStore = useMainStore()

const markPasswordAsWrong = async () => {
  errMessage.value = 'Invalid password'
  // @ts-ignore
  await form.value.validate();
}

const markPasswordAsOk = async () => {
  errMessage.value = ''
  password.value = ''
}

const closeDialog = () => {
  // @ts-ignore
  form.value.reset();
  props.onClose()
}

const onLogin = async () => {
  // @ts-ignore
  let { valid } = await form.value.validate()
  if (valid) {
    loading.value = true
    mainStore.login(password.value)
      .then((result) => {
        loading.value = false

        if (result) {
          props.onClose()
          setTimeout(() => {
            markPasswordAsOk()
          }, 500)
        } else {
          markPasswordAsWrong()
        }
      })
  }
}
</script>
