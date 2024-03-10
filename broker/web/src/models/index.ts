export type Client = {
  client_id: string,
  client_secret: string,
  name: string
}

export type Record = {
  timestamp: number,
  src: string,
  value: number
  quantity: Quantity
}

export type Event = {
  timestamp: number,
  source: string,
  state: number,
}

export enum Quantity {
  Humidity = 0,
  Signal = 1,
  Temperature = 2
}

export type Measurement = {
  timestamp: number
  source: string,
  quantity: Quantity,
  value: number,
}

export type Records = {
  events: Array<Event>,
  measurements: Array<Measurement>
}
