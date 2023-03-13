<template>
  <v-app-bar elevation="1">
    <v-app-bar-title>
      <v-icon icon="mdi-circle-slice-4" />
        Terrapi Dashboard
    </v-app-bar-title>
    <v-spacer></v-spacer>
    <v-btn
      @click="dialog = true"
    >
      Login
    </v-btn>
  </v-app-bar>
  <v-dialog
    v-model="dialog"
    persistent
    min-width="400"
    max-width="600"
  >
    <v-card
      class="pa-3"
      :loading="loading"
    >
      <v-card-title>Login</v-card-title>
      <v-card-subtitle>Fill the admin password.</v-card-subtitle>
      <v-container>
        <v-form
          ref="form"
          class="mt-2"
          @submit.prevent="onLogin"
        >
          <v-text-field
            v-model="password"
            :error="!!errMessage"
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
  </v-dialog>
</template>

<script lang="ts" setup>
import {ref} from "vue";
import api from "@/api";

const dialog = ref(true)
const form =  ref(null)
const loading = ref(false)
const password = ref("")
const errMessage = ref("")

const markPasswordAsWrong = async () => {
  errMessage.value = 'Invalid password'
  // @ts-ignore
  await form.value.validate();
}

const markPasswordAsOk = async () => {
  errMessage.value = ''
  password.value = ''
  // @ts-ignore
  await form.value.resetValidation();
}

const closeDialog = async () => {
  errMessage.value = ''
  password.value = ''
  dialog.value = false
}

const onLogin = async () => {
  // @ts-ignore
  let { valid } = await form.value.validate()
  if (valid) {
    valid = false
    loading.value = true
    api.login(password.value)
      .then(async (isPasswordOk) => {
        if (isPasswordOk) {
          api.setPassword(password.value)
          await markPasswordAsOk()

          // todo -> set password
          dialog.value = false
        } else {
          await markPasswordAsWrong()
        }
        loading.value = false
      })
      .catch(async (err) => {
        loading.value = false
        await markPasswordAsWrong()
      })
  }
}

</script>
