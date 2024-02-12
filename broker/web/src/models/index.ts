export type Client = {
  client_id: string,
  client_secret: string,
  name: string
}

export type Record = {
  timestamp: number,
  src: string,
  value: number
  quantity: number
}

export type Event = {
  timestamp: number,
  device_id: number,
  state: number,
}

export type Measurement = {
  timestamp: number
  source: string,
  quantity: number,
  value: number,
}

export type Records = {
  events: Array<Event>,
  measurements: Array<Measurement>
}
