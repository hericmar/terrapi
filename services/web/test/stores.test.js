import {prepareData, processData} from "../src/stores";

describe("stores test", () => {
    test("processes test payload", () => {
        let res = {
            datasetsMap: {},
            times: [],
            datasets: []
        }

        const testConfig = {
            "data": [{
                "clientID": "aoRkHPRIXwdHqpcpxpQhacbmFyWkKXwr",
                "name": "terárko",
                "sensors": [{"name": "dht11", "type": "DHT11"}],
                "switches": [{"name": "lights", "gpio": 17}, {"name": "heater", "gpio": 27}, {
                    "name": "humidifier",
                    "gpio": 22
                }]
            }], "message": null, "status": "success"
        }

        const testPayload = {
            "data": [{
                "clientID": "aoRkHPRIXwdHqpcpxpQhacbmFyWkKXwr",
                "sensorName": "dht11",
                "value": "0.000000",
                "physicalQuantity": 0,
                "timestamp": "2022-08-09T01:07:16+02:00"
            }, {
                "clientID": "aoRkHPRIXwdHqpcpxpQhacbmFyWkKXwr",
                "sensorName": "dht11",
                "value": "0.000000",
                "physicalQuantity": 1,
                "timestamp": "2022-08-09T01:07:16+02:00"
            }, {
                "clientID": "aoRkHPRIXwdHqpcpxpQhacbmFyWkKXwr",
                "sensorName": "dht11",
                "value": "0.000000",
                "physicalQuantity": 0,
                "timestamp": "2022-08-09T01:07:00+02:00"
            }, {
                "clientID": "aoRkHPRIXwdHqpcpxpQhacbmFyWkKXwr",
                "sensorName": "dht11",
                "value": "0.000000",
                "physicalQuantity": 1,
                "timestamp": "2022-08-09T01:07:00+02:00"
            }, {
                "clientID": "aoRkHPRIXwdHqpcpxpQhacbmFyWkKXwr",
                "sensorName": "dht11",
                "value": "0.000000",
                "physicalQuantity": 0,
                "timestamp": "2022-08-09T01:06:44+02:00"
            }, {
                "clientID": "aoRkHPRIXwdHqpcpxpQhacbmFyWkKXwr",
                "sensorName": "dht11",
                "value": "0.000000",
                "physicalQuantity": 1,
                "timestamp": "2022-08-09T01:06:44+02:00"
            }, {
                "clientID": "aoRkHPRIXwdHqpcpxpQhacbmFyWkKXwr",
                "sensorName": "dht11",
                "value": "0.000000",
                "physicalQuantity": 0,
                "timestamp": "2022-08-09T01:06:28+02:00"
            }, {
                "clientID": "aoRkHPRIXwdHqpcpxpQhacbmFyWkKXwr",
                "sensorName": "dht11",
                "value": "0.000000",
                "physicalQuantity": 1,
                "timestamp": "2022-08-09T01:06:28+02:00"
            }], "message": null, "status": "success"
        };
        expect(testPayload.data.length === 8).toBeTruthy();

        prepareData(res, testConfig.data[0]);
        processData(res, testPayload.data);

        expect(res.datasetsMap["dht11.temperature"]).not.toBeUndefined();
        expect(res.datasetsMap["dht11.humidity"]).not.toBeUndefined();

        expect(res.times.length === 2).toBeTruthy();

        const i = res.datasetsMap["dht11.temperature"];
        expect(res.times[i].length === 4).toBeTruthy();

        const j = res.datasetsMap["dht11.humidity"];
        expect(res.times[j].length === 4).toBeTruthy();

        console.log(res.datasets)
    });
});
