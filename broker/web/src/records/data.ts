import {Record} from "@/models";
import api from "@/api";

export const CODE_TO_NAME = new Map<number, string>([
  [0, 'Humidity'],
  [1, 'Signal'],
  [2, 'Temperature'],
])

/**
 * @param records
 * @returns Map<number, Array<Record>> - key is quantity, value is array of records with this quantity
 */
export const processRecords = (records: Array<Record>): Map<number, Array<Record>> => {
  // grouped by quantity
  const data = new Map<number, Array<Record>>()

  records.forEach((record) => {
    record.quantity = Math.round(record.quantity)

    if (data.has(record.quantity)) {
      data.get(record.quantity)?.push(record)
    } else {
      data.set(record.quantity, [record])
    }
  })

  return data
}

export const fetchRecords = async (clientId: string, now: Date): Promise<Array<Record>> => {
  const startOfDay = new Date(now.getFullYear(), now.getMonth(), now.getDate(), 0, 0, 0);

  // Set the time to one minute before midnight (23:59:59) for the current date
  const endOfDay = new Date(now.getFullYear(), now.getMonth(), now.getDate(), 24, 0, 0);

  return await api.record.listForClient(clientId, startOfDay, endOfDay)
}
