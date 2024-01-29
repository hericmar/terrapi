# TerraPi
Simple manager of environments such as terrariums, aquariums, or greenhouses.
The project consists of Raspberry Pi client implementation, 
a broker server that collects data, 
and a web interface that plots the data into graphs. 

## Requirements
- Raspberry Pi computer for client control unit (tested on Raspberry Pi Zero).
  Please note that Raspberry Pi 5 is not supported, because WiringPi library is not compatible with it.
- Optional: PostgreSQL database server for the broker.

## Instalation
TODO

## Configuration
### Supported sensors
| Name | Description |
| ---- | ----------- |
| DHT11 | Temperature and humidity sensor |
| HTU21D | Temperature and humidity sensor |

### Value types (Physical quantities)
Using only SI units.
Supported both in client and broker.

| Name | Unit | Description |
| ---- | ---- | ----------- |
| Humidity | % | Relative humidity |
| Signal | bool | Digital signal |
| Temperature | °C | Temperature |
