import {useMainStore} from "@/store";

let apiPassword = ""

const doRequest = async (url: string, method: string, body?: any): Promise<Response> => {
  const headers = new Headers({
    "Content-Type": "application/json",
  })
  if (apiPassword) {
    headers.append("Authorization", `Basic ${apiPassword}`)
  }

  return new Promise((resolve, reject) => {
    fetch(url, {
      method: method,
      mode: "cors",
      headers,
      referrerPolicy: "origin-when-cross-origin",
      body: body ? JSON.stringify(body) : undefined
    })
      .then(async (response) => {
        if (response.status === 401) {
          // Unauthorized
          useMainStore().removeToken()
        }
        resolve(response)
      })
      .catch((error) => {
        useMainStore().snackbarMessage = "Failed to fetch data from server. You may be offline."
        // Stop propagation
        // reject(result)
    })
  })
}

const api = {
  getApiUrl: () => {
    if (import.meta.env.DEV) {
      return "http://localhost:8091"
    }
    return ""
  },
  setToken(token: string) {
    apiPassword = token
  },
  auth: {
    async login(password: String) {
      return await doRequest(`${api.getApiUrl()}/api/v1/auth/login`, "POST", {
        password: password
      })
    },
    async logout() {
      return doRequest(`${api.getApiUrl()}/api/v1/auth/logout`, "POST")
    }
  },
  client: {
    async create(name: string) {
      return doRequest(`${api.getApiUrl()}/api/v1/client`, "POST", {
        name
      })
    },
    async list() {
      return await doRequest(`${api.getApiUrl()}/api/v1/client/preview`, "GET")
    },
    async rename(clientId: string, newName: string) {
      return doRequest(`${api.getApiUrl()}/api/v1/client/${clientId}`, "PATCH", {
        name: newName
      })
    },
    async delete(clientId: string) {
      return doRequest(`${api.getApiUrl()}/api/v1/client/${clientId}`, "DELETE")
    }
  },
  record: {
    async listForClient(clientId: string, from: Date, to: Date) {
      const fromUnix = Math.floor(from.getTime())
      const toUnix = Math.floor(to.getTime())

      const url = `${api.getApiUrl()}/api/v1/record/?client_id=${clientId}&from=${fromUnix}&to=${toUnix}`
      const response = await fetch(encodeURI(url));
      return response.json()
    },
  },
  config: {
    async get(clientId: string) {
      return doRequest(`${api.getApiUrl()}/api/v1/config/${clientId}`, "GET")
    }
  },
}

export default { ...api }
