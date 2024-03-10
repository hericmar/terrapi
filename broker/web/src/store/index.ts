import { defineStore } from 'pinia'
import api from "@/api";
import { Client } from "@/models";

const SETTINGS_STORAGE_KEY = "settings"
export type Settings = {
  // In seconds
  refreshRate: number
}

const getDefaultSettings = () => ({
  refreshRate: 60,
})

const getSettings = (): Settings => {
  const settings = localStorage.getItem(SETTINGS_STORAGE_KEY)
  if (settings) {
    return JSON.parse(settings)
  }

  return getDefaultSettings()
}

export type RootState = {
  drawer: boolean,
  snackbarMessage: string,
  settings: Settings,
  isLoggedIn: boolean,
  clients: Client[]
  date: Date
}

export const useMainStore = defineStore({
  id: 'mainStore',

  state: () => ({
    drawer: true,
    snackbarMessage: "",
    settings: getSettings(),
    isLoggedIn: false,
    clients: [] as Client[],
    date: new Date()
  } as RootState),

  actions: {
    init() {
      api.auth.user()
        .then(async (response) => {
          if (response.status === 200) {
            this.isLoggedIn = true;
          }
        })

      return this.fetchClients()
    },

    // TODO: create partial type
    updateSettings(partialSettings: any) {
      this.settings = {
        ...this.settings,
        ...partialSettings
      }

      localStorage.setItem(SETTINGS_STORAGE_KEY, JSON.stringify(this.settings))
    },

    async login(password: string) {
      return await api.auth.login(password)
        .then(async (response) => {
          if (!response) {
            return false;
          }

          if (response.status === 200) {
            this.isLoggedIn = true;

            return true;
          }
        })
    },
    logout() {
      api.auth.logout().then(() => {
        this.isLoggedIn = false
      })
    },
    sortClients() {
      this.clients = this.clients.sort((lhs: Client, rhs: Client) => {
        return lhs.name.localeCompare(rhs.name)
      })
    },
    async fetchClients() {
      api.client.list()
        .then(async (result) => {
          this.clients = await result.json()
          this.sortClients()
        })
    },
    //
    async createClient(name: string) {
      let result: Client = {
        name: name,
        client_id: "",
        client_secret: ""
      }
      const response = await api.client.create(name)
      result = await response.json()
      this.clients.push(result)
      this.sortClients()

      return result
    },
    async renameClient(clientId: string, newName: string) {
      api.client.rename(clientId, newName)
        .then(() => {
          const client = this.clients.find(item => {
            return item.client_id === clientId
          })

          if (client) {
            client.name = newName
          }
        })
    },
    async deleteClient(clientId: string) {
      api.client.delete(clientId)
        .then(() => {
          const index = this.clients.map(item => item.client_id).indexOf(clientId);
          this.clients.splice(index, 1);
        })
    },
    //
    async fetchConfig(clientId: string) {
      let result = { config: "", token: "" }
      await api.config.get(clientId)
        .then(async response => {
          result = await response.json()
        })

      return result
    }
  }
})
