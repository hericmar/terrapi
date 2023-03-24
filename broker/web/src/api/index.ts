import {useMainStore} from "@/store";

let apiPassword = ""

const api = {
  getApiUrl: () => {
    // @ts-ignore
    return API_URL
  },
  setToken(token: string) {
    apiPassword = token
  },
  async login(password: String) {
    const response = await fetch(`${api.getApiUrl()}/api/v1/login`, {
      method: "POST",
      mode: "cors",
      headers: {
        "Content-Type": "application/json",
      },
      referrerPolicy: "origin-when-cross-origin",
      body: JSON.stringify({
        password: password
      })
    })

    return response.json()
  },
  // client
  async createClient(name: String) {
    return fetch(`${api.getApiUrl()}/api/v1/client`, {
      method: "POST",
      mode: "cors",
      headers: {
        "Authorization": `Basic ${apiPassword}`,
        "Content-Type": "application/json",
      },
      referrerPolicy: "origin-when-cross-origin",
      body: JSON.stringify({
        name: name
      })
    })
  },
  renameClient(clientId: String, newName: String) {
    return fetch(`${api.getApiUrl()}/api/v1/client/${clientId}/name`, {
      method: "PUT",
      mode: "cors",
      headers: {
        "Authorization": `Basic ${apiPassword}`,
        "Content-Type": "application/json",
      },
      referrerPolicy: "origin-when-cross-origin",
      body: JSON.stringify({
        name: newName
      })
    })
  },
  deleteClient(clientId: string) {
    return fetch(`${api.getApiUrl()}/api/v1/client/${clientId}`, {
      method: "DELETE",
      mode: "cors",
      headers: {
        "Authorization": `Basic ${apiPassword}`,
        "Content-Type": "application/json",
      },
      referrerPolicy: "origin-when-cross-origin"
    })
  },
  listClientPreview: async () => {
    const response = await fetch(`${api.getApiUrl()}/api/v1/client/preview`);
    return response.json()
  },
  // record
  async listRecordsForClient(clientId: string, from: Date, to: Date) {
    const url = `${api.getApiUrl()}/api/v1/record/${clientId}?from=${from.toISOString()}&to=${to.toISOString()}`
    const response = await fetch(encodeURI(url));
    return response.json()
  },
  // config
  async fetchConfig(clientId: string) {
    return await fetch(`${api.getApiUrl()}/api/v1/config/${clientId}`, {
      headers: {
        "Authorization": `Basic ${apiPassword}`
      }
    })
  }
}

export default { ...api }
