let apiPassword = ""

const api = {
  getApiUrl: () => {
    // @ts-ignore
    return API_URL
  },
  setPassword(password: string) {
    apiPassword = password
  },
  async login(password: String): Promise<boolean> {
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

    return response.status === 200
  },
  // client
  async createClient(name: String) {
    const response = await fetch(`${api.getApiUrl()}/api/v1/client`, {
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

    return response.json()
  },
  async renameClient(clientId: String, newName: String) {
    const response = await fetch(`${api.getApiUrl()}/api/v1/client/${clientId}/name`, {
      method: "POST",
      mode: "cors",
      headers: {
        "Content-Type": "application/json",
      },
      referrerPolicy: "origin-when-cross-origin",
      body: JSON.stringify({
        name: newName
      })
    })

    return response.status === 200
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
    const response = await fetch(`${api.getApiUrl()}/api/v1/config/${clientId}`, {
      headers: {
        "Authorization": `Basic ${apiPassword}`
      }
    });
    return response.json()
  }
}

export default { ...api }
