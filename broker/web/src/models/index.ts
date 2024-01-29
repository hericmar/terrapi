export type Client = {
  client_id: string,
  token: string,
  name: string
}

export type Record = {
  timestamp: number,
  src: string,
  value: number
  quantity: number
  client_id: string
}
