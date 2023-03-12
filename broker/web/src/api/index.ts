const api = {
  getApiUrl: () => {
    // @ts-ignore
    return API_URL
  },
  listClientPreview: async () => {
    const response = await fetch(`${api.getApiUrl()}/api/v1/client/preview`);
    return response.json()
  },
  async listRecordsForClient(clientId: string, from: Date, to: Date) {
    const url = `${api.getApiUrl()}/api/v1/record/${clientId}?from=${from.toISOString()}&to=${to.toISOString()}`
    const response = await fetch(encodeURI(url));
    return response.json()
  }
}

export default { ...api }
