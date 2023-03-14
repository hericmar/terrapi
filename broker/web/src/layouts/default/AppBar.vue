<template>
  <v-app-bar
    elevation="1"
  >
    <v-app-bar-title>
      <v-icon icon="mdi-circle-slice-4" />
        Terrapi
    </v-app-bar-title>
    <v-spacer></v-spacer>

    <span
      class="mr-2"
      v-if="isLoggedIn"
    >
      Admin
    </span>
    <v-btn
      v-if="isLoggedIn"
      @click="mainStore.logout"
    >
      Logout
    </v-btn>
    <v-btn
      v-else
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
import {computed, ref} from "vue";
import { useMainStore } from '@/store/index'

const dialog = ref(false)
const form =  ref(null)
const loading = ref(false)
const password = ref("")
const errMessage = ref("")

const mainStore = useMainStore()

const isLoggedIn = computed(() => mainStore.isLoggedIn)

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
  dialog.value = false
}

const onLogin = async () => {
  // @ts-ignore
  let { valid } = await form.value.validate()
  if (valid) {
    loading.value = true
    mainStore.login(password.value)
      .then((result) => {
        console.log("loading.value = false")
        loading.value = false

        if (result) {
          dialog.value = false
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
