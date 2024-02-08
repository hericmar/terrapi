import {Measurement, Records} from "@/models";
import api from "@/api";

export const CODE_TO_NAME = new Map<number, string>([
  [0, 'Humidity'],
  [1, 'Signal'],
  [2, 'Temperature'],
])

export type DataSets = {
  // number is quantity
  measurements: Map<number, Array<Measurement>>,
}

/**
 * @param records
 * @returns Map<number, Array<Record>> - key is quantity, value is array of records with this quantity
 */
export function processRecords(records: Records): DataSets {
  // grouped by quantity
  const measurements = new Map<number, Array<Measurement>>()

  records.measurements.forEach((record) => {
    record.quantity = Math.round(record.quantity)

    // group by quantity
    if (measurements.has(record.quantity)) {
      measurements.get(record.quantity)?.push(record)
    } else {
      measurements.set(record.quantity, [record])
    }
  })

  return {
    measurements
  }
}

export const fetchRecords = async (clientId: string, now: Date): Promise<Records> => {
  const startOfDay = new Date(now.getFullYear(), now.getMonth(), now.getDate(), 0, 0, 0);

  // Set the time to one minute before midnight (23:59:59) for the current date
  const endOfDay = new Date(now.getFullYear(), now.getMonth(), now.getDate(), 24, 0, 0);

  return await api.record.listForClient(clientId, startOfDay, endOfDay)
}
