const apiRoot = "API_ROOT"

export async function fetchClients(adminPassword) {
    return await fetch(`${apiRoot}/client`, {
        headers: {
            'Authorization': adminPassword
        }
    })
}

export async function fetchConfigs() {
    return fetch(`${apiRoot}/config`)
}

export async function fetchMeasurements(clientID, requestOffset, requestLimit) {
    return await fetch(`${apiRoot}/measurement/${clientID}?offset=${requestOffset}&limit=${requestLimit}`)
}
