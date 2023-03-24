import { defineStore } from 'pinia'
import api from "@/api";
import { Client } from "@/models";

export type RootState = {
  isLoggedIn: boolean,
  clients: Client[]
}

export const useMainStore = defineStore({
  id: 'mainStore',

  state: () => ({
    isLoggedIn: false,
    clients: []
  } as RootState),

  actions: {
    init() {
      const token = localStorage.getItem("token");
      if (token) {
        this.setToken(token)
      } else {
        this.logout();
      }
    },

    setToken(token: string) {
      this.isLoggedIn = true;
      localStorage.setItem("token", token)
      api.setToken(token)
    },

    async login(password: string): Promise<boolean> {
      return api.login(password)
        .then(async (response) => {
            this.setToken(response.token)

            return true;
        })
        .catch(async (err) => {
          this.logout();

          return false;
        })
    },
    logout() {
      this.isLoggedIn = false
      localStorage.removeItem("token");
      api.setToken("")
    },
    async sortClients() {
      this.clients = this.clients.sort((lhs: Client, rhs: Client) => {
        return lhs.name.localeCompare(rhs.name)
      })
    },
    async fetchClients() {
      api.listClientPreview()
        .then((result) => {
          this.clients = result
          this.sortClients()
        })
    },
    //
    async createClient(name: string) {
      const result = {
        name: name,
        client_id: "",
        password: ""
      }
      await api.createClient(name)
        .then(async response => {
          if (response.status === 401) {
            this.logout()
            throw new Error("unauthorized");
          }

          const { client_id, password } = await response.json()
          result.client_id = client_id
          result.password = password

          this.clients.push(result)
          this.sortClients()
        })

      return result
    },
    async renameClient(clientId: string, newName: string) {
      api.renameClient(clientId, newName)
        .then(response => {
          if (response.status === 401) {
            this.logout()
            throw new Error("unauthorized");
          }

          const client = this.clients.find(item => {
            return item.client_id === clientId
          })

          if (client) {
            client.name = newName
          }
        })
    },
    async deleteClient(clientId: string) {
      api.deleteClient(clientId)
        .then(response => {
          if (response.status === 401) {
            this.logout()
            throw new Error("unauthorized");
          }

          const index = this.clients.map(item => item.client_id).indexOf(clientId);
          this.clients.splice(index, 1);
        })
    },
    //
    async fetchConfig(clientId: string) {
      let result = { config: "", token: "" }
      await api.fetchConfig(clientId)
        .then(async response => {
          if (response.status === 401) {
            this.logout()
            throw new Error("unauthorized");
          }

          result = await response.json()
        })

      return result
    }
  }
})
