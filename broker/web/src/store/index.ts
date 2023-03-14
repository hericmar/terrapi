import { defineStore } from 'pinia'
import api from "@/api";

export const useMainStore = defineStore({
  id: 'mainStore',

  state: () => ({
    isLoggedIn: false,
    clients: []
  }),

  actions: {
    async login(password: string): Promise<boolean> {
      return api.login(password)
        .then(async (isPasswordOk) => {
          if (isPasswordOk) {
            this.isLoggedIn = true;
            api.setPassword(password)

            return true;
          } else {
            this.isLoggedIn = false;

            return false;
          }
        })
        .catch(async (err) => {
          this.isLoggedIn = false;

          return false;
        })
    },
    logout() {
      this.isLoggedIn = false
      api.setPassword("")
    },
    async fetchClients() {
      api.listClientPreview()
        .then((result) => {
          this.clients = result
        })
    },
    async renameClient(clientId: string, newName: string) {
      api.renameClient(clientId, newName)
        .then(result => {
          const client = this.clients.find(item => {
            // @ts-ignore
            return item.client_id === props.clientId
          })
          // @ts-ignore
          client.name = newName
        })
    }
  }
})
