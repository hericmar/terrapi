# Schema
## PUT /api/v1/config
```json
{
  "client_id": "EXAMPLE",
  "config": "TOML data"
}
```

## POST /api/v1/records
```json
{
  "client_id": "EXAMPLE",
  "measurements": [
    {
      "sensor_name": "dht11",
      "value": 1.99,
      "physical_quantity": 0,
      "timestamp": 1678139206
    }
  ],
  "events": [
    {
      "switch_name": "humidifier", 
      "state": 0,
      "timestamp": 1678139206
    }
  ]
}
```
